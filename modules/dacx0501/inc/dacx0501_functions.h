#ifndef DACX0501_FUNCTIONS_H
#define DACX0501_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2021.09.09 07.12.55*/

#include "dacx0501.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>


void dacx0501_1ms_callback(chsm_tst *self, const cevent_tst *e_pst);

void dacx0501_init(chsm_tst *self, const cevent_tst *e_pst);

void dacx0501_reset_timer(chsm_tst *self, const cevent_tst *e_pst);

void dacx0501_send_error(chsm_tst *self, const cevent_tst *e_pst);

void dacx0501_set_valid_sample(chsm_tst *self, const cevent_tst *e_pst);

void dacx0501_write_sample(chsm_tst *self, const cevent_tst *e_pst);


typedef enum dacx0501_state_id_ten
{
    S_DACX0501 = 0,
    S_IDLE = 1,
    S_BUSY = 2,
} dacx0501_state_id_ten;


/*
Signals:
    SIG_DACX0501_SAMPLE      
    SIG_I2C_RESULT_ADDR_NACK 
    SIG_I2C_RESULT_DATA_NACK 
    SIG_I2C_RESULT_SUCCESS   
    SIG_SYS_TICK_1ms         
*/

/*
Other function notes:

dacx0501_timeout_ms:
*/
#endif
