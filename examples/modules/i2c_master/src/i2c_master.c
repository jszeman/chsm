/*Generated with CHSM v0.0.0 at 2020.12.29 08.44.03*/
#include "cevent.h"
#include "chsm.h"
#include "i2c_master.h"
#include "i2c_master_functions.h"


static chsm_result_ten s_i2c_master(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_wr_read(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_wr_write(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_write_read(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_read(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_write(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_busy(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_W_TRANSACTION:
            chsm_exit_children(self, e_pst, ctx_pst);
            store_transaction_info(self, e_pst);
            i2c_master_start_tx(self, e_pst);
            return chsm_transition(self, s_write);

        case SIG_I2C_R_TRANSACTION:
            chsm_exit_children(self, e_pst, ctx_pst);
            store_transaction_info(self, e_pst);
            i2c_master_start_rx(self, e_pst);
            return chsm_transition(self, s_read);

        case SIG_I2C_WR_TRANSACTION:
            chsm_exit_children(self, e_pst, ctx_pst);
            store_transaction_info(self, e_pst);
            i2c_master_start_tx(self, e_pst);
            return chsm_transition(self, s_wr_write);

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_i2c_master, NULL, guards_only_b);
}

static chsm_result_ten s_busy(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_W_TRANSACTION:
            chsm_defer(self, e_pst);
            break;

        case SIG_I2C_R_TRANSACTION:
            chsm_defer(self, e_pst);
            break;

        case SIG_I2C_WR_TRANSACTION:
            chsm_defer(self, e_pst);
            break;

        case SIG_I2C_WRITE_FAIL:
            chsm_exit_children(self, e_pst, ctx_pst);
            i2c_master_send_fail_response(self, e_pst);
            i2c_master_stop(self, e_pst);
            chsm_recall(self, e_pst);
            clear_transaction_info(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_I2C_READ_FAIL:
            chsm_exit_children(self, e_pst, ctx_pst);
            i2c_master_send_fail_response(self, e_pst);
            i2c_master_stop(self, e_pst);
            chsm_recall(self, e_pst);
            clear_transaction_info(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_i2c_master, i2c_master_stop, guards_only_b);
}

static chsm_result_ten s_write(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_WRITE_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            i2c_master_send_success_response(self, e_pst);
            i2c_master_stop(self, e_pst);
            chsm_recall(self, e_pst);
            clear_transaction_info(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_busy, NULL, guards_only_b);
}

static chsm_result_ten s_read(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_READ_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            i2c_master_send_success_response(self, e_pst);
            i2c_master_stop(self, e_pst);
            chsm_recall(self, e_pst);
            clear_transaction_info(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_busy, NULL, guards_only_b);
}

static chsm_result_ten s_write_read(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_busy, NULL, guards_only_b);
}

static chsm_result_ten s_wr_write(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_WRITE_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            i2c_master_start_rx(self, e_pst);
            return chsm_transition(self, s_wr_read);

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_write_read, NULL, guards_only_b);
}

static chsm_result_ten s_wr_read(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_READ_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            i2c_master_send_success_response(self, e_pst);
            i2c_master_stop(self, e_pst);
            chsm_recall(self, e_pst);
            clear_transaction_info(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_write_read, NULL, guards_only_b);
}

static chsm_result_ten s_i2c_master(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, i2c_master_top, NULL, guards_only_b);
}

chsm_result_ten i2c_master_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            i2c_master_init(self, e_pst);
            chsm_recall(self, e_pst);
            clear_transaction_info(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
        guards_only_b=false;
    }

    return chsm_ignored(self);
}
