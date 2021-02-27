#include "sdo.h"
#include "canopen.h"
#include "sdo_common.h"
#include <stdio.h>

void handle_block_ul_init(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst)
{
    sdo_hdr_tun     hdr_un = {.all_u32 = f_pst->mdl_un.all_u32};

    if (0 == (od_entry_pst->flags_u16 & OD_ATTR_READ))
    {
        sdo_abort(self, f_pst, r_pst, CO_SDO_ABORT_WRITE_ONLY_OBJECT);
    }
    else
    {
        self->block_size_u8 =  f_pst->mdh_un.bit_st.d4_u8;

        hdr_un.bit_st.command_u8 = CO_SDO_UL_RESP_BLK_INIT(0);
        r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
        r_pst->mdl_un.all_u32 = hdr_un.all_u32;
        r_pst->mdh_un.all_u32 = od_entry_pst->size_u32;
        CRF_EMIT(r_pst);

        self->active_obj_pst = od_entry_pst;
        self->segment_offset_u32 = 0;
        self->toggle_bit_u8 = 1;
        CRF_POST_TO_SELF(&sdo_block_ul_start_event);
    }
}

void send_next_block(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;
    uint8_t         size_u8;
    uint8_t         last_u8 = 0;
    uint8_t*        src_pu8;
    uint8_t*        dst_pu8;

    for (uint16_t i=0; (i<self->block_size_u8) && (!last_u8); i++)
    {
        can_frame_tst *r_pst = CRF_NEW(SIG_CAN_FRAME);

        /* Event allocation failed */
        if (NULL == r_pst) 
        {
            /* TODO: send statically allocated error frame */
            printf("\nEvent allocation failed in process_ul_block, cycle %d\n", i);
            return;
        }

        size_u8 = min(7, self->active_obj_pst->size_u32 - self->segment_offset_u32);
        last_u8 = ((self->segment_offset_u32 + size_u8) >= self->active_obj_pst->size_u32) ? 1 : 0;

        if (last_u8)
        {
            self->last_block_msg_size_u8 = size_u8;
        }

        r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
        r_pst->mdl_un.all_u32 = CO_SDO_UL_RESP_BLK_SUBBLOCK(last_u8, i);
        r_pst->mdh_un.all_u32 = 0;    

        dst_pu8 = (uint8_t *)(&r_pst->mdl_un.all_u32);
        dst_pu8++;

        src_pu8 = (uint8_t *)(self->active_obj_pst->addr_u + self->segment_offset_u32);
        self->segment_offset_u32 += size_u8;

        while (size_u8--)
        {
            *dst_pu8++ = *src_pu8++;
        }

        CRF_EMIT(r_pst);

    }
}

void send_block_finish(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;

    can_frame_tst *r_pst = CRF_NEW(SIG_CAN_FRAME);

    /* Event allocation failed */
    if (NULL == r_pst) 
    {
        /* TODO: send statically allocated error frame */
        printf("\nEvent allocation failed in send_block_finish, cycle\n");
        return;
    }

    r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
    r_pst->mdl_un.all_u32 = CO_SDO_UL_BLK_FINISH(self->last_block_msg_size_u8);
    r_pst->mdh_un.all_u32 = 0;

    CRF_EMIT(r_pst);
}


void process_block_finish_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;
    CRF_SIG_VAR(SIG_CAN_FRAME, f_pst, e_pst);

    if ((CO_SDO_UL_BLK_FINISH_RESP != f_pst->mdl_un.all_u32) ||
        f_pst->mdh_un.all_u32)
    {
        if (CO_SDO_ABORT == f_pst->mdl_un.bit_st.d0_u8)
        {
            CRF_POST_TO_SELF(&sdo_block_ul_end_event);
            return;
        }
        else
        {
            send_sdo_abort(_self, e_pst, CO_SDO_ABORT_INVALID_COMMAND);
        }
    }
}

bool process_ul_block_ack(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;
    CRF_SIG_VAR(SIG_CAN_FRAME, f_pst, e_pst);

    if (CO_SDO_UL_RESP_BLK_ACK != f_pst->mdl_un.bit_st.d0_u8)
    {
        if (CO_SDO_ABORT != f_pst->mdl_un.bit_st.d0_u8)
        {
            send_sdo_abort(_self, e_pst, CO_SDO_ABORT_INVALID_COMMAND);
        }
        CRF_POST_TO_SELF(&sdo_block_ul_end_event);
        return false;
    }
    
    self->block_size_u8 =  f_pst->mdl_un.bit_st.d2_u8;

    if (self->segment_offset_u32 >= self->active_obj_pst->size_u32)
    {
        CRF_POST_TO_SELF(&sdo_block_ul_finish_event);
        return false;
    }

    return true;
}

void process_ul_block_start(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;
    CRF_SIG_VAR(SIG_CAN_FRAME, f_pst, e_pst);
    uint8_t         size_u8;
    uint8_t         last_u8;
    uint8_t*        src_pu8;
    uint8_t*        dst_pu8;

    if (CO_SDO_UL_REQ_BLK_START != f_pst->mdl_un.bit_st.d0_u8)
    {
        if (CO_SDO_ABORT != f_pst->mdl_un.bit_st.d0_u8)
        {
            send_sdo_abort(_self, e_pst, CO_SDO_ABORT_INVALID_COMMAND);
        }
        CRF_POST_TO_SELF(&sdo_block_ul_end_event);
        return;
    }


    CRF_POST_TO_SELF(&sdo_block_ul_started_event);
}