#ifndef SPI_DRIVER_IF_H
#define SPI_DRIVER_IF_H

#include <stdint.h>
#include <stdbool.h>
#include "crf.h"
#include SIGNAL_CLASSES_H

typedef enum spi_driver_signals_ten
{
    SIG_SPI_DRIVER_TRANSFER_SUCCESS  = SIGNAL_FROM_CLASS(SIG_CLASS_SPI_DRIVER),
    SIG_SPI_DRIVER_TRANSFER_FAIL,
} spi_driver_signals_ten;

#define SIG_SPI_DRIVER_TRANSFER_SUCCESS_TYPE  cevent_tst
#define SIG_SPI_DRIVER_TRANSFER_FAIL_TYPE     cevent_tst

typedef struct spi_driver_if_tst spi_driver_if_tst;

struct spi_driver_if_tst
{
    void                    (*start)(spi_driver_if_tst *self, uint8_t slave_idx_u8, uint8_t *tx_data_pu8, uint8_t *rx_data_pu8, uint16_t len_u16);

    cqueue_tst*             q_pst;
};

/*
Motivation
==========

There is a good deal of diversity in the SPI HW implementation that makes it
difficult to define a very low level interface to SPI peripherals commonly
found in embedded MCU-s. Some HW has FIFO-s and DMA access, some has none of
it.

By defining only a simple start operations there is more burden on the driver
to keep consistent internal state but also more possibiliy for performace
optimizations.

Adding a pointer to an event queue enables async implementation where FIFO
or data register handling may be implemented in an ISR or fast polling
and a callback function.

Interface description
=====================

The driver implementation shall assume the following:
    * The upper layer will not call a function from the interface until
      the driver signals the end of the previous operation.
    * q_pst will be set properly before calling any function from the interface.

start
--------
params:
    slave_idx_u8:   Defines the target device to communicate with. The driver
                    may use this argument to decide which CS line to activate.
    tx_data_pu8:    Pointer to the buffer that holds the data to be sent to
                    the slave device. The caller shall make sure that it is
                    safe to dereference the pointer until the next function
                    call on the interface.
                    If NULL, the driver shall anything.
    rx_data_pu8:    Pointer to the buffer that holds the data to be received
                    from the slave device. The caller shall make sure that it
                    is safe to dereference the pointer until the next function
                    call on the interface.
                    If NULL, the driver shall discard all received data.
    len_u16:        Number of bytes to transfer.

Operation:
    * Pull down the CS of the slave device
    * Send len_u16 bytes from the tx_data_pu8 buffer and read the same number
      of bytes into the rx_data_pu8 buffer.
    * Send an SIG_SPI_DRIVER_TRANSFER_SUCCESS event to the q_pst queue, if the
      transfer was successful. Send a SIG_SPI_DRIVER_TRANSFER_FAIL event if any
      error happened during the transaction.

q_pst
-----
The upper layer shall set this pointer to a cqueue. The driver will send the messages here.
*/


#endif
