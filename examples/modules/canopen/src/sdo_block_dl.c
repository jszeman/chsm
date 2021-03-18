#include "sdo_functions.h"
#include "sdo.h"
#include "od.h"
#include "can_if.h"
#include "canopen.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "sdo_common.h"

void handle_block_dl_init(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst)
{
    sdo_hdr_tun     hdr_un = {.all_u32 = f_pst->mdl_un.all_u32};

    if (0 == (od_entry_pst->flags_u16 & OD_ATTR_WRITE))
    {
        sdo_abort(self, f_pst, r_pst, CO_SDO_ABORT_READ_ONLY_OBJECT);
    }
    else
    {
        hdr_un.bit_st.command_u8 = CO_SDO_DL_RESP_BLK_INIT(0);
        r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
        r_pst->mdl_un.all_u32 = hdr_un.all_u32;
        r_pst->mdh_un.all_u32 = 7;
        CRF_EMIT(r_pst);

        self->active_obj_pst = od_entry_pst;
        self->segment_offset_u32 = 0;
        self->toggle_bit_u8 = 1;
        self->subblock_counter_u8 = 0;
        CRF_POST_TO_SELF(&sdo_block_dl_start_event);
    }
}

void process_dl_subblock(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;
    CRF_SIG_VAR(SIG_CAN_FRAME, f_pst, e_pst);
    uint8_t         subblock_idx_u8 = (f_pst->mdl_un.bit_st.d0_u8 & 0x7f) - 1;
    uint8_t         last_u8 = f_pst->mdl_un.bit_st.d0_u8 & 0x80;
    uint8_t*        src_pu8;
    uint8_t*        dst_pu8;

    /* Abort the transfer if the client command specifier is not a segment
     * request, and put the message back to the queue for processing.
     */
    if (subblock_idx_u8 >= SDO_BLOCK_SIZE)
    {
        send_sdo_abort(_self, e_pst, CO_SDO_ABORT_INVALID_SEQUENCE_NUM);
        CRF_POST_TO_SELF(&sdo_block_dl_end_event);
        return;
    }

    if (subblock_idx_u8 > self->subblock_counter_u8)
    {
        /* TODO: we lost a frame */
        printf("\nMissing frame\n");
    }
    else if (subblock_idx_u8 < self->subblock_counter_u8)
    {
        /* We received a repeated frame */
        return;
    }
    else
    {
        self->subblock_counter_u8++;
    }

    
/*
    if ((self->segment_offset_u32 + size_u8) > self->active_obj_pst->size_u32)
    {
        send_sdo_abort(_self, e_pst, CO_SDO_ABORT_LENGTH_TOO_HIGH);
        CRF_POST_TO_SELF(&sdo_seg_dl_end_event);
        return;
    }
*/

	src_pu8 = (uint8_t *)(&f_pst->mdl_un.all_u32);
	src_pu8++;

    dst_pu8 = (uint8_t *)(self->block_buffer_au8 + 7 * subblock_idx_u8);


	for (uint16_t i=0; i<7; i++)
	{
		*dst_pu8++ = *src_pu8++;
	}

    if (((SDO_BLOCK_SIZE - 1) == subblock_idx_u8) || last_u8)
    {
        can_frame_tst *r_pst = CRF_NEW(SIG_CAN_FRAME);

        /* Event allocation failed */
        if (NULL == r_pst) 
        {
            /* TODO: send statically allocated error frame */
            return;
        }

        r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
        r_pst->mdl_un.all_u32 = CO_SDO_DL_REQ_BLK_ACK;
        r_pst->mdl_un.bit_st.d1_u8 = subblock_idx_u8;
        r_pst->mdl_un.bit_st.d2_u8 = 7;
        r_pst->mdh_un.all_u32 = 0;
        CRF_EMIT(r_pst);

        if (0 == last_u8)
        {
            src_pu8 = self->block_buffer_au8;
            dst_pu8 = (uint8_t *)(self->active_obj_pst->addr_u + self->segment_offset_u32);
            self->segment_offset_u32 += SDO_BLOCK_SIZE * 7;

            for (uint16_t i=0; i<(SDO_BLOCK_SIZE * 7); i++)
            {
                *dst_pu8++ = *src_pu8++;
            }
        }
        else
        {
            self->last_block_msg_size_u8 = subblock_idx_u8;
            CRF_POST_TO_SELF(&sdo_block_dl_finish_event);
        }
        
        self->subblock_counter_u8 = 0;
    }
}

void process_block_dl_end(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;
    CRF_SIG_VAR(SIG_CAN_FRAME, f_pst, e_pst);
    uint8_t*        src_pu8;
    uint8_t*        dst_pu8;
    uint8_t         last_frame_size_u8 = 7 - ((f_pst->mdl_un.bit_st.d0_u8 >> 2) & 7);


    src_pu8 = self->block_buffer_au8;
    dst_pu8 = (uint8_t *)(self->active_obj_pst->addr_u + self->segment_offset_u32);
    self->segment_offset_u32 += SDO_BLOCK_SIZE * 7;

    for (uint16_t i=0; i<(self->last_block_msg_size_u8 * 7 + last_frame_size_u8); i++)
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
    r_pst->mdl_un.all_u32 = CO_SDO_DL_BLK_FINISH_RESP;
    r_pst->mdh_un.all_u32 = 0;
    CRF_EMIT(r_pst);
}