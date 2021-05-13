/*Generated with CHSM v0.0.0 at 2021.02.14 14.35.37*/
#include "cevent.h"
#include "chsm.h"
#include "can_if.h"
#include "canopen.h"
#include "canopen_functions.h"

static chsm_result_ten s_ng_active(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_ng_inactive(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_canopen(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten s_canopen(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_CAN_FRAME:
            co_process_frame(self, e_pst);
            break;

        case SIG_SYS_TICK_1ms:
            co_callback(self, e_pst);
            break;

        default:
            guards_only_b = false;
    }

    return chsm_handle_in_parent(self, ctx_pst, co_node_top, NULL, guards_only_b);
}

static chsm_result_ten s_ng_inactive(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I_CANOPEN_NG:
            chsm_exit_children(self, e_pst, ctx_pst);
            co_send_ng_active(self, e_pst);
            co_reset_timer(self, e_pst);
            return chsm_transition(self, s_ng_active);

        default:
            guards_only_b = false;
    }


    return chsm_handle_in_parent(self, ctx_pst, s_canopen, NULL, guards_only_b);
}

static chsm_result_ten s_ng_active(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I_CANOPEN_NG:
            co_reset_timer(self, e_pst);
            break;

        default:
            guards_only_b = false;
    }

    if(co_timeout(self, e_pst, CO_LIFETIME))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        co_send_ng_inactive(self, e_pst);
        return chsm_transition(self, s_ng_inactive);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_canopen, co_send_ng_inactive, guards_only_b);
}

chsm_result_ten co_node_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            co_node_init(self, e_pst);
            co_send_bootup(self, e_pst);
            return chsm_transition(self, s_ng_inactive);

        default:
            guards_only_b = false;
    }

    return chsm_ignored(self);
}
