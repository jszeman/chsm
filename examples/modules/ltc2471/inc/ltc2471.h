#ifndef LTC2471_H
#define LTC2471_H

#include "i2c_master.h"
#include "i2c_driver_if.h"
#include "crf.h"
#include "sys_if.h"
#include SIGNAL_CLASSES_H

typedef enum ltc2471_signals_ten
{
    SIG_LTC2471_SAMPLE = SIGNAL_FROM_CLASS(SIG_CLASS_LTC2471),
    SIG_LTC2471_ERROR
} ltc2471_signals_ten;

#define SIG_LTC2471_SAMPLE_TYPE   ltc2471_sample_tst
#define SIG_LTC2471_ERROR_TYPE    cevent_tst

/*
 * EVENT DEFINITIONS
 */

typedef struct ltc2471_sample_tst
{
    cevent_tst          super;              // Signal and GC stuff
    uint16_t            sample_u16;         // Sample
    uint16_t            id_u16;             // ADC ID
} ltc2471_sample_tst;

/*
 * ACTIVE OBJECT
 */

typedef struct ltc2471_cfg_tst
{
    uint8_t             address_u8;     // Device slave address.
    uint16_t            read_timeout_ms_u16;
} ltc2471_cfg_tst;

typedef struct ltc2471_tst
{
    /* PUBLIC */
    chsm_tst            super;
    ltc2471_cfg_tst     config_st;

    /* PRIVATE */

    i2c_transaction_tst t_st;

    uint8_t             rx_buff_au8[4];

    uint16_t            last_sample_u16;
    bool                sample_valid_b;

    uint32_t            timer_cnt_u32;
} ltc2471_tst;

#define LTC2471_READ_TIMEOUT (((ltc2471_tst *)self)->config_st.read_timeout_ms_u16)

chsm_result_ten ltc2471_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

bool ltc2471_timeout(chsm_tst *self, const cevent_tst *e_pst, uint32_t timeout_u32);

#endif
