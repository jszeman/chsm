#include "canopen.h"
#include "crf.h"
#include "can_if.h"
#include "sys_if.h"
#include <stdio.h>

static const cevent_tst ng_active = {.sig=SIG_CANOPEN_NG_ACTIVE};
static const cevent_tst ng_inactive = {.sig=SIG_CANOPEN_NG_INACTIVE};
static const cevent_tst ng = {.sig=SIG_I_CANOPEN_NG};

void co_send_bootup(chsm_tst *_self, const cevent_tst *e_pst)
{
    co_node_tst *self = (co_node_tst *)_self;
    can_frame_tst *f_pst = CRF_NEW(SIG_CAN_FRAME);

    if (NULL == f_pst) return;

    f_pst->header_un = CAN_HDR(CO_NMT + self->config_st.node_id_u8, 0, 1);
    f_pst->mdl_un.bit_st.d0_u8 = CO_NMT_STATE_BOOTUP;

    CRF_EMIT(f_pst);
}

void co_node_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    co_node_tst *self = (co_node_tst *)_self;
    
    self->nmt_state_u8 = CO_NMT_STATE_PREOP;

	chsm_ctor(&self->sdo_st.super, sdo_top, self->sdo_events_ast, 4, 2);

    CRF_SEND_FUNC(&self->sdo_st) = CRF_SEND_FUNC(self);

    chsm_init(&self->sdo_st.super);

    self->sdo_st.config_st.od_pst = self->config_st.od_pst;
    self->sdo_st.config_st.node_id_u8 = self->config_st.node_id_u8;
    self->sdo_st.config_st.timeout_ms_u32 = self->config_st.sdo_timeout_ms_u32;

    self->timer_u32 = 0;

    self->super.next = (chsm_tst *)&self->sdo_st;
}

void send_ng_resp(co_node_tst *self)
{
    can_frame_tst *f_pst = CRF_NEW(SIG_CAN_FRAME);

    if (NULL == f_pst) return;

    f_pst->header_un = CAN_HDR(CO_NMT + self->config_st.node_id_u8, 0, 1);
    f_pst->mdl_un.bit_st.d0_u8 = self->nmt_state_u8 | (self->ng_toggle_state_b ? 0x80 : 0);

    self->ng_toggle_state_b = !self->ng_toggle_state_b;

    CRF_EMIT(f_pst);
}

static inline bool is_ng_rtr(co_node_tst *self, TYPEOF(SIG_CAN_FRAME) *f_pst)
{
    return f_pst->header_un.all_u32 == CAN_HDR(CO_NMT + self->config_st.node_id_u8, 1, 0).all_u32;
}

static inline bool is_nmt_cmd(co_node_tst *self, TYPEOF(SIG_CAN_FRAME) *f_pst)
{
    return (f_pst->header_un.all_u32 == CAN_HDR(CO_NMT_BROADCAST_ID, 0, 2).all_u32) && (self->config_st.node_id_u8 == f_pst->mdl_un.bit_st.d1_u8);
}

static inline bool is_sdo(co_node_tst *self, TYPEOF(SIG_CAN_FRAME) *f_pst)
{
    return f_pst->header_un.all_u32 == CAN_HDR(CO_SDO_RX + self->config_st.node_id_u8, 0, 8).all_u32;
}

static inline void forward_sdo(co_node_tst *self, TYPEOF(SIG_CAN_FRAME) *f_pst)
{
    CRF_POST(f_pst, &self->sdo_st);
}

void execute_nmt_cmd(co_node_tst *self, TYPEOF(SIG_CAN_FRAME) *f_pst)
{
    switch(f_pst->mdl_un.bit_st.d0_u8)
    {
        case CO_NMT_CMD_START:
            self->nmt_state_u8 = CO_NMT_STATE_OPERATIONAL;
            break;

        case CO_NMT_CMD_STOP:
            self->nmt_state_u8 = CO_NMT_STATE_STOPPED;
            break;
            
        case CO_NMT_CMD_PREOP:
            self->nmt_state_u8 = CO_NMT_STATE_PREOP;
            break;
            
        case CO_NMT_CMD_RESET:
            CRF_EMIT(&sys_event_reset_st);
            break;
            
        case CO_NMT_CMD_RESET_COMM:
            break;
            
        default:
            break;
    }
}

void co_process_frame(chsm_tst *_self, const cevent_tst *e_pst)
{
    co_node_tst *self = (co_node_tst *)_self;
    CRF_SIG_VAR(SIG_CAN_FRAME, f_pst, e_pst);

    if(is_ng_rtr(self, f_pst))
    {
        send_ng_resp(self);
        CRF_POST_TO_SELF(&ng);
    }
    else if (is_nmt_cmd(self, f_pst))
    {
        execute_nmt_cmd(self, f_pst);
    }
    else if (is_sdo(self, f_pst))
    {
        forward_sdo(self, f_pst);
    }
}

void co_callback(chsm_tst *_self, const cevent_tst *e_pst)
{
    co_node_tst *self = (co_node_tst *)_self;
    self->timer_u32++;

    CRF_POST(e_pst, &self->sdo_st);
}

void co_reset_timer(chsm_tst *_self, const cevent_tst *e_pst)
{
    co_node_tst *self = (co_node_tst *)_self;
    self->timer_u32 = 0;
}

void co_send_ng_active(chsm_tst *_self, const cevent_tst *e_pst)
{
    co_node_tst *self = (co_node_tst *)_self;

    CRF_EMIT(&ng_active);
}

void co_send_ng_inactive(chsm_tst *_self, const cevent_tst *e_pst)
{
    co_node_tst *self = (co_node_tst *)_self;

    CRF_EMIT(&ng_inactive);
}

bool co_timeout(chsm_tst *_self, const cevent_tst *e_pst, uint32_t timeout_u32)
{
    co_node_tst *self = (co_node_tst *)_self;

    return self->timer_u32 >= timeout_u32;
}

