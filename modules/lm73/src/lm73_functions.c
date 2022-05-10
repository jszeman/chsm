#include "lm73.h"
#include "lm73_regs.h"
#include "lm73_functions.h"
#include <stdio.h>

#ifdef MINGW_BUILD
#define STATE_PRINT() printf("\n%s\n", __FUNCTION__);
#else
#define STATE_PRINT()
#endif


void lm73_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();

    lm73_tst*   self =          (lm73_tst *)_self;

    self->counter_u32 =         0;
    self->error_counter_u32 =   0;

    self->t_st.super.sig =      SIG_I2C_WR_TRANSACTION;
    self->t_st.slave_addr_u16 = self->config_st.address_u8;
    self->t_st.target_q_pst =   (cqueue_tst *)self;
    self->t_st.read_data_pu8 =  self->rx_buff_au8;
    self->t_st.write_data_pu8 = self->tx_buff_au8;
}

/*Increase the timer counter.*/
void lm73_10ms_callback(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    self->counter_u32 += 10;
}

/*Increase the error counter.*/
void lm73_inc_error_counter(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    self->error_counter_u32++;
}

/*Try to read the ID register from the LM73 by sending a write-read transaction to the I2C master.*/
void lm73_read_id(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    self->t_st.super.sig =      SIG_I2C_WR_TRANSACTION;
    self->t_st.write_cnt_u16 =  1;
    self->t_st.read_cnt_u16 =   2;
    self->tx_buff_au8[0] =      LM73_REG_ID;
    self->rx_buff_au8[0] =      0;
    self->rx_buff_au8[1] =      0;

    self->super.send(_self, (const cevent_tst *)(&self->t_st));
}

void lm73_reset_pointer(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    self->t_st.super.sig =      SIG_I2C_W_TRANSACTION;
    self->t_st.write_cnt_u16 =  1;
    self->t_st.read_cnt_u16 =   0;
    self->tx_buff_au8[0] =      0;

    self->super.send(_self, (const cevent_tst *)(&self->t_st));
}

/*Send a SIG_LM73_OFFLINE event. This can be used to detect communication errors between the module and the I2C slave.*/
void send_offline_event(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    self->valid_b = false;

    TYPEOF(SIG_LM73_OFFLINE)* offline_pst = CRF_NEW(SIG_LM73_OFFLINE);

    offline_pst->id_u16 = self->config_st.id_u16;

    CRF_EMIT(offline_pst);
}

/*Send a SIG_LM73_ONLINE event. This can be used to detect successful initialization.*/
void send_online_event(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    TYPEOF(SIG_LM73_ONLINE)* online_pst = CRF_NEW(SIG_LM73_ONLINE);

    online_pst->id_u16 = self->config_st.id_u16;

    CRF_EMIT(online_pst);
}

/*Reset the timer counter.*/
void lm73_reset_timer(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();
    
    lm73_tst*   self = (lm73_tst *)_self;

    self->counter_u32 = 0;
}

/*Send a read transaction to the LM73.*/
void lm73_start_read(chsm_tst *_self, const cevent_tst *e_pst)
{    
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    self->t_st.super.sig =      SIG_I2C_R_TRANSACTION;
    self->t_st.write_cnt_u16 =  0;
    self->t_st.read_cnt_u16 =   2;
//    self->tx_buff_au8[0] =      LM73_REG_TEMPERATURE;
    self->rx_buff_au8[0] =      0;
    self->rx_buff_au8[1] =      0;

    self->super.send(_self, (const cevent_tst *)(&self->t_st));
}

void lm73_set_full_powerdown(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    self->t_st.super.sig =      SIG_I2C_W_TRANSACTION;
    self->t_st.write_cnt_u16 =  2;
    self->t_st.read_cnt_u16 =   0;
    self->tx_buff_au8[0] =      LM73_REG_CONFIG;
    self->tx_buff_au8[0] =      0x80;

    self->super.send(_self, (const cevent_tst *)(&self->t_st));
}
void lm73_set_full_powerup(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    self->t_st.super.sig =      SIG_I2C_W_TRANSACTION;
    self->t_st.write_cnt_u16 =  2;
    self->t_st.read_cnt_u16 =   0;
    self->tx_buff_au8[0] =      LM73_REG_CONFIG;
    self->tx_buff_au8[0] =      0x00;

    self->super.send(_self, (const cevent_tst *)(&self->t_st));
}

void lm73_get_resolution(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    self->t_st.super.sig =      SIG_I2C_WR_TRANSACTION;
    self->t_st.write_cnt_u16 =  1;
    self->t_st.read_cnt_u16 =   1;
    self->tx_buff_au8[0] =      LM73_REG_CTRLSTATUS;
    self->rx_buff_au8[0] =      0;

    self->super.send(_self, (const cevent_tst *)(&self->t_st));
}

void lm73_update_resolution(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;
    uint16_t reg_u16 = self->rx_buff_au8[0];
    lm73_resolution_t resolution = LM73_RESOLUTION_14BIT;

    self->resolution_u16 = (reg_u16 & LM73_MASK_RESOLUTION) | resolution;
}

void lm73_set_resolution(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    self->t_st.super.sig =      SIG_I2C_W_TRANSACTION;
    self->t_st.write_cnt_u16 =  2;
    self->t_st.read_cnt_u16 =   0;
    self->tx_buff_au8[0] =      LM73_REG_CTRLSTATUS;
    self->tx_buff_au8[0] =      0x60;

    self->super.send(_self, (const cevent_tst *)(&self->t_st));
}

/*Update the temperature display and send an event with the new value.*/
void lm73_update_temp(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();
    
    lm73_tst*   self = (lm73_tst *)_self;

    lm73_temp_tst* temp_pst = CRF_NEW(SIG_LM73_TEMP); //lm73_temp_tst

    if (NULL == temp_pst) return;

    // Compile the value of the Temperature Data Register into a variable
    uint32_t tdr_reg_u32 = (self->rx_buff_au8[0] << 8) | self->rx_buff_au8[1];

    // Convert the TDR reg format to Celsius:
    //  1: remove the sign bit
    //  2: shift down 7 bits
    //  3: restore the sign
   int16_t temp_c_i32 = tdr_reg_u32 >> 7;
   temp_c_i32 |= (tdr_reg_u32 & 0x8000) ? 0xfe00 : 0;
    // int32_t temp_c_i32 = (tdr_reg_u32 * 250) >> 5;
    //printf("%x --> %d\n", tdr_reg_u16, temp_c_i16);

    temp_pst->super.sig = SIG_LM73_TEMP;
    temp_pst->temp_C_i32 = temp_c_i32;
    temp_pst->id_u16 = self->config_st.id_u16;

    self->temp_C_i32 = temp_c_i32;
    self->valid_b = true;
    
    self->super.send(_self, (const cevent_tst *)temp_pst);

    if (self->error_counter_u32)
    {
        self->error_counter_u32--;
    }
}

/*True, if the response data is equal to 0x190. See LM73 datasheet section: 7.5.1.7 Identification Register*/
bool lm73_id_match(chsm_tst *_self, const cevent_tst *e_pst)
{
    STATE_PRINT();
    
    lm73_tst*   self = (lm73_tst *)_self;

    // Compile the value of the Identification Register into a variable
    uint16_t idr_reg_u16 = self->rx_buff_au8[0];
    idr_reg_u16 <<= 8;
    idr_reg_u16 |= self->rx_buff_au8[1];

    //printf("%x\n", idr_reg_u16);

    return LM73_ID_REG_VALUE == idr_reg_u16;
}

/*Return true, if the error counter is greater or equal then the parameter.*/
bool lm73_error_count(chsm_tst *_self, const cevent_tst *e_pst, uint16_t error_cnt_threshold_u16)
{   
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    return self->error_counter_u32 >= error_cnt_threshold_u16;
}

bool lm73_timeout(chsm_tst *_self, const cevent_tst *e_pst, uint32_t timeout_u32)
{  
    STATE_PRINT();

    lm73_tst*   self = (lm73_tst *)_self;

    return self->counter_u32 >= timeout_u32;
}
