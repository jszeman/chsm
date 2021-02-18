#include "sdo_functions.h"
#include "sdo.h"
#include "od.h"
#include "canopen.h"
#include "can_if.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define CO_OD_MAX_RECURSION_LEVEL 4

static const cevent_tst sdo_seg_dl_start_event = {.sig = SIG_CANOPEN_SEG_DL_START};
static const cevent_tst sdo_seg_dl_end_event = {.sig = SIG_CANOPEN_SEG_DL_END};

typedef struct sdo_hdr_bits_tst
{
    uint32_t        command_u8:     8;
    uint32_t        index_u16:      16;
    uint32_t        subindex_u8:    8;
} sdo_hdr_bits_tst;

typedef union sdo_hdr_tun
{
    uint32_t        all_u32;
    sdo_hdr_bits_tst    bit_st;
} sdo_hdr_tun;

od_entry_tst* find_od_entry_recursive(object_dictionary_tst* od_pst, uint32_t mlx_u32, uint8_t level_u8)
{
    od_entry_tst* od_entry_pst;

    if (level_u8 >= CO_OD_MAX_RECURSION_LEVEL) return NULL;

    for (od_entry_pst = od_pst->objects_ast;
         od_entry_pst->size_u16;
         od_entry_pst++)
    {
        if (mlx_u32 == od_entry_pst->mlx_u32)
        {
            return od_entry_pst;
        }
        else if (od_entry_pst->flags_u16 & OD_ATTR_OBJECT_DICT)
        {
            od_entry_tst* o_pst;

            o_pst = find_od_entry_recursive((object_dictionary_tst* )(od_entry_pst->addr_u), mlx_u32, level_u8 + 1);

            if (NULL != o_pst)
            {
                return o_pst;
            }
        }
    }

    return NULL;
}

od_entry_tst* find_od_entry(sdo_tst* self, uint32_t mlx_u32)
{
    return find_od_entry_recursive(self->config_st.od_pst, mlx_u32, 0);
}

static void sdo_abort(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst* r_pst, uint32_t abort_code_u32)
{
    sdo_hdr_tun     hdr_un = {.all_u32 = f_pst->mdl_un.all_u32};
    
    r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
    hdr_un.bit_st.command_u8 = CO_SDO_ABORT;
    r_pst->mdl_un.all_u32 = hdr_un.all_u32;
    r_pst->mdh_un.all_u32 = abort_code_u32;

    CRF_EMIT(r_pst);
}

static void handle_exp_dl(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst, uint16_t len_u16)
{
    sdo_hdr_tun     hdr_un = {.all_u32 = f_pst->mdl_un.all_u32};

    if (0 == (od_entry_pst->flags_u16 & OD_ATTR_WRITE))
    {
        sdo_abort(self, f_pst, r_pst, CO_SDO_ABORT_READ_ONLY_OBJECT);
    }
    else if (len_u16 != od_entry_pst->size_u16)
    {
        sdo_abort(self, f_pst, r_pst, CO_SDO_ABORT_LENGTH_MISMATCH);
    }
    else
    {   
        switch (len_u16)
        {
            case 1:
                *((uint8_t *)(od_entry_pst->addr_u)) = f_pst->mdh_un.all_u32 & 0xff;
                break;

            case 2:
                *((uint16_t *)(od_entry_pst->addr_u)) = f_pst->mdh_un.all_u32 & 0xffff;
                break;

            case 4:
                *((uint32_t *)(od_entry_pst->addr_u)) = f_pst->mdh_un.all_u32;
                break;
        }

        hdr_un.bit_st.command_u8 = CO_SDO_DL_RESP_EXP;
        r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
        r_pst->mdl_un.all_u32 = hdr_un.all_u32;
        r_pst->mdh_un.all_u32 = 0;
        CRF_EMIT(r_pst);
    }
}

static void handle_exp_ul(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst)
{
    sdo_hdr_tun     hdr_un = {.all_u32 = f_pst->mdl_un.all_u32};

    if (0 == (od_entry_pst->flags_u16 & OD_ATTR_READ))
    {
        sdo_abort(self, f_pst, r_pst, CO_SDO_ABORT_WRITE_ONLY_OBJECT);
    }
    else
    {   
        switch (od_entry_pst->size_u16)
        {
            case 1:
                r_pst->mdh_un.all_u32 = *((uint8_t *)(od_entry_pst->addr_u));
                hdr_un.bit_st.command_u8 = CO_SDO_UL_RESP_EXP_1B;
                break;

            case 2:
                r_pst->mdh_un.all_u32 = *((uint16_t *)(od_entry_pst->addr_u));
                hdr_un.bit_st.command_u8 = CO_SDO_UL_RESP_EXP_2B;
                break;

            case 4:
                r_pst->mdh_un.all_u32 = *((uint32_t *)(od_entry_pst->addr_u));
                hdr_un.bit_st.command_u8 = CO_SDO_UL_RESP_EXP_4B;
                break;
        }

        r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
        r_pst->mdl_un.all_u32 = hdr_un.all_u32;
        CRF_EMIT(r_pst);
    }
}

static void handle_seg_dl(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst)
{
    sdo_hdr_tun     hdr_un = {.all_u32 = f_pst->mdl_un.all_u32};

    if (0 == (od_entry_pst->flags_u16 & OD_ATTR_WRITE))
    {
        sdo_abort(self, f_pst, r_pst, CO_SDO_ABORT_READ_ONLY_OBJECT);
    }
    else
    {
        hdr_un.bit_st.command_u8 = CO_SDO_DL_RESP_EXP;
        r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
        r_pst->mdl_un.all_u32 = hdr_un.all_u32;
        r_pst->mdh_un.all_u32 = 0;
        CRF_EMIT(r_pst);

        self->active_obj_pst = od_entry_pst;
        self->segment_offset_u16 = 0;
        self->toggle_bit_u8 = 0;
        CRF_POST_TO_SELF(&sdo_seg_dl_start_event);
    }
}

void process_dl_segment(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;
    CRF_SIG_VAR(SIG_CAN_FRAME, f_pst, e_pst);
    uint8_t         size_u8 = 7 - ((f_pst->mdl_un.bit_st.d0_u8 >> 1) & 7);
    uint8_t         t_bit_u8 = (f_pst->mdl_un.bit_st.d0_u8 >> 4) & 1;
    uint8_t         last_u8 = f_pst->mdl_un.bit_st.d0_u8 & 1;
    uint8_t*        src_pu8;
    uint8_t*        dst_pu8;
    
    /* TODO: abort if toggle bit error */
    /* TODO: abort if not a segment */

	src_pu8 = (uint8_t *)(&f_pst->mdl_un.all_u32);
	src_pu8++;

    dst_pu8 = (uint8_t *)(self->active_obj_pst->addr_u + self->segment_offset_u16);
    self->segment_offset_u16 += size_u8;

	while (size_u8--)
	{
		*dst_pu8++ = *src_pu8++;
	}

    can_frame_tst *r_pst = CRF_NEW(SIG_CAN_FRAME);

    /* Event allocation failed */
    if (NULL == r_pst) 
    {
        /* TODO: send statically allocated error frame */
        return;
    }

    r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
    r_pst->mdl_un.all_u32 = CO_SDO_DL_SEG_RESP(t_bit_u8);
    r_pst->mdh_un.all_u32 = 0;
    CRF_EMIT(r_pst);

    if (last_u8)
    {
        CRF_POST_TO_SELF(&sdo_seg_dl_end_event);
    }

}

void process_sdo_request(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;
    CRF_SIG_VAR(SIG_CAN_FRAME, f_pst, e_pst);
    od_entry_tst*   od_entry_pst = NULL;
    sdo_hdr_tun     hdr_un = {.all_u32 = f_pst->mdl_un.all_u32};
    uint32_t        mlx_u32;

    mlx_u32 = hdr_un.bit_st.index_u16;
    mlx_u32 <<= 8;
    mlx_u32 |= hdr_un.bit_st.subindex_u8;

    can_frame_tst *r_pst = CRF_NEW(SIG_CAN_FRAME);

    /* Event allocation failed */
    if (NULL == r_pst) 
    {
        /* TODO: send statically allocated error frame */
        return;
    }

    /* Object dictionary is not available */
    if (NULL == self->config_st.od_pst)
    {
        sdo_abort(self, f_pst, r_pst, CO_SDO_ABORT_NO_OBJ_DICT);
        return;
    }

    od_entry_pst = find_od_entry(self, mlx_u32);

    /* Object not found */
    if (NULL == od_entry_pst)
    {
        sdo_abort(self, f_pst, r_pst, CO_SDO_ABORT_OBJECT_NOT_FOUND);
        return;
    }

    switch(hdr_un.bit_st.command_u8)
    {
        case CO_SDO_DL_REQ_EXP_1B:
            handle_exp_dl(self, f_pst, r_pst, od_entry_pst, 1);
            break;

        case CO_SDO_DL_REQ_EXP_2B:
            handle_exp_dl(self, f_pst, r_pst, od_entry_pst, 2);
            break;

        case CO_SDO_DL_REQ_EXP_4B:
            handle_exp_dl(self, f_pst, r_pst, od_entry_pst, 4);
            break;

        case CO_SDO_UL_REQ:
            handle_exp_ul(self, f_pst, r_pst, od_entry_pst);
            break;

        case CO_SDO_DL_REQ_SEG_INIT:
        case CO_SDO_DL_REQ_SEG_INIT_SI:
            handle_seg_dl(self, f_pst, r_pst, od_entry_pst);
            break;

        default:
            CRF_GC(r_pst);
    }
}

void send_sdo_exp_dl_abort(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst* self = (sdo_tst*)_self;
}

void send_sdo_exp_dl_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst* self = (sdo_tst*)_self;
}

void send_sdo_exp_ul_abort(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst* self = (sdo_tst*)_self;
}

void send_sdo_exp_ul_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst* self = (sdo_tst*)_self;
}

void send_sdo_abort(chsm_tst *_self, const cevent_tst *e_pst, uint32_t abort_code_u32)
{
    sdo_tst* self = (sdo_tst*)_self;
}

bool is_abort_request(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst* self = (sdo_tst*)_self;

    return false;
}

bool is_not_abort_request(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst* self = (sdo_tst*)_self;

    return false;
}

bool sdo_timeout(chsm_tst *_self, const cevent_tst *e_pst, uint32_t timeout_u32)
{
    sdo_tst* self = (sdo_tst*)_self;
    
    return false;
}