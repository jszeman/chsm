#include "sdo_functions.h"
#include "sdo.h"
#include "od.h"
#include "canopen.h"
#include "can_if.h"
#include <stdio.h>
#include <stddef.h>

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

od_entry_tst* find_od_entry(sdo_tst* self, uint32_t mlx_u32)
{
    od_entry_tst* od_entry_pst;;

    for (od_entry_pst = self->config_st.od_pst->objects_ast;
         od_entry_pst->size_u16;
         od_entry_pst++)
    {
        if (mlx_u32 == od_entry_pst->mlx_u32) return od_entry_pst;
    }
    
    return NULL;
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
            if (0 == (od_entry_pst->flags_u16 & OD_ATTR_WRITE))
            {
                sdo_abort(self, f_pst, r_pst, CO_SDO_ABORT_READ_ONLY_OBJECT);
                return;
            }
            else
            {
                *((uint8_t *)(od_entry_pst->addr_u)) = f_pst->mdh_un.bit_st.d4_u8;
                hdr_un.bit_st.command_u8 = CO_SDO_DL_RESP_EXP;
                r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
                r_pst->mdl_un.all_u32 = hdr_un.all_u32;
                r_pst->mdh_un.all_u32 = 0;
                CRF_EMIT(r_pst);
                return;
            }
            
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