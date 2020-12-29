#ifndef EVENTS_H
#define EVENTS_H

#include "crf.h"

typedef enum sig_class_ten {
    SIG_SYS = CRF_SIGNAL_CLASS_START,
    SIG_I2C,
    SIG_UART,
    SIG_LM73
} event_class_ten;

#define SIGNAL_CLASS(class) (class * CRF_SIGNAL_CLASS_SIZE)

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


#endif