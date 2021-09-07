#include "dacx0501_functions.h"
#include "dacx0501.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

const cevent_tst dacx0501_error_event_st = {.sig = SIG_DACX0501_ERROR};

#define DACX0501_CMD_NOOP       0x00
#define DACX0501_CMD_DEVID      0x01
#define DACX0501_CMD_SYNC       0x02
#define DACX0501_CMD_CONFIG     0x03
#define DACX0501_CMD_GAIN       0x04
#define DACX0501_CMD_TRIGGER    0x05
#define DACX0501_CMD_STATUS     0x07
#define DACX0501_CMD_DATA       0x08

void dacx0501_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    CRF_UNUSED(e_pst);
    dacx0501_tst* self = (dacx0501_tst *)_self;

    self->timer_cnt_u32 = 0;
    self->t_st = (i2c_transaction_tst){
        .super.sig = SIG_I2C_W_TRANSACTION,
        .read_cnt_u16 = 0,
        .read_data_pu8 = NULL,
        .slave_addr_u16 = self->config_st.address_u8,
        .target_q_pst = (cqueue_tst *)self,
        .write_cnt_u16 = 3,
        .write_data_pu8 = self->tx_buff_au8
    };
}

void dacx0501_1ms_callback(chsm_tst *_self, const cevent_tst *e_pst)
{
    CRF_UNUSED(e_pst);
    dacx0501_tst* self = (dacx0501_tst *)_self;

    self->timer_cnt_u32++;
}

void dacx0501_reset_timer(chsm_tst *_self, const cevent_tst *e_pst)
{
    CRF_UNUSED(e_pst);
    dacx0501_tst* self = (dacx0501_tst *)_self;

    self->timer_cnt_u32 = 0;
}

void dacx0501_send_error(chsm_tst *_self, const cevent_tst *e_pst)
{
    CRF_UNUSED(e_pst);
    dacx0501_tst* self = (dacx0501_tst *)_self;

    CRF_EMIT(&dacx0501_error_event_st);
}

void dacx0501_set_valid_sample(chsm_tst *_self, const cevent_tst *e_pst)
{
    CRF_UNUSED(e_pst);
    dacx0501_tst* self = (dacx0501_tst *)_self;

    self->sample_valid_b = true;
}

void dacx0501_write_sample(chsm_tst *_self, const cevent_tst *e_pst)
{
    dacx0501_tst* self = (dacx0501_tst *)_self;
    CRF_SIG_VAR(SIG_DACX0501_SAMPLE, s_pst, e_pst);

    self->sample_valid_b = false;

    self->tx_buff_au8[0] = DACX0501_CMD_DATA;
    self->tx_buff_au8[1] = (s_pst->sample_u16 >> 8) & 0xff;
    self->tx_buff_au8[2] = s_pst->sample_u16 & 0xff;

    CRF_EMIT(&self->t_st);
}

bool dacx0501_is_different(chsm_tst *_self, const cevent_tst *e_pst)
{
    dacx0501_tst* self = (dacx0501_tst *)_self;
    CRF_SIG_VAR(SIG_DACX0501_SAMPLE, s_pst, e_pst);

    return s_pst->sample_u16 == self->sample_u16 ? false : true;
}

bool dacx0501_timeout_ms(chsm_tst *_self, const cevent_tst *e_pst, uint32_t timeout_u32)
{
    CRF_UNUSED(e_pst);
    dacx0501_tst* self = (dacx0501_tst *)_self;

    return self->timer_cnt_u32 >= timeout_u32;
}


