/*Generated with CHSM v0.0.0 at 2021.01.02 21.06.03*/
#include "cevent.h"
#include "chsm.h"
#include "fram.h"
#include "fram_functions.h"


static chsm_result_ten state_4(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_writing(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_reading(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_fram(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten s_fram(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, fram_top, NULL, guards_only_b);
}

static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_MEM_READ:
            chsm_exit_children(self, e_pst, ctx_pst);
            store_op_info(self, e_pst);
            read_first_chunk(self, e_pst);
            return chsm_transition(self, s_reading);

        case SIG_MEM_WRITE:
            chsm_exit_children(self, e_pst, ctx_pst);
            store_op_info(self, e_pst);
            write_first_chunk(self, e_pst);
            return chsm_transition(self, s_writing);

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_fram, NULL, guards_only_b);
}

static chsm_result_ten s_reading(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_SUCCESS:
            read_next_chunk(self, e_pst);
            if(all_data_read(self, e_pst))
            {
                chsm_exit_children(self, e_pst, ctx_pst);
                send_read_success_response(self, e_pst);
                return chsm_transition(self, s_idle);
            }
            break;

        case SIG_I2C_RESULT_ADDR_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            send_read_fail_response(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_I2C_RESULT_DATA_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            send_read_fail_response(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, state_4, NULL, guards_only_b);
}

static chsm_result_ten s_writing(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_SUCCESS:
            write_next_chunk(self, e_pst);
            if(all_data_written(self, e_pst))
            {
                chsm_exit_children(self, e_pst, ctx_pst);
                send_write_success_response(self, e_pst);
                return chsm_transition(self, s_idle);
            }
            break;

        case SIG_I2C_RESULT_ADDR_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            send_write_fail_response(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_I2C_RESULT_DATA_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            send_write_fail_response(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, state_4, NULL, guards_only_b);
}

static chsm_result_ten state_4(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_fram, NULL, guards_only_b);
}

chsm_result_ten fram_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            fram_init(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
        guards_only_b=false;
    }

    return chsm_ignored(self);
}
