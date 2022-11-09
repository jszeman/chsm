/*Generated with CHSM v0.0.0 at 2022.11.08 20.45.19*/
#include "cevent.h"
#include "chsm.h"
#include "bus_driver.h"
#include "bus_driver_functions.h"


static chsm_result_ten waiting(chsm_tst *self, const cevent_tst *e_pst);
static chsm_result_ten idle(chsm_tst *self, const cevent_tst *e_pst);

static chsm_result_ten idle(chsm_tst *self, const cevent_tst *e_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_SEND_DATA:
            send_data(self, e_pst);
            break;

        case TEST_SIG_TICK_1MS:
            emit_event(self, e_pst);
            break;

        case TEST_SIG_START_WAIT:
            return chsm_transition(self, waiting);
    }

    return chsm_ignored(self);
}

static chsm_result_ten waiting(chsm_tst *self, const cevent_tst *e_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_STOP_WAIT:
            return chsm_transition(self, idle);
    }

    return chsm_ignored(self);
}

chsm_result_ten bus_driver_top(chsm_tst *self, const cevent_tst *e_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            return chsm_transition(self, idle);
    }

    return chsm_ignored(self);
}
