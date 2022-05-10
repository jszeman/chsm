/*
 * ut_i2c_driver_mock_test.c
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
#include "ut_i2c_driver_mock.h"

TEST_GROUP(i2c_drv_mock);

static ut_i2c_driver_mock_tst  drv_mock_st;
static i2c_driver_if_tst*      drv_pst = (i2c_driver_if_tst *)&drv_mock_st;

static const cevent_tst*       events_apst[4];
static cqueue_tst              q_st;

static void drv_tick(uint32_t tick_cnt_u32)
{
    for (uint32_t i=0; i<tick_cnt_u32; i++)
    {
        drv_mock_st.tick(&drv_mock_st);
    }
}

TEST_SETUP(i2c_drv_mock)
{
    memset(events_apst, 0, sizeof events_apst);
    memset(&q_st, 0, sizeof q_st);
    memset(&drv_mock_st, 0, sizeof drv_mock_st);

    cqueue_init(&q_st, events_apst, 4);

    drv_mock_st.intf_st.q_pst = &q_st;

    ut_i2c_driver_mock_init(&drv_mock_st);

    TEST_ASSERT(drv_mock_st.intf_st.start_tx);
    TEST_ASSERT(drv_mock_st.intf_st.start_rx);
    TEST_ASSERT(drv_mock_st.intf_st.stop);
}

TEST_TEAR_DOWN(i2c_drv_mock)
{
}

/*
 * Just call setup
 */
TEST(i2c_drv_mock, init)
{
}

/*
 * write_one_byte:
 * Start a frame then write one byte.
 */
TEST(i2c_drv_mock, write_one_byte)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 1
    };

    uint8_t data_au8[4] = {0x5a, 0, 0, 0};

    drv_mock_st.slave_pst = &dev_st;

    drv_pst->start_tx(drv_pst, 0x12, data_au8, 1);

    drv_tick(5);

    TEST_ASSERT(dev_st.busy_b);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.addr_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.data_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.busy_u16);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT_NOT_NULL(e_pst);
    TEST_ASSERT_EQUAL(SIG_I2C_WRITE_SUCCESS, e_pst->sig);

    drv_pst->stop(drv_pst);
    TEST_ASSERT_FALSE(dev_st.busy_b);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(data_au8, dev_st.rx_data_au8, 1);
}

/*
 * write_two_bytes:
 * Start a frame then write two bytes.
 */
TEST(i2c_drv_mock, write_two_bytes)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 2
    };

    uint8_t data_au8[4] = {0x5a, 0x6b, 0, 0};

    drv_mock_st.slave_pst = &dev_st;

    drv_pst->start_tx(drv_pst, 0x12, data_au8, 2);

    drv_tick(5);

    TEST_ASSERT(dev_st.busy_b);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.addr_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.data_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.busy_u16);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT(e_pst);
    TEST_ASSERT_EQUAL(SIG_I2C_WRITE_SUCCESS, e_pst->sig);

    drv_pst->stop(drv_pst);
    TEST_ASSERT_FALSE(dev_st.busy_b);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(data_au8, dev_st.rx_data_au8, 2);
}

/*
 * write_two_bytes_addr_nack:
 * Start a frame with the wrong address then write two bytes. Check, that data was not actually written.
 */
TEST(i2c_drv_mock, write_two_bytes_addr_nack)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 2
    };

    uint8_t data_au8[4] = {0x5a, 0x6b, 0, 0};

    drv_mock_st.slave_pst = &dev_st;

    drv_pst->start_tx(drv_pst, 0x13, data_au8, 2);
    drv_tick(5);
    TEST_ASSERT_FALSE(dev_st.busy_b);
    TEST_ASSERT_EQUAL(1, drv_pst->status_un.bit_st.addr_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.data_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.busy_u16);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT(e_pst);
    TEST_ASSERT_EQUAL(SIG_I2C_WRITE_FAIL, e_pst->sig);

    drv_pst->stop(drv_pst);
    TEST_ASSERT_FALSE(dev_st.busy_b);

    data_au8[0] = 0;
    data_au8[1] = 0;

    TEST_ASSERT_EQUAL_HEX8_ARRAY(data_au8, dev_st.rx_data_au8, 2);
}

/*
 * write_two_bytes_byte0_nack:
 * Start a frame then write two bytes to a slave that will NACK the first byte. Check, that data was not actually written.
 */
TEST(i2c_drv_mock, write_two_bytes_byte0_nack)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 0
    };

    uint8_t data_au8[4] = {0x5a, 0x6b, 0, 0};

    drv_mock_st.slave_pst = &dev_st;

    drv_pst->start_tx(drv_pst, 0x12, data_au8, 2);
    drv_tick(5);
    TEST_ASSERT(dev_st.busy_b);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.addr_nack_u16);
    TEST_ASSERT_EQUAL(1, drv_pst->status_un.bit_st.data_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.busy_u16);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT(e_pst);
    TEST_ASSERT_EQUAL(SIG_I2C_WRITE_FAIL, e_pst->sig);

    drv_pst->stop(drv_pst);
    TEST_ASSERT_FALSE(dev_st.busy_b);

    data_au8[0] = 0x5a;
    data_au8[1] = 0;

    TEST_ASSERT_EQUAL_HEX8_ARRAY(data_au8, dev_st.rx_data_au8, 2);
}

/*
 * write_two_bytes_byte1_nack:
 * Start a frame then write two bytes to a slave that will NACK the second byte. Check that only one byte was actually written.
 */
TEST(i2c_drv_mock, write_two_bytes_byte1_nack)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 1
    };

    uint8_t data_au8[4] = {0x5a, 0x6b, 0, 0};

    drv_mock_st.slave_pst = &dev_st;

    drv_pst->start_tx(drv_pst, 0x12, data_au8, 2);
    drv_tick(5);
    TEST_ASSERT(dev_st.busy_b);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.addr_nack_u16);
    TEST_ASSERT_EQUAL(1, drv_pst->status_un.bit_st.data_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.busy_u16);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT(e_pst);
    TEST_ASSERT_EQUAL(SIG_I2C_WRITE_FAIL, e_pst->sig);

    drv_pst->stop(drv_pst);
    TEST_ASSERT_FALSE(dev_st.busy_b);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(data_au8, dev_st.rx_data_au8, 2);
}

/*
 * read_one_byte:
 * Start a frame then read one byte.
 */
TEST(i2c_drv_mock, read_one_byte)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .tx_data_au8 = {0x7c, 0x8d, 0},
        .nack_idx_u16 = 1
    };

    uint8_t data_au8[4] = {0};

    drv_mock_st.slave_pst = &dev_st;

    drv_pst->start_rx(drv_pst, 0x12, data_au8, 1);
    drv_tick(5);
    TEST_ASSERT(dev_st.busy_b);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.addr_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.data_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.busy_u16);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT(e_pst);
    TEST_ASSERT_EQUAL(SIG_I2C_READ_SUCCESS, e_pst->sig);

    drv_pst->stop(drv_pst);
    TEST_ASSERT_FALSE(dev_st.busy_b);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(dev_st.tx_data_au8, data_au8, 1);
}

/*
 * read_two_bytes:
 * Start a frame then read two byts.
 */
TEST(i2c_drv_mock, read_two_bytes)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .tx_data_au8 = {0x7c, 0x8d, 0},
        .nack_idx_u16 = 1
    };

    uint8_t data_au8[4] = {0};

    drv_mock_st.slave_pst = &dev_st;

    drv_pst->start_rx(drv_pst, 0x12, data_au8, 2);
    drv_tick(5);
    TEST_ASSERT(dev_st.busy_b);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.addr_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.data_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.busy_u16);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT(e_pst);
    TEST_ASSERT_EQUAL(SIG_I2C_READ_SUCCESS, e_pst->sig);

    drv_pst->stop(drv_pst);
    TEST_ASSERT_FALSE(dev_st.busy_b);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(dev_st.tx_data_au8, data_au8, 2);
}

/*
 * read_two_bytes_addr_nack:
 * Start a frame then read two bytes but with wrong address.
 */
TEST(i2c_drv_mock, read_two_bytes_addr_nack)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .tx_data_au8 = {0x7c, 0x8d, 0},
        .nack_idx_u16 = 1
    };

    uint8_t data_au8[4] = {0};

    drv_mock_st.slave_pst = &dev_st;

    drv_pst->start_rx(drv_pst, 0x13, data_au8, 2);
    drv_tick(5);
    TEST_ASSERT_FALSE(dev_st.busy_b);
    TEST_ASSERT_EQUAL(1, drv_pst->status_un.bit_st.addr_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.data_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.busy_u16);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT(e_pst);
    TEST_ASSERT_EQUAL(SIG_I2C_READ_FAIL, e_pst->sig);

    drv_pst->stop(drv_pst);
    TEST_ASSERT_FALSE(dev_st.busy_b);

    uint8_t expected_data_au8[4] = {0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data_au8, data_au8, 2);
}

/*
 * write_2b_read_2b:
 * Write 2 bytes than restart and read 2 bytes.
 */
TEST(i2c_drv_mock, write_2b_read_2b)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .tx_data_au8 = {0x7c, 0x8d, 0},
        .nack_idx_u16 = 2
    };

    uint8_t tx_data_au8[4] = {0x5a, 0x6b};
    uint8_t rx_data_au8[4] = {0};

    drv_mock_st.slave_pst = &dev_st;

    drv_pst->start_tx(drv_pst, 0x12, tx_data_au8, 2);
    drv_tick(5);
    TEST_ASSERT(dev_st.busy_b);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.addr_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.data_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.busy_u16);

    const cevent_tst* e_pst = q_st.get(&q_st);
    TEST_ASSERT(e_pst);
    TEST_ASSERT_EQUAL(SIG_I2C_WRITE_SUCCESS, e_pst->sig);

    drv_pst->start_rx(drv_pst, 0x12, rx_data_au8, 2);
    drv_tick(5);
    TEST_ASSERT(dev_st.busy_b);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.addr_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.data_nack_u16);
    TEST_ASSERT_EQUAL(0, drv_pst->status_un.bit_st.busy_u16);
    
    e_pst = q_st.get(&q_st);
    TEST_ASSERT(e_pst);
    TEST_ASSERT_EQUAL(SIG_I2C_READ_SUCCESS, e_pst->sig);

    drv_pst->stop(drv_pst);
    TEST_ASSERT_FALSE(dev_st.busy_b);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(tx_data_au8, dev_st.rx_data_au8, 2);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(dev_st.tx_data_au8, rx_data_au8, 2);
}

TEST_GROUP_RUNNER(i2c_drv_mock)
{
	RUN_TEST_CASE(i2c_drv_mock, init);
	RUN_TEST_CASE(i2c_drv_mock, write_one_byte);
	RUN_TEST_CASE(i2c_drv_mock, write_two_bytes);
	RUN_TEST_CASE(i2c_drv_mock, write_two_bytes_addr_nack);
	RUN_TEST_CASE(i2c_drv_mock, write_two_bytes_byte0_nack);
	RUN_TEST_CASE(i2c_drv_mock, write_two_bytes_byte1_nack);
	RUN_TEST_CASE(i2c_drv_mock, read_one_byte);
	RUN_TEST_CASE(i2c_drv_mock, read_two_bytes);
	RUN_TEST_CASE(i2c_drv_mock, read_two_bytes_addr_nack);
	RUN_TEST_CASE(i2c_drv_mock, write_2b_read_2b);
	//RUN_TEST_CASE(i2c_drv_mock, init);
	//RUN_TEST_CASE(i2c_drv_mock, init);
	//RUN_TEST_CASE(i2c_drv_mock, init);
	//RUN_TEST_CASE(i2c_drv_mock, init);

}
