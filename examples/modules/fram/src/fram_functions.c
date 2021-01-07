#include "fram_functions.h"
#include "fram.h"
#include <stdio.h>


const mem_status_tst read_success_event_st = {.super.sig = SIG_MEM_READ_SUCCESS};
const mem_status_tst write_success_event_st = {.super.sig = SIG_MEM_WRITE_SUCCESS};

void fram_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    fram_tst*   self = (fram_tst *)_self;

    self->t_st.super.sig =      SIG_I2C_WR_TRANSACTION;
    self->t_st.slave_addr_u16 = self->config_st.address_u8;
    self->t_st.target_q_pst =   (cqueue_tst *)self;
    self->t_st.read_data_pu8 =  self->rx_buff_au8;
    self->t_st.write_data_pu8 = self->tx_buff_au8;
}

/*Prepare and send a transaction to the I2C master to write a chunk of data to the FRAM.*/
void write_a_chunk(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s ", __FUNCTION__);

    fram_tst*   self = (fram_tst *)_self;

    self->bytes_remaining_u32 -= self->t_st.write_cnt_u16 > 2 ? self->t_st.write_cnt_u16 - 2 : 0;

    if (0 == self->bytes_remaining_u32) return;

    /* Calculate the transfer length. */
    uint8_t size_u8 = self->op_st.len_u32 < self->config_st.chunk_size_u8
        ?
            self->op_st.len_u32
        :
            self->config_st.chunk_size_u8;

    /*
     * Prepare the transaction structure. Some of the fields are prepared in the init function.
     */

    // The FRAMs use 17 bit address, where the 17th bit is the LSB of the slave address
    self->t_st.slave_addr_u16 = (self->op_st.address_u32 > 0xffff)
        ?
            (self->config_st.address_u8 | 1)
        :
            self->config_st.address_u8;
    
    self->t_st.write_cnt_u16 =  2 + size_u8; // Two address bytes + data length
    self->t_st.super.sig =      SIG_I2C_W_TRANSACTION;

    /*
     * Fill up the write buffer. The first 2 bytes are the address the rest is data.
     */

    self->tx_buff_au8[0] =      (self->op_st.address_u32 >> 8) & 0xff;
    self->tx_buff_au8[1] =      self->op_st.address_u32 & 0xff;

    for (uint32_t i = 0; i < size_u8; i++)
    {
        self->tx_buff_au8[i+2] = *(self->op_st.buff_pu8)++;
    }

    self->super.send(_self, (const cevent_tst *)(&self->t_st));

    /*
     * Prepare for the next chunk.
     */
    self->op_st.len_u32 -= size_u8;
    self->op_st.address_u32 += size_u8;
}


/*Prepare and send a transaction to the I2C master to read a chunk of data from the FRAM.*/
void read_a_chunk(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s ", __FUNCTION__);

    fram_tst*   self = (fram_tst *)_self;

    self->bytes_remaining_u32 -= self->t_st.read_cnt_u16;

    //printf("%d %x ", self->bytes_remaining_u32, self->op_st.address_u32);

    if (0 == self->bytes_remaining_u32) return;

    /* Calculate the transfer length. */
    uint8_t size_u8 = self->op_st.len_u32 < self->config_st.chunk_size_u8
        ?
            self->op_st.len_u32
        :
            self->config_st.chunk_size_u8;

    self->t_st.slave_addr_u16 = (self->op_st.address_u32 > 0xffff)
                                ?
                                    (self->config_st.address_u8 | 1)
                                :
                                    self->config_st.address_u8;
    self->t_st.super.sig =      SIG_I2C_WR_TRANSACTION;
    self->t_st.write_cnt_u16 =  2;
    self->t_st.read_cnt_u16 =   size_u8;
    self->t_st.read_data_pu8 =  self->op_st.buff_pu8;
    self->tx_buff_au8[0] =      (self->op_st.address_u32 >> 8) & 0xff;
    self->tx_buff_au8[1] =      self->op_st.address_u32 & 0xff;

    self->super.send(_self, (const cevent_tst *)(&self->t_st));

    

    /*
     * Prepare for the next chunk.
     */
    self->op_st.len_u32 -= size_u8;
    self->op_st.address_u32 += size_u8;
    self->op_st.buff_pu8 += size_u8;
}

/*Send a read fail event to the queue designated by the read event.*/
void send_read_fail_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s ", __FUNCTION__);

    fram_tst*   self = (fram_tst *)_self;

    TYPEOF(SIG_MEM_READ_FAIL)* status_pst = CRF_NEW(SIG_MEM_READ_FAIL);

    if (NULL == status_pst) return;

    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_ADDR_NACK:
            status_pst->error_en = MEM_EC_HW_ACCESS_ERROR;
            break;

        case  SIG_I2C_RESULT_DATA_NACK:
            status_pst->error_en = MEM_EC_INVALID_ADDRESS;
            break;
    }

    status_pst->address_u32 = self->op_st.address_u32 - self->t_st.read_cnt_u16;
    
    CRF_POST(status_pst, self->op_st.q_pst);
}

/*Send a read success event to the queue designated by the read event.*/
void send_read_success_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;

    CRF_POST(&read_success_event_st, self->op_st.q_pst);
}

/*Make a local copy of the operation event so fields can be used later. */
void store_op_info(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;

    const mem_op_tst *op_st = (const mem_op_tst *)e_pst;

    self->op_st = *op_st;

    self->bytes_remaining_u32 = self->op_st.len_u32;
    self->t_st.read_cnt_u16 = 0;
    self->t_st.write_cnt_u16 = 0;

    //printf(" %d ", self->op_st.len_u32);
}

void clear_op_info(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;

    self->op_st = (mem_op_tst){0};
    self->bytes_remaining_u32 = 0;
}

/*Send a write fail event to the queue designated by the read event.*/
void send_write_fail_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;

    TYPEOF(SIG_MEM_WRITE_FAIL)* status_pst = CRF_NEW(SIG_MEM_WRITE_FAIL);

    if (NULL == status_pst) return;

    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_ADDR_NACK:
            status_pst->error_en = MEM_EC_HW_ACCESS_ERROR;
            break;

        case  SIG_I2C_RESULT_DATA_NACK:
            status_pst->error_en = MEM_EC_INVALID_ADDRESS;
            break;
    }

    status_pst->address_u32 = self->op_st.address_u32 - (self->t_st.write_cnt_u16 - 2);

    CRF_POST(status_pst, self->op_st.q_pst);
}

/*Send a write success event to the queue designated by the read event.*/
void send_write_success_response(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;

    self->op_st.q_pst->put(self->op_st.q_pst, (const cevent_tst *)&write_success_event_st);
}

/*Returns true, if we don't need to read more data from the FRAM.*/
bool all_data_read(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;

    return 0 == self->bytes_remaining_u32 ? true : false;
}

/*Returns true, if we don't need to write more data to the FRAM.*/
bool all_data_written(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;

    return 0 == self->op_st.len_u32 ? true : false;
}


bool last_transaction(chsm_tst *_self, const cevent_tst *e_pst)
{
    //printf("\n%s ", __FUNCTION__);
    
    fram_tst*   self = (fram_tst *)_self;

    return 0 == self->bytes_remaining_u32 ? true : false;
}
