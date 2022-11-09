/*Generated with CHSM v0.0.0 at 2022.11.08 20.45.11*/
#include "cevent.h"
#include "chsm.h"
#include "dev_driver.h"
#include "dev_driver_functions.h"


static chsm_result_ten wait_response(chsm_tst *self, const cevent_tst *e_pst);
static chsm_result_ten idle(chsm_tst *self, const cevent_tst *e_pst);

static chsm_result_ten idle(chsm_tst *self, const cevent_tst *e_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_READ:
            send_read_request(self, e_pst);
            start_timer(self, e_pst);
            return chsm_transition(self, wait_response);

        case TEST_SIG_WRITE:
            send_write_request(self, e_pst);
            start_timer(self, e_pst);
            return chsm_transition(self, wait_response);
    }

    return chsm_ignored(self);
}

static chsm_result_ten wait_response(chsm_tst *self, const cevent_tst *e_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_RECEIVE:
            insert_data(self, e_pst);
            break;

        case TEST_SIG_TICK_1MS:
            inc_timer(self, e_pst);
            break;

        case TEST_SIG_CANCEL:
            cancel_request(self, e_pst);
            dev_drv_init(self, e_pst);
            return chsm_transition(self, idle);
    }

    if(timeout(self, e_pst))
    {
        dev_drv_init(self, e_pst);
        return chsm_transition(self, idle);
    }

    if(response_complete(self, e_pst))
    {
        dev_drv_init(self, e_pst);
        return chsm_transition(self, idle);
    }

    return chsm_ignored(self);
}

chsm_result_ten dev_driver_top(chsm_tst *self, const cevent_tst *e_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            dev_drv_init(self, e_pst);
            return chsm_transition(self, idle);
    }

    return chsm_ignored(self);
}
