#include "sdo_functions.h"
#include "sdo.h"
#include "od.h"
#include "can_if.h"
#include "canopen.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>



static const cevent_tst sdo_seg_dl_start_event =        {.sig = SIG_CANOPEN_SEG_DL_START};
static const cevent_tst sdo_seg_dl_end_event =          {.sig = SIG_CANOPEN_SEG_DL_END};
static const cevent_tst sdo_seg_ul_start_event =        {.sig = SIG_CANOPEN_SEG_UL_START};
static const cevent_tst sdo_seg_ul_end_event =          {.sig = SIG_CANOPEN_SEG_UL_END};
static const cevent_tst sdo_block_ul_start_event =      {.sig = SIG_CANOPEN_BLOCK_UL_START};
static const cevent_tst sdo_block_ul_end_event =        {.sig = SIG_CANOPEN_BLOCK_UL_END};
static const cevent_tst sdo_block_ul_started_event =    {.sig = SIG_CANOPEN_BLOCK_UL_STARTED};
static const cevent_tst sdo_block_ul_finish_event =     {.sig = SIG_CANOPEN_BLOCK_UL_FINISH};
static const cevent_tst sdo_block_dl_start_event =      {.sig = SIG_CANOPEN_BLOCK_DL_START};
static const cevent_tst sdo_block_dl_end_event =        {.sig = SIG_CANOPEN_BLOCK_DL_END};
static const cevent_tst sdo_block_dl_finish_event =     {.sig = SIG_CANOPEN_BLOCK_DL_FINISH};


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

static void handle_ul(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst)
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
        self->segment_offset_u32 = 0;
        self->toggle_bit_u8 = 1;
        CRF_POST_TO_SELF(&sdo_seg_dl_start_event);
    }
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

static inline uint8_t min(uint8_t a, uint16_t b)
{
    return (a < b) ? a : b; 
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

static void handle_block_ul_init(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst)
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

void process_dl_subblock(chsm_tst *_self, const cevent_tst *e_pst)
{

    sdo_tst*        self = (sdo_tst*)_self;
    CRF_SIG_VAR(SIG_CAN_FRAME, f_pst, e_pst);
    uint8_t         subblock_idx_u8 = f_pst->mdl_un.bit_st.d0_u8 & 0x7f;
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

static void handle_block_dl_init(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst)
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