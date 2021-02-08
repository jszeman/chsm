#include "canopen.h"
#include "crf.h"
#include "can_if.h"
#include "signals.h"
#include <stdio.h>

void co_send_bootup(chsm_tst *_self, const cevent_tst *e_pst)
{
    co_node_tst *self = (co_node_tst *)_self;
    can_frame_tst *f_pst = CRF_NEW(SIG_CAN_FRAME);

    if (NULL == f_pst) return;

    f_pst->header_un = CAN_HDR(CO_NMT + self->config_st.node_id_u8, 0, 1);

    CRF_EMIT(f_pst);
}

void send_ng_resp(co_node_tst *self)
{
    can_frame_tst *f_pst = CRF_NEW(SIG_CAN_FRAME);

    if (NULL == f_pst) return;

    f_pst->header_un = CAN_HDR(CO_NMT + self->config_st.node_id_u8, 0, 1);
    f_pst->mdl_un.bit_st.d0_u8 = 0x7f | (self->ng_toggle_state_b ? 0x80 : 0);

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

void execute_nmt_cmd(co_node_tst *self, TYPEOF(SIG_CAN_FRAME) *f_pst)
{
    switch(f_pst->mdl_un.bit_st.d0_u8)
    {
        case CO_NMT_CMD_START:
            break;

        case CO_NMT_CMD_STOP:
            break;
            
        case CO_NMT_CMD_PREOP:
            break;
            
        case CO_NMT_CMD_RESET:
            if (self->config_st.on_nmt_reset)
            {
                self->config_st.on_nmt_reset(self->config_st.user_param_pv);
            }
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
    }
    else if (is_nmt_cmd(self, f_pst))
    {
        execute_nmt_cmd(self, f_pst);
    }

}

