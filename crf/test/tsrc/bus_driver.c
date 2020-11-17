/*Generated with CHSM v0.0.0 at 2020.07.26 07.12.40*/

#include "cevent.h"
#include "chsm.h"
#include "bus_driver.h"
#include "bus_driver_functions.h"


static chsm_result_ten waiting(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten idle(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten idle(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_SEND_DATA:
            send_data((bus_driver_tst *)self, e_pst);
            return chsm_handled(self);

        case TEST_SIG_TICK_1MS:
            emit_event((bus_driver_tst *)self, e_pst);
            return chsm_handled(self);

        case TEST_SIG_START_WAIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            return chsm_transition(self, waiting);

    }

    return chsm_handle_in_parent(self, ctx_pst, bus_driver_top, NULL, false);
}

static chsm_result_ten waiting(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_STOP_WAIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            return chsm_transition(self, idle);

    }

    return chsm_handle_in_parent(self, ctx_pst, bus_driver_top, NULL, false);
}

chsm_result_ten bus_driver_top(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            return chsm_transition(self, idle);

    }

    return chsm_ignored(self);
}
