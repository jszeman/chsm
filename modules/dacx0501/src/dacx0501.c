/*Generated with CHSM v0.0.0 at 2021.09.09 07.12.55*/
#include "cevent.h"
#include "chsm.h"
#include "dacx0501.h"
#include "dacx0501_functions.h"


static chsm_result_ten s_busy(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_dacx0501(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten s_dacx0501(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_SYS_TICK_1ms:
            dacx0501_1ms_callback(self, e_pst);
            break;

        default:
            guards_only_b = false;
    }

    return chsm_handle_in_parent(self, ctx_pst, dacx0501_top, NULL, guards_only_b);
}

static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_DACX0501_SAMPLE:
            chsm_exit_children(self, e_pst, ctx_pst);
            dacx0501_write_sample(self, e_pst);
            dacx0501_reset_timer(self, e_pst);
            return chsm_transition(self, s_busy);

        default:
            guards_only_b = false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_dacx0501, NULL, guards_only_b);
}

static chsm_result_ten s_busy(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_DACX0501_SAMPLE:
            chsm_defer(self, e_pst);
            break;

        case SIG_I2C_RESULT_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            dacx0501_set_valid_sample(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_I2C_RESULT_ADDR_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            dacx0501_send_error(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_I2C_RESULT_DATA_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            dacx0501_send_error(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
            guards_only_b = false;
    }

    if(dacx0501_timeout_ms(self, e_pst, DACX0501_TIMEOUT))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        dacx0501_send_error(self, e_pst);
        chsm_recall(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_dacx0501, NULL, guards_only_b);
}

chsm_result_ten dacx0501_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            dacx0501_init(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
            guards_only_b = false;
    }

    return chsm_ignored(self);
}
