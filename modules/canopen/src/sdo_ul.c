#include "sdo.h"
#include "canopen.h"
#include "sdo_common.h"

void handle_ul(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst)
{
    sdo_hdr_tun     hdr_un = {.all_u32 = f_pst->mdl_un.all_u32};

    if (0 == (od_entry_pst->flags_u16 & OD_ATTR_READ))
    {
        sdo_abort(self, f_pst, r_pst, CO_SDO_ABORT_WRITE_ONLY_OBJECT);
    }
    else
    {   
        switch (od_entry_pst->size_u32)
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

            /* Have to use segmented upload if size is grater than 4. */
            default:
                hdr_un.bit_st.command_u8 = CO_SDO_UL_RESP_SEG_INIT_SI;
                r_pst->mdh_un.all_u32 = od_entry_pst->size_u32;

                self->active_obj_pst = od_entry_pst;
                self->segment_offset_u32 = 0;
                self->toggle_bit_u8 = 1;
                CRF_POST_TO_SELF(&sdo_seg_ul_start_event);
        }

        r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
        r_pst->mdl_un.all_u32 = hdr_un.all_u32;
        CRF_EMIT(r_pst);
    }
}

void process_ul_segment(chsm_tst *_self, const cevent_tst *e_pst)
{
    sdo_tst*        self = (sdo_tst*)_self;
    CRF_SIG_VAR(SIG_CAN_FRAME, f_pst, e_pst);
    uint8_t         size_u8;
    uint8_t         t_bit_u8 = (f_pst->mdl_un.bit_st.d0_u8 >> 4) & 1;
    uint8_t         last_u8;
    uint8_t*        src_pu8;
    uint8_t*        dst_pu8;

    /* Stop the transfer if the client command specifier is not a segment
     * request, and put the message back to the queue for processing.
     */
    if (!IS_CO_SDO_UL_SEG_REQ(f_pst->mdl_un.bit_st.d0_u8))
    {
        CRF_POST_TO_SELF(e_pst);
        CRF_POST_TO_SELF(&sdo_seg_ul_end_event);
        return;
    }

    can_frame_tst *r_pst = CRF_NEW(SIG_CAN_FRAME);

    /* Event allocation failed */
    if (NULL == r_pst) 
    {
        /* TODO: send statically allocated error frame */
        return;
    }

    size_u8 = min(7, self->active_obj_pst->size_u32 - self->segment_offset_u32);
    last_u8 = ((self->segment_offset_u32 + size_u8) >= self->active_obj_pst->size_u32) ? 1 : 0;

        /* Ignore frames with the same consecutive toggle bits */
    if (t_bit_u8 == self->toggle_bit_u8)
    {
        if (self->segment_offset_u32 >= 7)
        {
            self->segment_offset_u32 -= 7;
        }
    }
    
    self->toggle_bit_u8 = t_bit_u8;

    r_pst->header_un = CAN_HDR(CO_SDO_TX + self->config_st.node_id_u8, 0, 8);
    r_pst->mdl_un.all_u32 = CO_SDO_UL_RESP_SEG(t_bit_u8, size_u8, last_u8);
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

    if (last_u8)
    {
        CRF_POST_TO_SELF(&sdo_seg_ul_end_event);
    }
}