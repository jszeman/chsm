#ifndef LTC2471_FUNCTIONS_H
#define LTC2471_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2022.11.16 21.08.53*/

#include "ltc2471.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>


void ltc2471_1ms_callack(chsm_tst *self, const cevent_tst *e_pst);

void ltc2471_invalidate_sample(chsm_tst *self, const cevent_tst *e_pst);

void ltc2471_restart_timer(chsm_tst *self, const cevent_tst *e_pst);

void ltc2471_start_read(chsm_tst *self, const cevent_tst *e_pst);

void ltc2471_update_sample(chsm_tst *self, const cevent_tst *e_pst);


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
*/

/*
Other function notes:

ltc2471_timeout:
*/
#endif
