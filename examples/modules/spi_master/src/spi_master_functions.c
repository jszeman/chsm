#include "crf.h"
#include "cevent.h"
#include "spi_master.h"
#include <stdio.h>


static const cevent_tst success_response_st = {.sig = SIG_SPI_MASTER_RESULT_SUCCESS};
static const cevent_tst fail_response_st = {.sig = SIG_SPI_MASTER_RESULT_FAIL};

void spi_master_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    spi_master_tst* self = (spi_master_tst*)_self;

    self->config_st.driver_pst->q_pst = (cqueue_tst *)self;
}

void spi_master_send_fail(chsm_tst* _self, const cevent_tst *e_pst)
{
    spi_master_tst* self = (spi_master_tst*)_self;

    CRF_POST(&fail_response_st, self->cached_tr_st.target_q_pst);
}

void spi_master_send_success(chsm_tst* _self, const cevent_tst *e_pst)
{
    spi_master_tst* self = (spi_master_tst*)_self;

    CRF_POST(&success_response_st, self->cached_tr_st.target_q_pst);
}

void spi_master_start(chsm_tst* _self, const cevent_tst *e_pst)
{
    spi_master_tst* self = (spi_master_tst*)_self;
    CRF_SIG_VAR(SIG_SPI_MASTER_TRANSACTION, t_pst, e_pst);

    self->cached_tr_st = *t_pst;

    self->config_st.driver_pst->start(self->config_st.driver_pst,
                                      t_pst->slave_idx_u8,
                                      t_pst->tx_data_pu8,
                                      t_pst->rx_data_pu8,
                                      t_pst->len_u16);
}

