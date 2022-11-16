#ifndef FRAM_H
#define FRAM_H

#include "i2c_master.h"
#include "i2c_driver_if.h"
#include "crf.h"
#include SIGNAL_CLASSES_H
#include "mem_driver_if.h"

/*
FRAM driver
==========

This module implements a high level driver for I2C FRAM memories. Max
density is 1Mbit (128kbyte)

The driver chops up large data requests into a number of smaller chunks to
avoid starving other modules that are trying to use the same I2C bus. It is
also necessary because for the write operations the address and the data must
be presented to the I2C master in the same buffer but the upper layers are
sending them in two different fields so the FRAM module has to prepare it
in an internal buffer.
*/

/*
 * ACTIVE OBJECT
 */

#define FRAM_BUFFER_SIZE    32

typedef struct fram_cfg_tst
{
    uint8_t             address_u8;     // Device slave address.
    uint8_t             chunk_size_u8;  // Maximum number of bytes to transfer in one transaction.
    uint32_t            size_u32;       // Memory density in bytes.
} fram_cfg_tst;

typedef struct fram_tst
{
    /* PUBLIC */
    chsm_tst            super;
    fram_cfg_tst        config_st;

    /* PRIVATE */

    i2c_transaction_tst t_st;
    uint8_t             tx_buff_au8[FRAM_BUFFER_SIZE];
    uint8_t             rx_buff_au8[4];

    mem_op_tst          op_st;

    uint32_t            bytes_remaining_u32;
} fram_tst;

chsm_result_ten fram_top(chsm_tst *self, const cevent_tst  *e_pst);

bool fram_timeout(chsm_tst *self, const cevent_tst *e_pst, uint32_t timeout_u32);
bool fram_error_count(chsm_tst *self, const cevent_tst *e_pst, uint16_t error_cnt_threshold_u16);

#endif
