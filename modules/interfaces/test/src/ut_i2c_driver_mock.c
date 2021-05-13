#include "ut_i2c_driver_mock.h"
#include "signals.h"
#include <stdio.h>

static const cevent_tst i2c_tx_success_st = {.sig = SIG_I2C_WRITE_SUCCESS};
static const cevent_tst i2c_tx_fail_st = {.sig = SIG_I2C_WRITE_FAIL};
static const cevent_tst i2c_rx_success_st = {.sig = SIG_I2C_READ_SUCCESS};
static const cevent_tst i2c_rx_fail_st = {.sig = SIG_I2C_READ_FAIL};

static i2c_mock_slave_device_tst* find_slave(ut_i2c_driver_mock_tst *self, uint8_t slave_addr_u8)
{
    i2c_mock_slave_device_tst* slave_pst = self->slave_pst;

    for (uint16_t i=0; i<self->slave_count_u16; i++)
    {
        if (slave_pst->address_u8 == slave_addr_u8)
        {
            return slave_pst;
        }

        slave_pst++;
    }

    return NULL;
}


static void tick(ut_i2c_driver_mock_tst* self)
{
    if (NULL == self->actual_slave_pst) return;
    if (NULL == self->buff_pu8) return;
    if (0 == self->len_u16) return;

    self->len_u16--;

    if (self->reading_b) // Reading from the slave.
    {
        *self->buff_pu8++ = self->actual_slave_pst->tx_data_au8[self->actual_slave_pst->tx_idx_u16++];

        if (0 == self->len_u16)
        {
            self->intf_st.q_pst->put(self->intf_st.q_pst, &i2c_rx_success_st);
        }
    }
    else // Writing to the slave.
    {
        self->actual_slave_pst->rx_data_au8[self->actual_slave_pst->rx_idx_u16++] = *self->buff_pu8++;

        // If the slave wants to NACK the next byte
        if (self->actual_slave_pst->rx_idx_u16 > self->actual_slave_pst->nack_idx_u16)
        {
            self->intf_st.status_un.bit_st.data_nack_u16 = 1;
            self->intf_st.q_pst->put(self->intf_st.q_pst, &i2c_tx_fail_st);
            self->len_u16 = 0;
        }
        else // The slave will receive the byte in the buffer
        {
            if (0 == self->len_u16)
            {
                self->intf_st.q_pst->put(self->intf_st.q_pst, &i2c_tx_success_st);
            }
        }
    }
}


static void start_tx(i2c_driver_if_tst *_self, uint8_t slave_addr_u8, uint8_t *data_pu8, uint16_t len_u16)
{
    ut_i2c_driver_mock_tst *self = (ut_i2c_driver_mock_tst *)_self;

    i2c_mock_slave_device_tst* slave_pst = find_slave(self, slave_addr_u8);

    if (NULL != slave_pst)
    {
        self->intf_st.status_un.bit_st.addr_nack_u16 = 0;
        slave_pst->busy_b = true;

        self->actual_slave_pst = slave_pst;
        self->buff_pu8 = data_pu8;
        self->len_u16 = len_u16;
        self->reading_b = false;
    }
    else
    {
        self->intf_st.status_un.bit_st.addr_nack_u16 = 1;
        self->intf_st.q_pst->put(self->intf_st.q_pst, &i2c_tx_fail_st);
    }
}

static void start_rx(i2c_driver_if_tst *_self, uint8_t slave_addr_u8, uint8_t *data_pu8, uint16_t len_u16)
{
    ut_i2c_driver_mock_tst *self = (ut_i2c_driver_mock_tst *)_self;

    i2c_mock_slave_device_tst* slave_pst = find_slave(self, slave_addr_u8);

    if (NULL != slave_pst)
    {
        self->intf_st.status_un.bit_st.addr_nack_u16 = 0;
        slave_pst->busy_b = true;

        self->actual_slave_pst = slave_pst;
        self->buff_pu8 = data_pu8;
        self->len_u16 = len_u16;
        self->reading_b = true;
    }
    else
    {
        self->intf_st.status_un.bit_st.addr_nack_u16 = 1;
        self->intf_st.q_pst->put(self->intf_st.q_pst, &i2c_rx_fail_st);
    }
}

static void stop(i2c_driver_if_tst *_self)
{
    ut_i2c_driver_mock_tst *self = (ut_i2c_driver_mock_tst *)_self;

    i2c_mock_slave_device_tst* slave_pst = self->slave_pst;

    for (uint16_t i=0; i<self->slave_count_u16; i++)
    {
        slave_pst->busy_b = false;

        slave_pst++;

    }

    self->len_u16 = 0;
    self->actual_slave_pst = NULL;
    self->buff_pu8 = NULL;
}

void ut_i2c_driver_mock_init(ut_i2c_driver_mock_tst *self)
{
    self->intf_st.start_rx = start_rx;
    self->intf_st.start_tx = start_tx;
    self->intf_st.stop = stop;

    self->intf_st.status_un.all_u16 = 0;
    self->slave_count_u16 = 1;

    self->tick = tick;
}