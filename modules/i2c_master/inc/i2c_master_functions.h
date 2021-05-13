#ifndef I2C_MASTER_FUNCTIONS_H
#define I2C_MASTER_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2020.12.29 08.44.03*/

#include "i2c_master.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>


void clear_transaction_info(chsm_tst *self, const cevent_tst *e_pst);

void i2c_master_init(chsm_tst *self, const cevent_tst *e_pst);

void i2c_master_send_fail_response(chsm_tst *self, const cevent_tst *e_pst);

void i2c_master_send_success_response(chsm_tst *self, const cevent_tst *e_pst);

void i2c_master_start_rx(chsm_tst *self, const cevent_tst *e_pst);

void i2c_master_start_tx(chsm_tst *self, const cevent_tst *e_pst);

void i2c_master_stop(chsm_tst *self, const cevent_tst *e_pst);

void store_transaction_info(chsm_tst *self, const cevent_tst *e_pst);


/*
Signals:
    SIG_I2C_READ_FAIL      
    SIG_I2C_READ_SUCCESS   
    SIG_I2C_R_TRANSACTION  
    SIG_I2C_WRITE_FAIL     
    SIG_I2C_WRITE_SUCCESS  
    SIG_I2C_WR_TRANSACTION 
    SIG_I2C_W_TRANSACTION  
*/

/*
Other function notes:
*/
#endif
