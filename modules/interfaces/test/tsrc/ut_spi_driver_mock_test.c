/*
 * ut_spi_driver_mock_test.c
 *
 *  Created on: 2020. dec. 19.
 *      Author: jszeman
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "unity_fixture.h"
#include "crf.h"
#include "cbits.h"
#include "ut_spi_driver_mock.h"

TEST_GROUP(spi_drv_mock);

static ut_spi_driver_mock_tst  drv_mock_st;
static spi_driver_if_tst*      drv_pst = (spi_driver_if_tst *)&drv_mock_st;

static const cevent_tst*       events_apst[4];
static cqueue_tst              q_st;

static void drv_tick(uint32_t tick_cnt_u32)
{
    for (uint32_t i=0; i<tick_cnt_u32; i++)
    {
        drv_mock_st.tick(&drv_mock_st);
    }
}

TEST_SETUP(spi_drv_mock)
{
    memset(events_apst, 0, sizeof events_apst);
    memset(&q_st, 0, sizeof q_st);
    memset(&drv_mock_st, 0, sizeof drv_mock_st);

    cqueue_init(&q_st, events_apst, 4);

    drv_mock_st.intf_st.q_pst = &q_st;

    ut_spi_driver_mock_init(&drv_mock_st);

    TEST_ASSERT(drv_mock_st.intf_st.start);
}

TEST_TEAR_DOWN(spi_drv_mock)
{
}

/*
 * Just call setup
 */
TEST(spi_drv_mock, init)
{
}

/*
 * write_one_byte
 */
TEST(spi_drv_mock, write_one_byte)
{
    spi_mock_slave_device_tst dev_st = {0};

    uint8_t tx_data_au8[4] = {0x5a, 0, 0, 0};

    drv_mock_st.slave_pst = &dev_st;
    drv_mock_st.slave_count_u16 = 1;

    drv_pst->start(drv_pst, 0, tx_data_au8, NULL, 1);

    drv_tick(5);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT_NOT_NULL(e_pst);
    TEST_ASSERT_EQUAL(SIG_SPI_DRIVER_TRANSFER_SUCCESS, e_pst->sig);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(tx_data_au8, dev_st.rx_data_au8, 1);
}

/*
 * write_two_bytes
 */
TEST(spi_drv_mock, write_two_bytes)
{
    spi_mock_slave_device_tst dev_st = {0};

    uint8_t tx_data_au8[4] = {0x5a, 0x6b, 0, 0};

    drv_mock_st.slave_pst = &dev_st;
    drv_mock_st.slave_count_u16 = 1;

    drv_pst->start(drv_pst, 0, tx_data_au8, NULL, 2);

    drv_tick(5);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT_NOT_NULL(e_pst);
    TEST_ASSERT_EQUAL(SIG_SPI_DRIVER_TRANSFER_SUCCESS, e_pst->sig);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(tx_data_au8, dev_st.rx_data_au8, 2);
}


/*
 * read_one_byte
 */
TEST(spi_drv_mock, read_one_byte)
{
    spi_mock_slave_device_tst dev_st = {.tx_data_au8 = {0x5a, 0, 0, }};

    uint8_t rx_data_au8[4] = {0, 0, 0, 0};

    drv_mock_st.slave_pst = &dev_st;
    drv_mock_st.slave_count_u16 = 1;

    drv_pst->start(drv_pst, 0, NULL, rx_data_au8, 1);

    drv_tick(5);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT_NOT_NULL(e_pst);
    TEST_ASSERT_EQUAL(SIG_SPI_DRIVER_TRANSFER_SUCCESS, e_pst->sig);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(dev_st.tx_data_au8, rx_data_au8, 1);
}


/*
 * read_two_bytes
 */
TEST(spi_drv_mock, read_two_bytes)
{
    spi_mock_slave_device_tst dev_st = {.tx_data_au8 = {0x5a, 0x6b, 0, }};

    uint8_t rx_data_au8[4] = {0, 0, 0, 0};

    drv_mock_st.slave_pst = &dev_st;
    drv_mock_st.slave_count_u16 = 1;

    drv_pst->start(drv_pst, 0, NULL, rx_data_au8, 2);

    drv_tick(5);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT_NOT_NULL(e_pst);
    TEST_ASSERT_EQUAL(SIG_SPI_DRIVER_TRANSFER_SUCCESS, e_pst->sig);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(dev_st.tx_data_au8, rx_data_au8, 2);
}

/*
 * read_write_two_bytes
 */
TEST(spi_drv_mock, read_write_two_bytes)
{
    spi_mock_slave_device_tst dev_st = {.tx_data_au8 = {0x5a, 0x6b, 0, }};

    uint8_t rx_data_au8[4] = {0, 0, 0, 0};
    uint8_t tx_data_au8[4] = {0x7c, 0x8d, 0, 0};


    drv_mock_st.slave_pst = &dev_st;
    drv_mock_st.slave_count_u16 = 1;

    drv_pst->start(drv_pst, 0, tx_data_au8, rx_data_au8, 2);

    drv_tick(5);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT_NOT_NULL(e_pst);
    TEST_ASSERT_EQUAL(SIG_SPI_DRIVER_TRANSFER_SUCCESS, e_pst->sig);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(dev_st.tx_data_au8, rx_data_au8, 2);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(tx_data_au8, dev_st.rx_data_au8, 2);
}
    


TEST_GROUP_RUNNER(spi_drv_mock)
{
	RUN_TEST_CASE(spi_drv_mock, init);
	RUN_TEST_CASE(spi_drv_mock, write_one_byte);
	RUN_TEST_CASE(spi_drv_mock, write_two_bytes);
	RUN_TEST_CASE(spi_drv_mock, read_one_byte);
	RUN_TEST_CASE(spi_drv_mock, read_two_bytes);
	RUN_TEST_CASE(spi_drv_mock, read_write_two_bytes);

}
