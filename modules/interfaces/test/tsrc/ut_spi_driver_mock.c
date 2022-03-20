#include "spi_driver_if.h"
#include "ut_spi_driver_mock.h"
#include "crf.h"

static const cevent_tst spi_drv_succes_event_st = {.sig = SIG_SPI_DRIVER_TRANSFER_SUCCESS};
static const cevent_tst spi_drv_fail_event_st = {.sig = SIG_SPI_DRIVER_TRANSFER_FAIL};

static void start(spi_driver_if_tst *_self, uint8_t slave_idx_u8,
                  uint8_t *tx_data_pu8, uint8_t *rx_data_pu8, uint16_t len_u16)
{
    ut_spi_driver_mock_tst *self = (ut_spi_driver_mock_tst *)_self;

    if (slave_idx_u8 >= self->slave_count_u16)
    {
        self->actual_slave_pst = NULL;
        self->intf_st.q_pst->put(self->intf_st.q_pst, &spi_drv_fail_event_st);
        return;
    }

    self->actual_slave_pst = self->slave_pst + slave_idx_u8;
    self->tx_buff_pu8 = tx_data_pu8;
    self->rx_buff_pu8 = rx_data_pu8;
    self->len_u16 = len_u16;
}


static void tick(ut_spi_driver_mock_tst* self)
{
    if (NULL == self->actual_slave_pst)
    {
        return;
    }

    if (NULL != self->tx_buff_pu8)
    {
        self->actual_slave_pst->rx_data_au8[self->actual_slave_pst->idx_u16] = *self->tx_buff_pu8++;
    }

    if (NULL != self->rx_buff_pu8)
    {
        *self->rx_buff_pu8++ = self->actual_slave_pst->tx_data_au8[self->actual_slave_pst->idx_u16]; 
    }

    self->actual_slave_pst->idx_u16++;

    self->len_u16--;

    if (0 == self->len_u16)
    {
        self->actual_slave_pst = NULL;
        self->intf_st.q_pst->put(self->intf_st.q_pst, &spi_drv_succes_event_st);
    }
}

void ut_spi_driver_mock_init(ut_spi_driver_mock_tst* self)
{
    self->tick = tick;
    self->intf_st.start = start;
}