/*Generated with CHSM v0.0.0 at 2022.11.22 13.30.18*/
#include "cevent.h"
#include "chsm.h"
#include "spi_master.h"
#include "spi_master_functions.h"


static chsm_result_ten s_spi_master_busy(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_spi_master_idle(chsm_tst *self, const cevent_tst  *e_pst);

static chsm_result_ten s_spi_master_idle(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_SPI_MASTER_TRANSACTION:
            spi_master_start(self, e_pst);
            return chsm_transition(self, s_spi_master_busy);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_spi_master_busy(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_SPI_MASTER_TRANSACTION:
            chsm_defer(self, e_pst);
            break;

        case SIG_SPI_DRIVER_TRANSFER_SUCCESS:
            spi_master_send_success(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_spi_master_idle);

        case SIG_SPI_DRIVER_TRANSFER_FAIL:
            spi_master_send_fail(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_spi_master_idle);
    }

    return chsm_ignored(self);
}

chsm_result_ten spi_master_top(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            spi_master_init(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_spi_master_idle);
    }

    return chsm_ignored(self);
}
