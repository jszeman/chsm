/*Generated with CHSM v0.0.0 at 2021.04.30 16.05.52*/
#include "cevent.h"
#include "chsm.h"
#include "ltc2471.h"
#include "ltc2471_functions.h"


static chsm_result_ten s_reading(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_ltc2471(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten s_ltc2471(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_SYS_TICK_1ms:
            ltc2471_1ms_callack(self, e_pst);
            break;

        default:
            guards_only_b = false;
    }

    return chsm_handle_in_parent(self, ctx_pst, ltc2471_top, NULL, guards_only_b);
}

static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
            guards_only_b = false;
    }

    if(ltc2471_timeout(self, e_pst, LTC2471_READ_TIMEOUT))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        ltc2471_start_read(self, e_pst);
        ltc2471_restart_timer(self, e_pst);
        return chsm_transition(self, s_reading);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_ltc2471, NULL, guards_only_b);
}

static chsm_result_ten s_reading(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            ltc2471_update_sample(self, e_pst);
            ltc2471_restart_timer(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_I2C_RESULT_ADDR_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            ltc2471_invalidate_sample(self, e_pst);
            ltc2471_restart_timer(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
            guards_only_b = false;
    }

    if(ltc2471_timeout(self, e_pst, LTC2471_READ_TIMEOUT))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        ltc2471_invalidate_sample(self, e_pst);
        ltc2471_restart_timer(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_ltc2471, NULL, guards_only_b);
}

chsm_result_ten ltc2471_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            ltc2471_restart_timer(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
            guards_only_b = false;
    }

    return chsm_ignored(self);
}
