#ifndef SPI_MASTER_H
#define SPI_MASTER_H

#include "spi_driver_if.h"
#include "crf.h"
#include SIGNAL_CLASSES_H

/*
SPI master
==========

The SPI master module is responsible for managing access to a single SPI
peripheral driver by providing an event driven interface that supports
transaction access to the SPI bus.

Motivation
----------

The main goal of this module is to handle concurrent access to a single
SPI driver. Device drivers can send transactions to the master at any time,
even if another transaction is in progress. The master will queue up incoming
transactions until they can be processed.

Requirements for the SPI master module:
    * Shall be able to handle multiple parallel transaction requests
    * Shall send a reply to each request after it was finished
    * Shall be able handle one SPI driver via the SPI driver interface
    * Shall support simple write operations
    * Shall support simple read operations
    * Shall support write-and-read operations
*/


/*
 * SPI_SIGNALS
 */

typedef enum spi_master_signals_ten
{
    /* Caller buffered transactions */
    SIG_SPI_MASTER_TRANSACTION = SIGNAL_FROM_CLASS(SIG_CLASS_SPI_MASTER),

    /* Transaction result signals */
    SIG_SPI_MASTER_RESULT_SUCCESS,
    SIG_SPI_MASTER_RESULT_FAIL,
} spi_master_signals_ten;

#define SIG_SPI_MASTER_TRANSACTION_TYPE     spi_transaction_tst

#define SIG_SPI_MASTER_RESULT_SUCCESS_TYPE    cevent_tst
#define SIG_SPI_MASTER_RESULT_FAIL_TYPE    cevent_tst

/*
 * EVENT DEFINITIONS
 */

typedef struct spi_transaction_tst
{
    cevent_tst          super;          // Signal and GC stuff
    uint8_t             slave_idx_u8;   // Slave index
    uint16_t            len_u16;        // Number of bytes transfer
    cqueue_tst*         target_q_pst;   // Target queue. After the master
                                        //   has finished the transaction it
                                        //   will put the results here
    uint8_t*            tx_data_pu8;    // Pointer to the transmit data buffer
    uint8_t*            rx_data_pu8;    // Pointer to the receive data buffer
} spi_transaction_tst;

/*
 * ACTIVE OBJECT
 */

typedef struct spi_master_cfg_tst
{
    spi_driver_if_tst*  driver_pst;
} spi_master_cfg_tst;

typedef struct spi_master_tst spi_master_tst;

struct spi_master_tst
{
    chsm_tst            super;
    spi_master_cfg_tst  config_st;

    // This is used to store transaction data
    // while waiting for an operation to finish.
    spi_transaction_tst cached_tr_st;
};

chsm_result_ten spi_master_top(chsm_tst *self, const cevent_tst  *e_pst);


#endif
