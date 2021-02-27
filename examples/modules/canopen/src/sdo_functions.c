#include "sdo_functions.h"
#include "sdo.h"
#include "od.h"
#include "can_if.h"
#include "canopen.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "sdo_common.h"

const cevent_tst sdo_seg_dl_start_event =        {.sig = SIG_CANOPEN_SEG_DL_START};
const cevent_tst sdo_seg_dl_end_event =          {.sig = SIG_CANOPEN_SEG_DL_END};
const cevent_tst sdo_seg_ul_start_event =        {.sig = SIG_CANOPEN_SEG_UL_START};
const cevent_tst sdo_seg_ul_end_event =          {.sig = SIG_CANOPEN_SEG_UL_END};
const cevent_tst sdo_block_ul_start_event =      {.sig = SIG_CANOPEN_BLOCK_UL_START};
const cevent_tst sdo_block_ul_end_event =        {.sig = SIG_CANOPEN_BLOCK_UL_END};
const cevent_tst sdo_block_ul_started_event =    {.sig = SIG_CANOPEN_BLOCK_UL_STARTED};
const cevent_tst sdo_block_ul_finish_event =     {.sig = SIG_CANOPEN_BLOCK_UL_FINISH};
const cevent_tst sdo_block_dl_start_event =      {.sig = SIG_CANOPEN_BLOCK_DL_START};
const cevent_tst sdo_block_dl_end_event =        {.sig = SIG_CANOPEN_BLOCK_DL_END};
const cevent_tst sdo_block_dl_finish_event =     {.sig = SIG_CANOPEN_BLOCK_DL_FINISH};

void sdo_abort(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst* r_pst, uint32_t abort_code_u32)
{
    sdo_hdr_tun     hdr_un = {.all_u32 = f_pst->mdl_un.all_u32};
    
    r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
    hdr_un.bit_st.command_u8 = CO_SDO_ABORT;
    r_pst->mdl_un.all_u32 = hdr_un.all_u32;
    r_pst->mdh_un.all_u32 = abort_code_u32;

    CRF_EMIT(r_pst);
}

void sdo_callback(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;

    self->counter_ms_u32++;
}

void sdo_reset_timer(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;

    self->counter_ms_u32 = 0;
}

void sdo_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;

    self->counter_ms_u32 = 0;
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
            handle_ul(self, f_pst, r_pst, od_entry_pst);
            break;

        case CO_SDO_DL_REQ_SEG_INIT:
        case CO_SDO_DL_REQ_SEG_INIT_SI:
            handle_seg_dl(self, f_pst, r_pst, od_entry_pst);
            break;

        case CO_SDO_UL_REQ_BLK_INIT(0):
        case CO_SDO_UL_REQ_BLK_INIT(1):
            handle_block_ul_init(self, f_pst, r_pst, od_entry_pst);
            break;

        case CO_SDO_DL_REQ_BLK_INIT(0, 0):
        case CO_SDO_DL_REQ_BLK_INIT(0, 1):
        case CO_SDO_DL_REQ_BLK_INIT(1, 0):
        case CO_SDO_DL_REQ_BLK_INIT(1, 1):
            handle_block_dl_init(self, f_pst, r_pst, od_entry_pst);
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
    sdo_hdr_tun hdr_un;

    can_frame_tst *r_pst = CRF_NEW(SIG_CAN_FRAME);

    /* Event allocation failed */
    if (NULL == r_pst) 
    {
        /* TODO: send statically allocated error frame */
        return;
    }

    r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
    hdr_un.bit_st.command_u8 = CO_SDO_ABORT;
    hdr_un.bit_st.index_u16 = (self->active_obj_pst->mlx_u32 >> 8) & 0xffff;
    hdr_un.bit_st.subindex_u8 = self->active_obj_pst->mlx_u32 & 0xff;
    r_pst->mdl_un.all_u32 = hdr_un.all_u32;
    r_pst->mdh_un.all_u32 = abort_code_u32;

    CRF_EMIT(r_pst);
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
    
    return self->counter_ms_u32 >= timeout_u32;
}