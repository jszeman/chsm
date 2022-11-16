/*Generated with CHSM v0.0.0 at 2022.11.16 21.08.53*/
#include "cevent.h"
#include "chsm.h"
#include "ltc2471.h"
#include "ltc2471_functions.h"


static chsm_result_ten s_reading(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst);

static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_SYS_TICK_1ms:
            ltc2471_1ms_callack(self, e_pst);
            break;
    }

    if(ltc2471_timeout(self, e_pst, LTC2471_READ_TIMEOUT))
    {
        ltc2471_start_read(self, e_pst);
        ltc2471_restart_timer(self, e_pst);
        return chsm_transition(self, s_reading);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_reading(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_SUCCESS:
            ltc2471_update_sample(self, e_pst);
            ltc2471_restart_timer(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_I2C_RESULT_ADDR_NACK:
            ltc2471_invalidate_sample(self, e_pst);
            ltc2471_restart_timer(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_SYS_TICK_1ms:
            ltc2471_1ms_callack(self, e_pst);
            break;
    }

    if(ltc2471_timeout(self, e_pst, LTC2471_READ_TIMEOUT))
    {
        ltc2471_invalidate_sample(self, e_pst);
        ltc2471_restart_timer(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_ignored(self);
}

chsm_result_ten ltc2471_top(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            ltc2471_restart_timer(self, e_pst);
            return chsm_transition(self, s_idle);
    }

    return chsm_ignored(self);
}
