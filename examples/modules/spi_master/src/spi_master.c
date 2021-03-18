/*Generated with CHSM v0.0.0 at 2021.03.17 20.57.21*/
#include "cevent.h"
#include "chsm.h"
#include "spi_master.h"
#include "spi_master_functions.h"


static chsm_result_ten s_spi_master_busy(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_spi_master_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_spi_master(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten s_spi_master(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
            guards_only_b = false;
    }

    return chsm_handle_in_parent(self, ctx_pst, spi_master_top, NULL, guards_only_b);
}

static chsm_result_ten s_spi_master_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_SPI_MASTER_TRANSACTION:
            chsm_exit_children(self, e_pst, ctx_pst);
            spi_master_start(self, e_pst);
            return chsm_transition(self, s_spi_master_busy);

        default:
            guards_only_b = false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_spi_master, NULL, guards_only_b);
}

static chsm_result_ten s_spi_master_busy(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_SPI_MASTER_TRANSACTION:
            chsm_defer(self, e_pst);
            break;

        case SIG_SPI_DRIVER_TRANSFER_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            spi_master_send_success(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_spi_master_idle);

        case SIG_SPI_DRIVER_TRANSFER_FAIL:
            chsm_exit_children(self, e_pst, ctx_pst);
            spi_master_send_fail(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_spi_master_idle);

        default:
            guards_only_b = false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_spi_master, NULL, guards_only_b);
}

chsm_result_ten spi_master_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            spi_master_init(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_spi_master_idle);

        default:
            guards_only_b = false;
    }

    return chsm_ignored(self);
}
