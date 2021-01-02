#ifndef FRAM_FUNCTIONS_H
#define FRAM_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2021.01.02 21.06.03*/

#include "fram.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>


/*Initialize private fields.*/
void fram_init(chsm_tst *self, const cevent_tst *e_pst);

/*Prepare and send a transaction to the I2C master to read a chunk of data from the FRAM.*/
void read_first_chunk(chsm_tst *self, const cevent_tst *e_pst);

/*If there is more data to be read then prepare and send a transaction to the I2C master to read a chunk of data from the FRAM.*/
void read_next_chunk(chsm_tst *self, const cevent_tst *e_pst);

/*Send a read fail event to the queue designated by the read event.*/
void send_read_fail_response(chsm_tst *self, const cevent_tst *e_pst);

/*Send a read success event to the queue designated by the read event.*/
void send_read_success_response(chsm_tst *self, const cevent_tst *e_pst);

/*Send a write fail event to the queue designated by the read event.*/
void send_write_fail_response(chsm_tst *self, const cevent_tst *e_pst);

/*Send a write success event to the queue designated by the read event.*/
void send_write_success_response(chsm_tst *self, const cevent_tst *e_pst);

/*Make a local copy of the operation event so fields can be used later. */
void store_op_info(chsm_tst *self, const cevent_tst *e_pst);

/*Prepare and send a transaction to the I2C master to write a chunk of data to the FRAM.*/
void write_first_chunk(chsm_tst *self, const cevent_tst *e_pst);

/*If there is more data to be sent then prepare and send a transaction to the I2C master to write a chunk of data to the FRAM.*/
void write_next_chunk(chsm_tst *self, const cevent_tst *e_pst);


/*Returns true, if we don't need to read more data from the FRAM.*/
bool all_data_read(chsm_tst *self, const cevent_tst *e_pst);

/*Returns true, if we don't need to write more data to the FRAM.*/
bool all_data_written(chsm_tst *self, const cevent_tst *e_pst);

/*
Signals:
    SIG_I2C_RESULT_ADDR_NACK 
    SIG_I2C_RESULT_DATA_NACK 
    SIG_I2C_RESULT_SUCCESS   
    SIG_MEM_READ             
    SIG_MEM_WRITE            
*/

/*
Other function notes:
*/
#endif
