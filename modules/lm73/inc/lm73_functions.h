#ifndef LM73_FUNCTIONS_H
#define LM73_FUNCTIONS_H

#include "chsm.h"
#include "cevent.h"
#include "lm73.h"
#include "stdbool.h"

/* The following functions shall be implemented by the user */

/*
Increase the timer counter.
*/
void lm73_1ms_callback (chsm_tst *self, const cevent_tst *e_pst);

/*
Increase the error counter.
*/
void lm73_inc_error_counter (chsm_tst *self, const cevent_tst *e_pst);

void lm73_init (chsm_tst *self, const cevent_tst *e_pst);

/*
Try to read the ID register from the LM73 by sending a write-read transaction to the I2C master.
*/
void lm73_read_id (chsm_tst *self, const cevent_tst *e_pst);

/*
Reset the pointer register in the LM73 to 0, so all subsequent reads will access the temperature register.
*/
void lm73_reset_pointer (chsm_tst *self, const cevent_tst *e_pst);

/*
Reset the timer counter.
*/
void lm73_reset_timer (chsm_tst *self, const cevent_tst *e_pst);

/*
Send a read transaction to the LM73.
*/
void lm73_start_read (chsm_tst *self, const cevent_tst *e_pst);

/*
- Update the temperature display
- Send an event with the new value
- Decrease the error counter
*/
void lm73_update_temp (chsm_tst *self, const cevent_tst *e_pst);

/*
Send a SIG_LM73_OFFLINE event. This can be used to detect communication errors between the module and the I2C slave.
*/
void send_offline_event (chsm_tst *self, const cevent_tst *e_pst);

/*
Send a SIG_LM73_ONLINE event. This can be used to detect successful initialization.
*/
void send_online_event (chsm_tst *self, const cevent_tst *e_pst);


/*
True, if the response data is equal to 0x190. See LM73 datasheet section: 7.5.1.7 Identification Register
*/
bool lm73_id_match (chsm_tst *self, const cevent_tst *e_pst);


typedef enum lm73_state_id_ten
{
    S_IDLE = 1,
    S_READING = 2,
    S_READ_ID_REG = 3,
    S_RESET_PTR_REG = 4,
} lm73_state_id_ten;

/*
Signals:
    SIG_I2C_RESULT_ADDR_NACK     
    SIG_I2C_RESULT_DATA_NACK     
    SIG_I2C_RESULT_SUCCESS     
    SIG_LM73_READ     
    SIG_SYS_TICK_1ms 

The following functions shall be declared and implemented by the user:

    lm73_timeout
        Evaluates to true if the internal timer counter is greater than or equal to the parameter.

    lm73_error_count
        Return true, if the error counter is greater or equal then the parameter.

*/

#endif