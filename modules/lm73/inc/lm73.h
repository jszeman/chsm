#ifndef LM73_H
#define LM73_H

#include "i2c_master.h"
#include "i2c_driver_if.h"
#include "crf.h"
#include "signals.h"
#include "sys_if.h"

/*
LM73 driver
==========

This module implements a high level driver for the LM73 temperature sensor.

Requirements for the LM73 module:
    * Read out the ID register to test the communication
    * Retry the ID register reading after a timeout if the previous read failed
    * Send an online event, when the link is established
    * Send an offline event if the temperature read operation fails for a number of times
    * Send periodic temperature events
    * Allow triggered temperature reads
    * Add a user defined ID into the events (so the application can differentiate between
      the different LM73 events)
*/

/*
 * LM73 SIGNALS
 */

typedef enum lm73_signals_ten
{
    SIG_LM73_TEMP = SIGNAL_FROM_CLASS(SIG_CLASS_LM73),
    SIG_LM73_ONLINE,
    SIG_LM73_OFFLINE,
    SIG_LM73_READ
} lm73_signals_ten;

#define SIG_LM73_TEMP_TYPE      lm73_temp_tst
#define SIG_LM73_ONLINE_TYPE    lm73_status_tst
#define SIG_LM73_OFFLINE_TYPE   lm73_status_tst
#define SIG_LM73_READ_TYPE      cevent_tst


/*
 * EVENT DEFINITIONS
 */

typedef struct lm73_temp_tst
{
    cevent_tst          super;              // Signal and GC stuff
    int32_t             temp_C_i32;         // Temperature
    uint16_t            id_u16;             // Sensor ID
} lm73_temp_tst;

typedef struct lm73_status_tst
{
    cevent_tst          super;              // Signal and GC stuff
    uint16_t            id_u16;             // Sensor ID
} lm73_status_tst;

/*
 * ACTIVE OBJECT
 */

typedef struct lm73_cfg_tst
{
    uint16_t            id_u16;             // This ID will be inserted into temperature events
    uint16_t            period_ms_u16;      // Temperature read period in ms
    uint16_t            max_error_cnt_u16;  // Number of times the module is allowed to fail a read operation before going offline
    uint8_t             address_u8;         // I2C slave address of the sensor
} lm73_cfg_tst;

typedef struct lm73_tst lm73_tst;

struct lm73_tst
{
    /* PUBLIC */
    chsm_tst            super;
    lm73_cfg_tst        config_st;
    int16_t             temp_C_i32;
    bool                valid_b;
    uint16_t            resolution_u16;

    /* PRIVATE */
    uint32_t            counter_u32;
    uint16_t            error_counter_u32;

    i2c_transaction_tst t_st;
    uint8_t             tx_buff_au8[4];
    uint8_t             rx_buff_au8[4];
};

chsm_result_ten lm73_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

bool lm73_timeout(chsm_tst *self, const cevent_tst *e_pst, uint32_t timeout_u32);
bool lm73_error_count(chsm_tst *self, const cevent_tst *e_pst, uint16_t error_cnt_threshold_u16);


#define LM73_READ_PERIOD_VALUE      100
#define LM73_MAX_ERROR_COUNT_VALUE  5
#define LM73_RETRY_TIMEOUT          500
#define LM73_UNPLUGGED_TIMEOUT      5000
#define LM73_ID_REG_VALUE           0x190

#define LM73_READ_PERIOD        (((lm73_tst *)self)->config_st.period_ms_u16)
#define LM73_MAX_ERROR_COUNT    (((lm73_tst *)self)->config_st.max_error_cnt_u16)

#endif
