/*Generated with CHSM v0.0.0 at 2021.02.18 21.51.37*/
#include "cevent.h"
#include "chsm.h"
#include "can_if.h"
#include "sdo.h"
#include "sdo_functions.h"


static chsm_result_ten s_sdo_segmented_ul(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_busy(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_sdo(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_sdo_segmented_dl(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_sdo_wait_exp_dl(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_sdo_wait_exp_ul(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_CAN_FRAME:
            process_sdo_request(self, e_pst);
            break;

        case SIG_CANOPEN_WAIT_EXP_SDO_UL:
            chsm_exit_children(self, e_pst, ctx_pst);
            return chsm_transition(self, s_sdo_wait_exp_ul);

        case SIG_CANOPEN_WAIT_EXP_SDO_DL:
            chsm_exit_children(self, e_pst, ctx_pst);
            return chsm_transition(self, s_sdo_wait_exp_dl);

        case SIG_CANOPEN_SEG_DL_START:
            chsm_exit_children(self, e_pst, ctx_pst);
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_segmented_dl);

        case SIG_CANOPEN_SEG_UL_START:
            chsm_exit_children(self, e_pst, ctx_pst);
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_segmented_ul);

        default:
            guards_only_b = false;
    }

    return chsm_handle_in_parent(self, ctx_pst, s_sdo, NULL, guards_only_b);
}

static chsm_result_ten s_sdo_wait_exp_ul(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_MEM_READ_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            send_sdo_exp_ul_response(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_MEM_READ_FAIL:
            chsm_exit_children(self, e_pst, ctx_pst);
            send_sdo_exp_ul_abort(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
            guards_only_b = false;
    }


    return chsm_handle_in_parent(self, ctx_pst, s_busy, NULL, guards_only_b);
}

static chsm_result_ten s_sdo_wait_exp_dl(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_MEM_READ_SUCCESS:
            chsm_exit_children(self, e_pst, ctx_pst);
            send_sdo_exp_dl_response(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_MEM_READ_FAIL:
            chsm_exit_children(self, e_pst, ctx_pst);
            send_sdo_exp_dl_abort(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
            guards_only_b = false;
    }


    return chsm_handle_in_parent(self, ctx_pst, s_busy, NULL, guards_only_b);
}

static chsm_result_ten s_sdo_segmented_dl(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_CANOPEN_SEG_DL_END:
            chsm_exit_children(self, e_pst, ctx_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_CAN_FRAME:
            chsm_exit_children(self, e_pst, ctx_pst);
            process_dl_segment(self, e_pst);
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_segmented_dl);

        default:
            guards_only_b = false;
    }

    if(sdo_timeout(self, e_pst, SDO_TIMEOUT))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        send_sdo_abort(self, e_pst, CO_SDO_ABORT_TIMEOUT);
        chsm_recall(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_sdo, NULL, guards_only_b);
}

static chsm_result_ten s_sdo(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_CAN_FRAME:
            process_sdo_request(self, e_pst);
            break;

        case SIG_SYS_TICK_1ms:
            sdo_callback(self, e_pst);
            break;

        default:
            guards_only_b = false;
    }

    return chsm_handle_in_parent(self, ctx_pst, sdo_top, NULL, guards_only_b);
}

static chsm_result_ten s_busy(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_CAN_FRAME:
            if(is_not_abort_request(self, e_pst))
            {
                chsm_defer(self, e_pst);
            }
            if(is_abort_request(self, e_pst))
            {
                chsm_exit_children(self, e_pst, ctx_pst);
                chsm_recall(self, e_pst);
                return chsm_transition(self, s_idle);
            }
            break;

        default:
            guards_only_b = false;
    }

    if(sdo_timeout(self, e_pst, SDO_TIMEOUT))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        send_sdo_abort(self, e_pst, CO_SDO_ABORT_TIMEOUT);
        chsm_recall(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_sdo, NULL, guards_only_b);
}

static chsm_result_ten s_sdo_segmented_ul(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_CAN_FRAME:
            chsm_exit_children(self, e_pst, ctx_pst);
            process_ul_segment(self, e_pst);
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_segmented_ul);

        case SIG_CANOPEN_SEG_UL_END:
            chsm_exit_children(self, e_pst, ctx_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
            guards_only_b = false;
    }

    if(sdo_timeout(self, e_pst, SDO_TIMEOUT))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        send_sdo_abort(self, e_pst, CO_SDO_ABORT_TIMEOUT);
        chsm_recall(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_handle_in_parent(self, ctx_pst, s_sdo, NULL, guards_only_b);
}

chsm_result_ten sdo_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            sdo_init(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        default:
            guards_only_b = false;
    }

    return chsm_ignored(self);
}
