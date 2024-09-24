#ifndef FRAM_FUNCTIONS_H
#define FRAM_FUNCTIONS_H

#include "chsm.h"
#include "cevent.h"
#include "fram.h"
#include "stdbool.h"

/* The following functions shall be implemented by the user */

void clear_op_info (chsm_tst *self, const cevent_tst *e_pst);

/*
Initialize private fields.
*/
void fram_init (chsm_tst *self, const cevent_tst *e_pst);

/*
Prepare and send a transaction to the I2C master to read a chunk of data from the FRAM.
*/
void read_a_chunk (chsm_tst *self, const cevent_tst *e_pst);

/*
Send a read fail event to the queue designated by the read event.
*/
void send_read_fail_response (chsm_tst *self, const cevent_tst *e_pst);

/*
Send a read success event to the queue designated by the read event.
*/
void send_read_success_response (chsm_tst *self, const cevent_tst *e_pst);

/*
Send a write fail event to the queue designated by the read event.
*/
void send_write_fail_response (chsm_tst *self, const cevent_tst *e_pst);

/*
Send a write success event to the queue designated by the read event.
*/
void send_write_success_response (chsm_tst *self, const cevent_tst *e_pst);

/*
Make a local copy of the operation event so fields can be used later. 
*/
void store_op_info (chsm_tst *self, const cevent_tst *e_pst);

/*
Prepare and send a transaction to the I2C master to write a chunk of data to the FRAM.
*/
void write_a_chunk (chsm_tst *self, const cevent_tst *e_pst);


bool last_transaction (chsm_tst *self, const cevent_tst *e_pst);


typedef enum fram_state_id_ten
{
    S_IDLE = 1,
    S_READING = 2,
    S_WRITING = 3,
} fram_state_id_ten;

/*
Signals:
    SIG_I2C_RESULT_ADDR_NACK     
    SIG_I2C_RESULT_DATA_NACK     
    SIG_I2C_RESULT_SUCCESS     
    SIG_MEM_READ 
    SIG_MEM_WRITE     

The following functions shall be declared and implemented by the user:

*/

#endif