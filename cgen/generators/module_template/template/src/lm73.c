/*Generated with CHSM v0.0.0 at 2022.03.30 20.54.15*/
#include "cevent.h"
#include "chsm.h"
#include "lm73.h"
#include "lm73_functions.h"


static chsm_result_ten s_unplugged(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_get_resolution(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_set_config(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_set_resolution(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_init(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_online(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_reset_ptr_reg(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_read_id_reg(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_reading(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_lm73(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten s_lm73(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_SYS_TICK_10ms:
            lm73_10ms_callback(self, e_pst);
            break;

        default:
            guards_only_b = false;
    }

    return chsm_handle_in_parent(self, ctx_pst, lm73_top, NULL, guards_only_b);
}

static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_LM73_READ:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_start_read(self, e_pst);
            return chsm_transition(self, s_reading);

        default:
            guards_only_b = false;
    }


    if(lm73_timeout(self, e_pst, LM73_READ_PERIOD))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        lm73_reset_timer(self, e_pst);
        lm73_start_read(self, e_pst);
        return chsm_transition(self, s_reading);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_online, NULL, guards_only_b);
}

static chsm_result_ten s_reading(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_update_temp(self, e_pst);
            lm73_reset_timer(self, e_pst);
            lm73_reset_timer(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_I2C_RESULT_ADDR_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_inc_error_counter(self, e_pst);
            lm73_reset_timer(self, e_pst);
            lm73_reset_timer(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_I2C_RESULT_DATA_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_inc_error_counter(self, e_pst);
            lm73_reset_timer(self, e_pst);
            lm73_reset_timer(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
            guards_only_b = false;
    }

    if(lm73_timeout(self, e_pst, LM73_READ_PERIOD))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        lm73_inc_error_counter(self, e_pst);
        lm73_reset_timer(self, e_pst);
        lm73_reset_timer(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_online, lm73_reset_timer, guards_only_b);
}

static chsm_result_ten s_read_id_reg(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_ADDR_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_reset_timer(self, e_pst);
            return chsm_transition(self, s_unplugged);

        case SIG_I2C_RESULT_DATA_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_reset_timer(self, e_pst);
            return chsm_transition(self, s_unplugged);

        case SIG_I2C_RESULT_SUCCESS:
            if(lm73_id_match(self, e_pst))
            {
                chsm_exit_children(self, e_pst, ctx_pst);
                lm73_reset_timer(self, e_pst);
                lm73_reset_pointer(self, e_pst);
                return chsm_transition(self, s_reset_ptr_reg);
            }
            break;

        default:
            guards_only_b = false;
    }

    if(lm73_timeout(self, e_pst, LM73_RETRY_TIMEOUT))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        lm73_reset_timer(self, e_pst);
        return chsm_transition(self, s_unplugged);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_init, NULL, guards_only_b);
}

static chsm_result_ten s_reset_ptr_reg(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            send_online_event(self, e_pst);
            lm73_start_read(self, e_pst);
            return chsm_transition(self, s_reading);

        case SIG_I2C_RESULT_ADDR_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_inc_error_counter(self, e_pst);
            lm73_reset_pointer(self, e_pst);
            return chsm_transition(self, s_reset_ptr_reg);

        case SIG_I2C_RESULT_DATA_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_inc_error_counter(self, e_pst);
            lm73_reset_pointer(self, e_pst);
            return chsm_transition(self, s_reset_ptr_reg);

        default:
            guards_only_b = false;
    }

    if(lm73_timeout(self, e_pst, LM73_RETRY_TIMEOUT))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        lm73_reset_timer(self, e_pst);
        lm73_reset_pointer(self, e_pst);
        return chsm_transition(self, s_reset_ptr_reg);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_set_config, NULL, guards_only_b);
}

static chsm_result_ten s_online(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
            guards_only_b = false;
    }

    if(lm73_error_count(self, e_pst, LM73_MAX_ERROR_COUNT))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        send_offline_event(self, e_pst);
        lm73_reset_timer(self, e_pst);
        lm73_read_id(self, e_pst);
        return chsm_transition(self, s_read_id_reg);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_lm73, send_offline_event, guards_only_b);
}

static chsm_result_ten s_init(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
            guards_only_b = false;
    }


    return chsm_handle_in_parent(self, ctx_pst, s_lm73, NULL, guards_only_b);
}

static chsm_result_ten s_set_resolution(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_reset_pointer(self, e_pst);
            return chsm_transition(self, s_reset_ptr_reg);

        case SIG_I2C_RESULT_ADDR_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_inc_error_counter(self, e_pst);
            lm73_set_resolution(self, e_pst);
            return chsm_transition(self, s_set_resolution);

        case SIG_I2C_RESULT_DATA_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_inc_error_counter(self, e_pst);
            lm73_set_resolution(self, e_pst);
            return chsm_transition(self, s_set_resolution);

        default:
            guards_only_b = false;
    }

    if(lm73_timeout(self, e_pst, LM73_RETRY_TIMEOUT))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        lm73_reset_timer(self, e_pst);
        lm73_set_resolution(self, e_pst);
        return chsm_transition(self, s_set_resolution);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_set_config, NULL, guards_only_b);
}

static chsm_result_ten s_set_config(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
            guards_only_b = false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_lm73, NULL, guards_only_b);
}

static chsm_result_ten s_get_resolution(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_update_resolution(self, e_pst);
            lm73_set_resolution(self, e_pst);
            return chsm_transition(self, s_set_resolution);

        case SIG_I2C_RESULT_ADDR_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_inc_error_counter(self, e_pst);
            lm73_get_resolution(self, e_pst);
            return chsm_transition(self, s_get_resolution);

        case SIG_I2C_RESULT_DATA_NACK:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_inc_error_counter(self, e_pst);
            lm73_get_resolution(self, e_pst);
            return chsm_transition(self, s_get_resolution);

        default:
            guards_only_b = false;
    }

    if(lm73_timeout(self, e_pst, LM73_RETRY_TIMEOUT))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        lm73_reset_timer(self, e_pst);
        lm73_get_resolution(self, e_pst);
        return chsm_transition(self, s_get_resolution);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_set_config, NULL, guards_only_b);
}

static chsm_result_ten s_unplugged(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
            guards_only_b = false;
    }

    if(lm73_timeout(self, e_pst, LM73_UNPLUGGED_TIMEOUT))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        lm73_reset_timer(self, e_pst);
        lm73_reset_timer(self, e_pst);
        lm73_read_id(self, e_pst);
        return chsm_transition(self, s_read_id_reg);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_init, NULL, guards_only_b);
}

chsm_result_ten lm73_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            lm73_init(self, e_pst);
            lm73_reset_timer(self, e_pst);
            lm73_read_id(self, e_pst);
            return chsm_transition(self, s_read_id_reg);

        default:
            guards_only_b = false;
    }

    return chsm_ignored(self);
}
