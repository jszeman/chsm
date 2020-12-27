#include "crf.h"
#include "i2c_master.h"
#include <stdio.h>


void i2c_master_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    i2c_master_tst *self = (i2c_master_tst *)_self;

    self->config_st.driver_pst->q_pst = (cqueue_tst *)self;
}

void i2c_master_send_r_response(chsm_tst *self, const cevent_tst *e_pst)
{

}

void i2c_master_send_rw_response(chsm_tst *self, const cevent_tst *e_pst)
{

}

void i2c_master_send_w_response(chsm_tst *self, const cevent_tst *e_pst)
{
    
}

void i2c_master_start_rx(chsm_tst *self, const cevent_tst *e_pst)
{
    
}

void i2c_master_start_tx(chsm_tst *_self, const cevent_tst *e_pst)
{
    i2c_master_tst *self = (i2c_master_tst *)_self;

    i2c_transaction_tst *t_pst = (i2c_transaction_tst *)e_pst;

    self->config_st.driver_pst->start_tx(self->config_st.driver_pst, t_pst->slave_addr_u16, t_pst->write_data_au8, t_pst->write_cnt_u16);
}

void i2c_master_stop(chsm_tst *_self, const cevent_tst *e_pst)
{
    i2c_master_tst *self = (i2c_master_tst *)_self;

    self->config_st.driver_pst->stop(self->config_st.driver_pst);
}