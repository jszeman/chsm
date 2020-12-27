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
    uint8_t             write_data_au8[];   // Data for the write operation
} i2c_transaction_tst;


typedef enum i2c_status_ten
{
    I2C_SUCCESS,
    I2C_ADDR_NACK,
    I2C_DATA_NACK,
} i2c_status_ten;

typedef struct i2c_result_tst
{
    cevent_tst          super;
    i2c_status_ten      status_en;
    uint8_t             len_u8;
    uint8_t             data_au8[];
} i2c_result_tst;

#define SIG_I2C_TRANSACTION_TYPE    i2c_transaction_tst
#define SIG_I2C_RESULT_TYPE         i2c_result_tst

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
};

chsm_result_ten i2c_master_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);


#endif