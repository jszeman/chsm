#ifndef EVENTS_H
#define EVENTS_H

#include "crf.h"

typedef enum sig_class_ten {
    SIG_SYS = CRF_SIGNAL_CLASS_START,
    SIG_I2C,
    SIG_UART,
    SIG_LM73,
    SIG_MEM,
} event_class_ten;

#define SIGNAL_CLASS(class) (class * CRF_SIGNAL_CLASS_SIZE)

/*
 * SYSTEM SIGNALS
 */

typedef enum sys_signals_ten
{
    SIG_SYS_TICK_1ms = SIGNAL_CLASS(SIG_SYS),
    SIG_SYS_TICK_10ms,
    SIG_SYS_TICK_100ms,
    SIG_SYS_TICK_1s,
} sys_signals_ten;

/*
 * I2C_SIGNALS
 */

typedef enum i2c_signals_ten
{
    /* External signals */
    SIG_I2C_W_TRANSACTION = SIGNAL_CLASS(SIG_I2C),
    SIG_I2C_R_TRANSACTION,
    SIG_I2C_WR_TRANSACTION,
    SIG_I2C_BUFFERED_W_TRANSACTION,     // Not implemented yet.
    SIG_I2C_BUFFERED_R_TRANSACTION,     // Not implemented yet.
    SIG_I2C_BUFFERED_WR_TRANSACTION,    // Not implemented yet.
    SIG_I2C_RESULT_SUCCESS,
    SIG_I2C_RESULT_ADDR_NACK,
    SIG_I2C_RESULT_DATA_NACK,

    /* Internal signals, used between the driver and the master */
    SIG_I2C_WRITE_SUCCESS,
    SIG_I2C_WRITE_FAIL,
    SIG_I2C_READ_SUCCESS,
    SIG_I2C_READ_FAIL
} i2c_signals_ten;

/*
 * LM73 SIGNALS
 */

typedef enum lm73_signals_ten
{
    /* External signals */
    SIG_LM73_TEMP = SIGNAL_CLASS(SIG_LM73),
    SIG_LM73_ONLINE,
    SIG_LM73_OFFLINE,
    SIG_LM73_READ
} lm73_signals_ten;

/*
 * MEMORY INTERFACE SIGNALS
 */

typedef enum data_signals_ten
{
    /* External signals */
    SIG_MEM_READ = SIGNAL_CLASS(SIG_MEM),
    SIG_MEM_WRITE,
    SIG_MEM_WRITE_SUCCESS,
    SIG_MEM_WRITE_FAIL,
    SIG_MEM_READ_SUCCESS,
    SIG_MEM_READ_FAIL
} data_signals_ten;
#endif