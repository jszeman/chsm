#include "sdo.h"
#include "canopen.h"
#include "sdo_common.h"

void handle_exp_dl(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst, uint16_t len_u16)
{
    sdo_hdr_tun     hdr_un = {.all_u32 = f_pst->mdl_un.all_u32};

    if (0 == (od_entry_pst->flags_u16 & OD_ATTR_WRITE))
    {
        sdo_abort(self, f_pst, r_pst, CO_SDO_ABORT_READ_ONLY_OBJECT);
    }
    else if (len_u16 != od_entry_pst->size_u32)
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

void handle_seg_dl(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst)
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
        self->segment_offset_u32 = 0;
        self->toggle_bit_u8 = 1;
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

    /* Abort the transfer if the client command specifier is not a segment
     * request, and put the message back to the queue for processing.
     */
    if (!IS_CO_SDO_DL_SEG_REQ(f_pst->mdl_un.bit_st.d0_u8))
    {
        CRF_POST_TO_SELF(e_pst);
        CRF_POST_TO_SELF(&sdo_seg_dl_end_event);
        return;
    }
    
    /* Ignore frames with the same consecutive toggle bits */
    if (t_bit_u8 == self->toggle_bit_u8)
    {
        return;
    }

    self->toggle_bit_u8 = t_bit_u8;

    if ((self->segment_offset_u32 + size_u8) > self->active_obj_pst->size_u32)
    {
        send_sdo_abort(_self, e_pst, CO_SDO_ABORT_LENGTH_TOO_HIGH);
        CRF_POST_TO_SELF(&sdo_seg_dl_end_event);
        return;
    }

	src_pu8 = (uint8_t *)(&f_pst->mdl_un.all_u32);
	src_pu8++;

    dst_pu8 = (uint8_t *)(self->active_obj_pst->addr_u + self->segment_offset_u32);
    self->segment_offset_u32 += size_u8;

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