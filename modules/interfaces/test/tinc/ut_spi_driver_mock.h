#ifndef UT_SPI_DRIVER_MOCK_H
#define UT_SPI_DRIVER_MOCK_H

#include "crf.h"
#include "spi_driver_if.h"

#define SPI_MOCK_BUFFER_SIZE    256

typedef struct spi_mock_slave_device_tst
{
    uint8_t                     tx_data_au8[SPI_MOCK_BUFFER_SIZE];
    uint8_t                     rx_data_au8[SPI_MOCK_BUFFER_SIZE];
    uint16_t                    idx_u16;
} spi_mock_slave_device_tst;

typedef struct ut_spi_driver_mock_tst ut_spi_driver_mock_tst;

struct ut_spi_driver_mock_tst
{
    spi_driver_if_tst           intf_st;

    spi_mock_slave_device_tst*  slave_pst;
    uint16_t                    slave_count_u16;

    void                        (*tick)(ut_spi_driver_mock_tst* self);

    spi_mock_slave_device_tst*  actual_slave_pst;
    uint8_t*                    tx_buff_pu8;
    uint8_t*                    rx_buff_pu8;
    uint16_t                    len_u16;
};

void ut_spi_driver_mock_init(ut_spi_driver_mock_tst* self);

#endif