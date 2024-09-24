#ifndef LTC2471_FUNCTIONS_H
#define LTC2471_FUNCTIONS_H

#include "chsm.h"
#include "cevent.h"
#include "ltc2471.h"
#include "stdbool.h"

/* The following functions shall be implemented by the user */

void ltc2471_1ms_callack (chsm_tst *self, const cevent_tst *e_pst);

void ltc2471_invalidate_sample (chsm_tst *self, const cevent_tst *e_pst);

void ltc2471_restart_timer (chsm_tst *self, const cevent_tst *e_pst);

void ltc2471_start_read (chsm_tst *self, const cevent_tst *e_pst);

void ltc2471_update_sample (chsm_tst *self, const cevent_tst *e_pst);



typedef enum ltc2471_state_id_ten
{
    S_IDLE = 1,
    S_READING = 2,
} ltc2471_state_id_ten;

/*
Signals:
    SIG_I2C_RESULT_ADDR_NACK 
    SIG_I2C_RESULT_SUCCESS     
    SIG_SYS_TICK_1ms     

The following functions shall be declared and implemented by the user:

    ltc2471_timeout

*/

#endif