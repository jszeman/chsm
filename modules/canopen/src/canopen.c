/*Generated with CHSM v0.0.0 at 2022.11.17 20.21.44*/
#include "cevent.h"
#include "chsm.h"
#include "canopen.h"
#include "canopen_functions.h"


static chsm_result_ten s_ng_active(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_ng_inactive(chsm_tst *self, const cevent_tst  *e_pst);

static chsm_result_ten s_ng_inactive(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_I_CANOPEN_NG:
            co_send_ng_active(self, e_pst);
            co_reset_timer(self, e_pst);
            return chsm_transition(self, s_ng_active);

        case SIG_CAN_FRAME:
            co_process_frame(self, e_pst);
            break;

        case SIG_SYS_TICK_1ms:
            co_callback(self, e_pst);
            break;
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_ng_active(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_I_CANOPEN_NG:
            co_reset_timer(self, e_pst);
            break;

        case SIG_CAN_FRAME:
            co_process_frame(self, e_pst);
            break;

        case SIG_SYS_TICK_1ms:
            co_callback(self, e_pst);
            break;
    }

    if(co_timeout(self, e_pst, CO_LIFETIME))
    {
        co_send_ng_inactive(self, e_pst);
        return chsm_transition(self, s_ng_inactive);
    }

    return chsm_ignored(self);
}

chsm_result_ten co_node_top(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            co_node_init(self, e_pst);
            co_send_bootup(self, e_pst);
            return chsm_transition(self, s_ng_inactive);
    }

    return chsm_ignored(self);
}
