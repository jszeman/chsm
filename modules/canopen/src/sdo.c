/*Generated with CHSM v0.0.0 at 2022.11.22 13.22.23*/
#include "cevent.h"
#include "chsm.h"
#include "sdo.h"
#include "sdo_functions.h"


static chsm_result_ten s_sdo_block_dl_finish(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_sdo_block_dl_started(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_sdo_block_ul_finish(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_sdo_block_ul_started(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_sdo_block_ul_wait_for_start(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_sdo_segmented_ul(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_sdo_segmented_dl(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_sdo_wait_exp_dl(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_sdo_wait_exp_ul(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst);

static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_CAN_FRAME:
            process_sdo_request(self, e_pst);
            break;

        case SIG_CANOPEN_WAIT_EXP_SDO_UL:
            return chsm_transition(self, s_sdo_wait_exp_ul);

        case SIG_CANOPEN_WAIT_EXP_SDO_DL:
            return chsm_transition(self, s_sdo_wait_exp_dl);

        case SIG_CANOPEN_SEG_DL_START:
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_segmented_dl);

        case SIG_CANOPEN_SEG_UL_START:
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_segmented_ul);

        case SIG_CANOPEN_BLOCK_UL_START:
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_block_ul_wait_for_start);

        case SIG_CANOPEN_BLOCK_DL_START:
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_block_dl_started);

        case SIG_SYS_TICK_1ms:
            sdo_callback(self, e_pst);
            break;
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_sdo_wait_exp_ul(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_MEM_READ_SUCCESS:
            send_sdo_exp_ul_response(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_MEM_READ_FAIL:
            send_sdo_exp_ul_abort(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_CAN_FRAME:
            if(is_not_abort_request(self, e_pst))
            {
                chsm_defer(self, e_pst);
            }
            if(is_abort_request(self, e_pst))
            {
                chsm_recall(self, e_pst);
                return chsm_transition(self, s_idle);
            }
            break;

        case SIG_SYS_TICK_1ms:
            sdo_callback(self, e_pst);
            break;
    }

    if(sdo_timeout(self, e_pst, SDO_TIMEOUT))
    {
        send_sdo_abort(self, e_pst, CO_SDO_ABORT_TIMEOUT);
        chsm_recall(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_sdo_wait_exp_dl(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_MEM_READ_SUCCESS:
            send_sdo_exp_dl_response(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_MEM_READ_FAIL:
            send_sdo_exp_dl_abort(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_CAN_FRAME:
            if(is_not_abort_request(self, e_pst))
            {
                chsm_defer(self, e_pst);
            }
            if(is_abort_request(self, e_pst))
            {
                chsm_recall(self, e_pst);
                return chsm_transition(self, s_idle);
            }
            break;

        case SIG_SYS_TICK_1ms:
            sdo_callback(self, e_pst);
            break;
    }

    if(sdo_timeout(self, e_pst, SDO_TIMEOUT))
    {
        send_sdo_abort(self, e_pst, CO_SDO_ABORT_TIMEOUT);
        chsm_recall(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_sdo_segmented_dl(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_CANOPEN_SEG_DL_END:
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_CAN_FRAME:
            process_dl_segment(self, e_pst);
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_segmented_dl);

        case SIG_SYS_TICK_1ms:
            sdo_callback(self, e_pst);
            break;
    }

    if(sdo_timeout(self, e_pst, SDO_TIMEOUT))
    {
        send_sdo_abort(self, e_pst, CO_SDO_ABORT_TIMEOUT);
        chsm_recall(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_sdo_segmented_ul(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_CAN_FRAME:
            process_ul_segment(self, e_pst);
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_segmented_ul);

        case SIG_CANOPEN_SEG_UL_END:
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_SYS_TICK_1ms:
            sdo_callback(self, e_pst);
            break;
    }

    if(sdo_timeout(self, e_pst, SDO_TIMEOUT))
    {
        send_sdo_abort(self, e_pst, CO_SDO_ABORT_TIMEOUT);
        chsm_recall(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_sdo_block_ul_wait_for_start(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_CAN_FRAME:
            process_ul_block_start(self, e_pst);
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_block_ul_wait_for_start);

        case SIG_CANOPEN_BLOCK_UL_END:
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_CANOPEN_BLOCK_UL_STARTED:
            send_next_block(self, e_pst);
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_block_ul_started);

        case SIG_SYS_TICK_1ms:
            sdo_callback(self, e_pst);
            break;
    }

    if(sdo_timeout(self, e_pst, SDO_TIMEOUT))
    {
        send_sdo_abort(self, e_pst, CO_SDO_ABORT_TIMEOUT);
        chsm_recall(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_sdo_block_ul_started(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_CANOPEN_BLOCK_UL_END:
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_CAN_FRAME:
            if(process_ul_block_ack(self, e_pst))
            {
                send_next_block(self, e_pst);
                sdo_reset_timer(self, e_pst);
                return chsm_transition(self, s_sdo_block_ul_started);
            }
            break;

        case SIG_CANOPEN_BLOCK_UL_FINISH:
            send_block_finish(self, e_pst);
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_block_ul_finish);

        case SIG_SYS_TICK_1ms:
            sdo_callback(self, e_pst);
            break;
    }

    if(sdo_timeout(self, e_pst, SDO_TIMEOUT))
    {
        send_sdo_abort(self, e_pst, CO_SDO_ABORT_TIMEOUT);
        chsm_recall(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_sdo_block_ul_finish(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_CAN_FRAME:
            process_block_finish_response(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_SYS_TICK_1ms:
            sdo_callback(self, e_pst);
            break;
    }

    if(sdo_timeout(self, e_pst, SDO_TIMEOUT))
    {
        send_sdo_abort(self, e_pst, CO_SDO_ABORT_TIMEOUT);
        chsm_recall(self, e_pst);
        return chsm_transition(self, s_idle);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_sdo_block_dl_started(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_CAN_FRAME:
            process_dl_subblock(self, e_pst);
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_block_dl_started);

        case SIG_CANOPEN_BLOCK_DL_FINISH:
            sdo_reset_timer(self, e_pst);
            return chsm_transition(self, s_sdo_block_dl_finish);

        case SIG_SYS_TICK_1ms:
            sdo_callback(self, e_pst);
            break;
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_sdo_block_dl_finish(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_CAN_FRAME:
            process_block_dl_end(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_SYS_TICK_1ms:
            sdo_callback(self, e_pst);
            break;
    }

    return chsm_ignored(self);
}

chsm_result_ten sdo_top(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            sdo_init(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);
    }

    return chsm_ignored(self);
}
