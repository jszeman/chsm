#include "canopen.h"
#include "crf.h"
#include "can_if.h"
#include "signals.h"

void send_bootup(chsm_tst *_self, const cevent_tst *e_pst)
{
    co_node_tst *self = (co_node_tst *)_self;
    can_frame_tst *f_pst = CRF_NEW(SIG_CAN_FRAME);

    if (NULL == f_pst) return;

    f_pst->super.sig = SIG_CAN_FRAME;
    f_pst->header_un = CAN_HDR(CO_NMT + self->config_st.node_id_u8, 0, 1);

    CRF_EMIT(f_pst);
}