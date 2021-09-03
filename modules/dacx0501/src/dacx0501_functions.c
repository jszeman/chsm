#include "ltc2471_functions.h"
#include "ltc2471.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

const cevent_tst ltc2471_error_event_st = {.sig = SIG_LTC2471_ERROR};

void ltc2471_1ms_callack(chsm_tst *_self, const cevent_tst *e_pst)
{
    ltc2471_tst* self = (ltc2471_tst *)_self;

    self->timer_cnt_u32++;
}

void ltc2471_invalidate_sample(chsm_tst *_self, const cevent_tst *e_pst)
{
    ltc2471_tst* self = (ltc2471_tst *)_self;

    self->sample_valid_b = false;

    CRF_EMIT(&ltc2471_error_event_st);
}

void ltc2471_restart_timer(chsm_tst *_self, const cevent_tst *e_pst)
{
    ltc2471_tst* self = (ltc2471_tst *)_self;

    self->timer_cnt_u32 = 0;
}

void ltc2471_start_read(chsm_tst *_self, const cevent_tst *e_pst)
{
    ltc2471_tst* self = (ltc2471_tst *)_self;

    self->t_st.super.sig =      SIG_I2C_R_TRANSACTION;
    self->t_st.write_cnt_u16 =  0;
    self->t_st.read_cnt_u16 =   2;
    self->rx_buff_au8[0] =      0;
    self->rx_buff_au8[1] =      0;
    self->t_st.slave_addr_u16 = self->config_st.address_u8;
    self->t_st.target_q_pst =   (cqueue_tst *)self;
    self->t_st.read_data_pu8 =  self->rx_buff_au8;

    CRF_EMIT(&self->t_st);
}

void ltc2471_update_sample(chsm_tst *_self, const cevent_tst *e_pst)
{
    ltc2471_tst* self = (ltc2471_tst *)_self;
    
    ltc2471_sample_tst* sample_pst = CRF_NEW(SIG_LTC2471_SAMPLE);

    if (NULL == sample_pst) return;

    // Compile the value of the Temperature Data Register into a variable
    uint16_t t_u16 =            self->rx_buff_au8[0];
    t_u16 <<= 8;
    t_u16 |=                    self->rx_buff_au8[1];

    sample_pst->sample_u16 =    t_u16;
    sample_pst->id_u16 =        self->config_st.address_u8;

    self->last_sample_u16 =     t_u16;
    self->sample_valid_b =      true;
    
    CRF_EMIT(sample_pst);
}

bool ltc2471_timeout(chsm_tst *_self, const cevent_tst *e_pst, uint32_t timeout_u32)
{
    ltc2471_tst* self = (ltc2471_tst *)_self;

    return self->timer_cnt_u32 > timeout_u32;
}


