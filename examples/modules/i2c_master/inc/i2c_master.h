#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include "i2c_driver_if.h"
#include "crf.h"
#include "signals.h"

/*
 * EVENT DEFINITIONS
 */

typedef struct i2c_transaction_tst
{
    cevent_tst          super;              // Signal and GC stuff
    uint8_t             slave_addr_u16;     // Slave address
    uint16_t            write_cnt_u16;      // Number of bytes to write
    uint16_t            read_cnt_u16;       // Number of bytes to read
    cqueue_tst*         target_q_pst;       // Target queue. After the master
                                            // has finished the transaction it
                                            // will put the results here
    uint8_t*            write_data_pu8;     // Pointer to the data buffer for the write operation
    uint8_t*            read_data_pu8;      // Pointer to the data buffer for the read operation
} i2c_transaction_tst;

#define SIG_I2C_TRANSACTION_TYPE    i2c_transaction_tst

/*
 * ACTIVE OBJECT
 */

typedef struct i2c_master_cfg_tst
{
    i2c_driver_if_tst*  driver_pst;
} i2c_master_cfg_tst;

typedef struct i2c_master_tst i2c_master_tst;

struct i2c_master_tst
{
    chsm_tst            super;
    i2c_master_cfg_tst  config_st;

    // This is used to store transaction data
    // while waiting for an operation to finish.
    i2c_transaction_tst cached_tr_st;
};

chsm_result_ten i2c_master_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);


#endif