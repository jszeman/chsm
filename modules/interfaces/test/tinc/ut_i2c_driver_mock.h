#ifndef UT_I2C_DRIVER_MOCK_H
#define UT_I2C_DRIVER_MOCK_H

#include <stdint.h>
#include <stdbool.h>

#include "i2c_driver_if.h"

#define I2C_START       0x0100
#define I2C_STOP        0x0200
#define I2C_ACK         0x0400
#define I2C_NACK        0x0800
#define I2C_WRITE       0x1000
#define I2C_READ        0x2000
#define I2C_BLANK       0

#define I2C_MOCK_BUFFER_SIZE    256

typedef struct i2c_mock_slave_device_tst
{
    uint8_t     address_u8;
    uint8_t     tx_data_au8[I2C_MOCK_BUFFER_SIZE];
    uint8_t     rx_data_au8[I2C_MOCK_BUFFER_SIZE];
    uint16_t    nack_idx_u16;
    bool        busy_b;

    /* PRIVATE - shall be 0 */
    uint16_t    tx_idx_u16;
    uint16_t    rx_idx_u16;
} i2c_mock_slave_device_tst;

typedef struct ut_i2c_driver_mock_tst ut_i2c_driver_mock_tst;

struct ut_i2c_driver_mock_tst
{
    i2c_driver_if_tst           intf_st;

    i2c_mock_slave_device_tst*  slave_pst;
    uint16_t                    slave_count_u16;

    void                        (*tick)(ut_i2c_driver_mock_tst* self);

    i2c_mock_slave_device_tst*  actual_slave_pst;
    uint8_t                     *buff_pu8;
    uint16_t                    len_u16;
    bool                        reading_b;
};

void ut_i2c_driver_mock_init(ut_i2c_driver_mock_tst *self);

#endif // UT_I2C_DRIVER_MOCK_H