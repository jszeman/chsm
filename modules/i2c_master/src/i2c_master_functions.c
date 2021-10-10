#include "crf.h"
#include "cevent.h"
#include "i2c_master.h"
#include <stdio.h>


const cevent_tst success_response_st = {.sig = SIG_I2C_RESULT_SUCCESS};
const cevent_tst addr_nack_response_st = {.sig = SIG_I2C_RESULT_ADDR_NACK};
const cevent_tst data_nack_response_st = {.sig = SIG_I2C_RESULT_DATA_NACK};
const cevent_tst bus_busy_response_st = {.sig = SIG_I2C_RESULT_BUS_BUSY};

void i2c_master_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    i2c_master_tst *self = (i2c_master_tst *)_self;

    self->config_st.driver_pst->q_pst = (cqueue_tst *)self;
}


void store_transaction_info(chsm_tst *_self, const cevent_tst *e_pst)
{
    i2c_master_tst *self = (i2c_master_tst *)_self;

    self->cached_tr_st = *((i2c_transaction_tst *)e_pst);
}

void clear_transaction_info(chsm_tst *self, const cevent_tst *e_pst)
{

}

void i2c_master_send_success_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    i2c_master_tst *self = (i2c_master_tst *)_self;

    self->cached_tr_st.target_q_pst->put(self->cached_tr_st.target_q_pst, &success_response_st);
}

void i2c_master_send_fail_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    i2c_master_tst *self = (i2c_master_tst *)_self;

    if (self->config_st.driver_pst->status_un.bit_st.bus_busy_u16)
    {
        CRF_EMIT(&bus_busy_response_st);
        self->cached_tr_st.target_q_pst->put(self->cached_tr_st.target_q_pst, &addr_nack_response_st);
    }
    else if (self->config_st.driver_pst->status_un.bit_st.addr_nack_u16)
    {
        self->cached_tr_st.target_q_pst->put(self->cached_tr_st.target_q_pst, &addr_nack_response_st);
    }
    else if (self->config_st.driver_pst->status_un.bit_st.data_nack_u16)
    {
        self->cached_tr_st.target_q_pst->put(self->cached_tr_st.target_q_pst, &data_nack_response_st);
    }

}

void i2c_master_start_rx(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s\n", __FUNCTION__);

    i2c_master_tst *self = (i2c_master_tst *)_self;

    i2c_transaction_tst *t_pst = &self->cached_tr_st;

    self->config_st.driver_pst->start_rx(self->config_st.driver_pst, t_pst->slave_addr_u16, t_pst->read_data_pu8, t_pst->read_cnt_u16);
}

void i2c_master_start_tx(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s\n", __FUNCTION__);

    i2c_master_tst *self = (i2c_master_tst *)_self;

    i2c_transaction_tst *t_pst = &self->cached_tr_st;

    self->config_st.driver_pst->start_tx(self->config_st.driver_pst, t_pst->slave_addr_u16, t_pst->write_data_pu8, t_pst->write_cnt_u16);
}

void i2c_master_stop(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s\n", __FUNCTION__);
    
    i2c_master_tst *self = (i2c_master_tst *)_self;

    self->config_st.driver_pst->stop(self->config_st.driver_pst);
}
