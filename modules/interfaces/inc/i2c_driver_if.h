#ifndef I2C_DRIVER_IF_H
#define I2C_DRIVER_IF_H

#include <stdint.h>
#include <stdbool.h>
#include "crf.h"
#include SIGNAL_CLASSES_H

typedef enum i2c_driver_signals_ten
{
    SIG_I2C_WRITE_SUCCESS  = SIGNAL_FROM_CLASS(SIG_CLASS_I2C_DRIVER),
    SIG_I2C_WRITE_FAIL,
    SIG_I2C_READ_SUCCESS,
    SIG_I2C_READ_FAIL
} i2c_driver_signals_ten;

#define SIG_I2C_WRITE_SUCCESS_TYPE  cevent_tst
#define SIG_I2C_READ_SUCCESS_TYPE   cevent_tst
#define SIG_I2C_WRITE_FAIL_TYPE     cevent_tst
#define SIG_I2C_READ_FAIL_TYPE      cevent_tst

typedef struct i2c_driver_status_bits_tst
{
    uint16_t    busy_u16: 1; // The peripheral is busy transmitting or receiving data.
    uint16_t    addr_nack_u16: 1;
    uint16_t    data_nack_u16: 1;
    uint16_t    bus_busy_u16: 1;
} i2c_driver_status_bits_tst;

typedef union i2c_driver_status_tun
{
    uint16_t                    all_u16;
    i2c_driver_status_bits_tst  bit_st;
} i2c_driver_status_tun;

typedef struct i2c_driver_if_tst i2c_driver_if_tst;

struct i2c_driver_if_tst
{
    void                    (*start_tx)(i2c_driver_if_tst *self, uint8_t slave_addr_u8, uint8_t *data_pu8, uint16_t len_u16);
    void                    (*start_rx)(i2c_driver_if_tst *self, uint8_t slave_addr_u8, uint8_t *data_pu8, uint16_t len_u16);
    void                    (*stop)(i2c_driver_if_tst* self);

    cqueue_tst*             q_pst;

    i2c_driver_status_tun   status_un;
};

/*
Motivation
==========

There is a good deal of diversity in the I2C HW implementation that makes it
difficult to define a very low level interface to I2C peripherals commonly
found in embedded MCU-s. Some HW stores slave address in a separate register,
some not, other HW has deep FIFOs while most has none. During a transaction
different MCU-s need very different operations to keep the data flow continuous.

By defining only tx, rx and stop operations there is more burden on the driver
to keep consistent internal state but also more freedom to perform a read or
write operation with the maximum efficiency.

Adding a pointer to an event queue enables async implementation where FIFO
or data register handling may be implemented in an ISR or fast polling
and a callback function.

Interface description
=====================

The driver implementation shall assume the following:
    * The upper layer will not call a function from the interface until
      the driver signals the end of the previous operation.
    * q_pst will be set properly before calling any function from the interface.

start_tx
--------
params:
    slave_addr_u8:  7 bit slave device address.
    data_pu8:       Pointer to the buffer that holds the data to be sent to the slave device.
                    The caller shall make sure that it is safe to dereference the pointer until
                    the next function call on the interface.
    len_u16:        Number of bytes to send to the slave device.

Operation:
    * Generate a START condition on the bus
    * Send the address with the R/W bit set to 0
    * If the address was NACK-d, send a SIG_I2C_WRITE_FAIL event to the q_pst queue.
    * Send len_u16 bytes from the data_pu8 buffer
    * If a NACK was received before the last byte send a SIG_I2C_WRITE_FAIL event to the q_pst queue.
    * If all bytes were ACK-d by the slave (except maybe the last) send an SIG_I2C_WRITE_SUCCESS event to the q_pst queue.
    * May be implemented as async function. The caller shall not expect that the full transaction was executed
      during the call.


start_rx
--------
params:
    slave_addr_u8:  7 bit slave device address.
    data_pu8:       Pointer to the buffer that holds the data to be received from the slave device.
                    The caller shall make sure that it is safe to dereference the pointer until
                    the next function call on the interface.
    len_u16:        Number of bytes to send to the slave device.

Operation:
    * Generate a START condition on the bus
    * Send the address with the R/W bit set to 1
    * If the address was NACK-d, send a SIG_I2C_READ_FAIL event to the q_pst queue.
    * Read len_u16 bytes from bus to the the data_pu8 buffer
    * NACK the last byte read from the bus to signal the slave that a STOP or RESTART is coming so it
      must release the bus.
    * May be implemented as async function. The caller shall not expect that the full transaction was executed
      during the call.


stop
----
Operation:
    * Generate a STOP condition.


q_pst
-----
The upper layer shall set this pointer to a cqueue. The driver will send the messages here.


status_un
---------
Current driver status.
*/


#endif
