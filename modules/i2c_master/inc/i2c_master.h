#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include "i2c_driver_if.h"
#include "crf.h"
#include SIGNAL_CLASSES_H

/*
I2C master
==========

The I2C master module is responsible for managing access to a single I2C
peripheral driver by providing an event driven interface that supports
read, write and write-read access to the I2C bus.

Motivation
----------

It is very difficult to make unit tests for embedded hardware drivers. Not
impossible, but extremly inconvenient compared to a module that only has
software interfaces and therefore can be tested on a PC.
A simple workaround for this problem is to keep the actual HW drivers as simple
as possible and move all complexity into modules that can be tested on the 
host and reused with different driver implementations. Like this module, for
example.

Requirements for the I2C master module:
    * Shall be able to handle multiple parallel transaction requests
    * Shall send a reply to each request after it was finished
    * Shall be able handle one I2C driver via the I2C driver interface
    * Shall support simple write operations
    * Shall support simple read operations
    * Shall support write-than-read operations
*/


/*
 * I2C_SIGNALS
 */

typedef enum i2c_master_signals_ten
{
    /* Caller buffered transactions */
    SIG_I2C_W_TRANSACTION = SIGNAL_FROM_CLASS(SIG_CLASS_I2C_MASTER),
    SIG_I2C_R_TRANSACTION,
    SIG_I2C_WR_TRANSACTION,

    /* !!!NOT IMPLEMENTED!!! Internally buffered transactions */
    SIG_I2C_BUFFERED_W_TRANSACTION,
    SIG_I2C_BUFFERED_R_TRANSACTION,
    SIG_I2C_BUFFERED_WR_TRANSACTION,

    /* Transaction result signals */
    SIG_I2C_RESULT_SUCCESS,
    SIG_I2C_RESULT_ADDR_NACK,
    SIG_I2C_RESULT_DATA_NACK,
    SIG_I2C_RESULT_BUS_BUSY
} i2c_master_signals_ten;

#define SIG_I2C_W_TRANSACTION_TYPE     i2c_transaction_tst
#define SIG_I2C_R_TRANSACTION_TYPE     i2c_transaction_tst
#define SIG_I2C_WR_TRANSACTION_TYPE    i2c_transaction_tst

#define SIG_I2C_RESULT_SUCCESS_TYPE    cevent_tst
#define SIG_I2C_RESULT_ADDR_NACK_TYPE  cevent_tst
#define SIG_I2C_RESULT_DATA_NACK_TYPE  cevent_tst
#define SIG_I2C_RESULT_BUS_BUSY_TYPE   cevent_tst

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
