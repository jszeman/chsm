#ifndef I2C_DRIVER_IF_H
#define I2C_DRIVER_IF_H

#include <stdint.h>
#include <stdbool.h>
#include "crf.h"
#include "signals.h"

typedef struct i2c_driver_status_bits_tst
{
    uint16_t    busy_u16: 1; // The peripheral is busy transmitting or receiving data.
    uint16_t    addr_nack_u16: 1;
    uint16_t    data_nack_u16: 1;
} i2c_driver_status_bits_tst;

typedef union i2c_driver_status_tun
{
    uint16_t                    all_u16;
    i2c_driver_status_bits_tst  bit_st;
} i2c_driver_status_tun;

typedef struct i2c_driver_if_tst i2c_driver_if_tst;

struct i2c_driver_if_tst
{
    uint16_t                (*start_tx)(i2c_driver_if_tst *self, uint8_t slave_addr_u8, uint8_t *data_pu8, uint16_t len_u16);
    uint16_t                (*start_rx)(i2c_driver_if_tst *self, uint8_t slave_addr_u8, uint8_t *data_pu8, uint16_t len_u16);
    uint16_t                (*stop)(i2c_driver_if_tst* self);

    cqueue_tst*             q_pst;
    
    i2c_driver_status_tun   status_un;
};

#endif