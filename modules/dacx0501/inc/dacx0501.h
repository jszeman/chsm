#ifndef DACX0501_H
#define DACX0501_H

#include "i2c_master.h"
#include "i2c_driver_if.h"
#include "crf.h"
#include "sys_if.h"
#include SIGNAL_CLASSES_H

typedef enum dacx0501_signals_ten
{
    SIG_DACX0501_SAMPLE = SIGNAL_FROM_CLASS(SIG_CLASS_DACX0501),
    SIG_DACX0501_ERROR
} dacx0501_signals_ten;

#define SIG_DACX0501_SAMPLE_TYPE   dacx0501_sample_tst
#define SIG_DACX0501_ERROR_TYPE    cevent_tst

/*
 * EVENT DEFINITIONS
 */

typedef struct dacx0501_sample_tst
{
    cevent_tst          super;              // Signal and GC stuff
    uint16_t            sample_u16;         // Sample
    uint16_t            id_u16;             // ADC ID
} dacx0501_sample_tst;

/*
 * ACTIVE OBJECT
 */

typedef struct dacx0501_cfg_tst
{
    uint8_t             address_u8;     // Device slave address.
    uint16_t            read_timeout_ms_u16;
} dacx0501_cfg_tst;

typedef struct dacx0501_tst
{
    /* PUBLIC */
    chsm_tst            super;
    dacx0501_cfg_tst     config_st;

    /* PRIVATE */

    i2c_transaction_tst t_st;

    uint8_t             rx_buff_au8[4];

    uint16_t            last_sample_u16;
    bool                sample_valid_b;

    uint32_t            timer_cnt_u32;
} dacx0501_tst;

#define DACX0501_READ_TIMEOUT (((dacx0501_tst *)self)->config_st.read_timeout_ms_u16)

chsm_result_ten dacx0501_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

bool dacx0501_timeout(chsm_tst *self, const cevent_tst *e_pst, uint32_t timeout_u32);

#endif
