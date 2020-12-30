#ifndef LM73_H
#define LM73_H

#include "i2c_master.h"
#include "i2c_driver_if.h"
#include "crf.h"
#include "signals.h"

/*
 * EVENT DEFINITIONS
 */

typedef struct lm73_temp_tst
{
    cevent_tst          super;              // Signal and GC stuff
    int16_t             temp_C_i16;         // Temperature
    uint16_t            id_u16;             // Sensor ID
} lm73_temp_tst;

#define SIG_LM73_TEMP_TYPE    lm73_temp_tst

/*
 * ACTIVE OBJECT
 */

typedef struct lm73_cfg_tst
{
    uint16_t            id_u16;             // This ID will be inserted into temperature events
    uint16_t            period_ms_u16;      // Temperature read period in ms
    uint8_t             address_u8;         // I2C slave address of the sensor
} lm73_cfg_tst;

typedef struct lm73_tst lm73_tst;

struct lm73_tst
{
    chsm_tst            super;
    lm73_cfg_tst        config_st;

    uint16_t            counter_u16;
};

chsm_result_ten lm73_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);


#endif