/*Generated with CHSM v0.0.0 at 2020.07.29 06.46.58*/

#include "cevent.h"
#include "chsm.h"
#include "dev_driver.h"
#include "dev_driver_functions.h"


static chsm_result_ten wait_response(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten idle(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten idle(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_READ:
            chsm_exit_children(self, e_pst, ctx_pst);
            send_read_request((dev_driver_tst *)self, e_pst);
            start_timer((dev_driver_tst *)self, e_pst);
            return chsm_transition(self, wait_response);

        case TEST_SIG_WRITE:
            chsm_exit_children(self, e_pst, ctx_pst);
            send_write_request((dev_driver_tst *)self, e_pst);
            start_timer((dev_driver_tst *)self, e_pst);
            return chsm_transition(self, wait_response);

    }

    return chsm_handle_in_parent(self, ctx_pst, dev_driver_top, NULL, false);
}

static chsm_result_ten wait_response(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_RECEIVE:
            insert_data((dev_driver_tst *)self, e_pst);
            return chsm_handled(self);

        case TEST_SIG_TICK_1MS:
            inc_timer((dev_driver_tst *)self, e_pst);
            return chsm_handled(self);

        case TEST_SIG_CANCEL:
            chsm_exit_children(self, e_pst, ctx_pst);
            cancel_request((dev_driver_tst *)self, e_pst);
            dev_drv_init((dev_driver_tst *)self, e_pst);
            return chsm_transition(self, idle);

    }

    if(timeout((dev_driver_tst *)self, e_pst))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        dev_drv_init((dev_driver_tst *)self, e_pst);
        return chsm_transition(self, idle);
    }

    if(response_complete((dev_driver_tst *)self, e_pst))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        dev_drv_init((dev_driver_tst *)self, e_pst);
        return chsm_transition(self, idle);
    }

    return chsm_handle_in_parent(self, ctx_pst, dev_driver_top, NULL, false);
}

chsm_result_ten dev_driver_top(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            dev_drv_init((dev_driver_tst *)self, e_pst);
            return chsm_transition(self, idle);

    }

    return chsm_ignored(self);
}
