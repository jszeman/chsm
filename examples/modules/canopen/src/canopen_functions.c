#include "canopen.h"
#include "crf.h"
#include "can_if.h"
#include "signals.h"

void send_bootup(chsm_tst *_self, const cevent_tst *e_pst)
{
    co_node_tst *self = (co_node_tst *)_self;
    can_frame_tst *f_pst = CRF_NEW(SIG_CAN_FRAME);

    if (NULL == f_pst) return;

    f_pst->header_un = CAN_HDR(CO_NMT + self->config_st.node_id_u8, 0, 1);

    CRF_EMIT(f_pst);
}

void send_ng_resp(chsm_tst *_self, const cevent_tst *e_pst)
{
    co_node_tst *self = (co_node_tst *)_self;
    can_frame_tst *f_pst = CRF_NEW(SIG_CAN_FRAME);

    if (NULL == f_pst) return;

    f_pst->header_un = CAN_HDR(CO_NMT + self->config_st.node_id_u8, 0, 1);
    f_pst->mdl_un.bit_st.d0_u8 = 0x7f | (self->ng_toggle_state_b ? 0x80 : 0);

    self->ng_toggle_state_b = !self->ng_toggle_state_b;

    CRF_EMIT(f_pst);
}


bool is_ng_rtr(chsm_tst *_self, const cevent_tst *e_pst)
{
    co_node_tst *self = (co_node_tst *)_self;
    CRF_SIG_VAR(SIG_CAN_FRAME, f_pst, e_pst);

    return f_pst->header_un.all_u32 == CAN_HDR(CO_NMT + self->config_st.node_id_u8, 1, 0).all_u32;
}