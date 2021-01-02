#include "fram_functions.h"
#include "fram.h"
#include <stdio.h>


void fram_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    fram_tst*   self = (fram_tst *)_self;

    self->t_st.super.sig =      SIG_I2C_WR_TRANSACTION;
    self->t_st.slave_addr_u16 = self->config_st.address_u8;
    self->t_st.target_q_pst =   (cqueue_tst *)self;
    self->t_st.read_data_pu8 =  self->rx_buff_au8;
    self->t_st.write_data_pu8 = self->tx_buff_au8;
}


/*Prepare and send a transaction to the I2C master to read a chunk of data from the FRAM.*/
void read_first_chunk(chsm_tst *_self, const cevent_tst *e_pst)
{
    printf("\n%s ", __FUNCTION__);

    fram_tst*   self = (fram_tst *)_self;

    const mem_op_tst *op_st = (const mem_op_tst *)e_pst;

    self->t_st.super.sig =      SIG_I2C_WR_TRANSACTION;
    self->t_st.write_cnt_u16 =  2;
    self->t_st.read_cnt_u16 =   self->op_st.len_u32;
    self->t_st.read_data_pu8 =  self->op_st.buff_pu8;
    self->tx_buff_au8[0] =      (self->op_st.address_u32 >> 8) & 0xff;
    self->tx_buff_au8[1] =      self->op_st.address_u32 & 0xff;

    self->super.send(_self, (const cevent_tst *)(&self->t_st));

    self->op_st.len_u32 -= self->t_st.read_cnt_u16;
}

/*If there is more data to be read then prepare and send a transaction to the I2C master to read a chunk of data from the FRAM.*/
void read_next_chunk(chsm_tst *_self, const cevent_tst *e_pst)
{
    fram_tst*   self = (fram_tst *)_self;
}

/*Send a read fail event to the queue designated by the read event.*/
void send_read_fail_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    fram_tst*   self = (fram_tst *)_self;
}


mem_status_tst read_success_event_st = {.super.sig = SIG_MEM_READ_SUCCESS};

/*Send a read success event to the queue designated by the read event.*/
void send_read_success_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;

    self->op_st.q_pst->put(self->op_st.q_pst, (const cevent_tst *)&read_success_event_st);
}

/*Make a local copy of the operation event so fields can be used later. */
void store_op_info(chsm_tst *_self, const cevent_tst *e_pst)
{
    printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;

    const mem_op_tst *op_st = (const mem_op_tst *)e_pst;

    self->op_st = *op_st;
}

/*Send a write fail event to the queue designated by the read event.*/
void send_write_fail_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;

    mem_status_tst* status_pst = CRF_NEW_EVENT(mem_status_tst);

    if (NULL == status_pst) return;

    status_pst->super.sig = SIG_MEM_READ_FAIL;

    self->op_st.q_pst->put(self->op_st.q_pst, (const cevent_tst *)status_pst);
}

/*Send a write success event to the queue designated by the read event.*/
void send_write_success_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;
}

/*Prepare and send a transaction to the I2C master to write a chunk of data to the FRAM.*/
void write_first_chunk(chsm_tst *_self, const cevent_tst *e_pst)
{
    printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;
}

/*If there is more data to be sent then prepare and send a transaction to the I2C master to write a chunk of data to the FRAM.*/
void write_next_chunk(chsm_tst *_self, const cevent_tst *e_pst)
{
    printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;
}

/*Returns true, if we don't need to read more data from the FRAM.*/
bool all_data_read(chsm_tst *_self, const cevent_tst *e_pst)
{
    printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;

    return 0 == self->op_st.len_u32 ? true : false;
}

/*Returns true, if we don't need to write more data to the FRAM.*/
bool all_data_written(chsm_tst *_self, const cevent_tst *e_pst)
{
    printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;
    return false;
}