/*Generated with CHSM v0.0.0 at 2022.11.16 20.52.07*/
#include "cevent.h"
#include "chsm.h"
#include "fram.h"
#include "fram_functions.h"


static chsm_result_ten s_writing(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_reading(chsm_tst *self, const cevent_tst  *e_pst);
static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst);

static chsm_result_ten s_idle(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_MEM_READ:
            store_op_info(self, e_pst);
            read_a_chunk(self, e_pst);
            return chsm_transition(self, s_reading);

        case SIG_MEM_WRITE:
            store_op_info(self, e_pst);
            write_a_chunk(self, e_pst);
            return chsm_transition(self, s_writing);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_reading(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_SUCCESS:
            read_a_chunk(self, e_pst);
            if(last_transaction(self, e_pst))
            {
                send_read_success_response(self, e_pst);
                clear_op_info(self, e_pst);
                chsm_recall(self, e_pst);
                return chsm_transition(self, s_idle);
            }
            break;

        case SIG_I2C_RESULT_ADDR_NACK:
            send_read_fail_response(self, e_pst);
            clear_op_info(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_I2C_RESULT_DATA_NACK:
            send_read_fail_response(self, e_pst);
            clear_op_info(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_MEM_READ:
            chsm_defer(self, e_pst);
            break;

        case SIG_MEM_WRITE:
            chsm_defer(self, e_pst);
            break;
    }

    return chsm_ignored(self);
}

static chsm_result_ten s_writing(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case SIG_I2C_RESULT_SUCCESS:
            write_a_chunk(self, e_pst);
            if(last_transaction(self, e_pst))
            {
                send_write_success_response(self, e_pst);
                clear_op_info(self, e_pst);
                chsm_recall(self, e_pst);
                return chsm_transition(self, s_idle);
            }
            break;

        case SIG_I2C_RESULT_ADDR_NACK:
            send_write_fail_response(self, e_pst);
            clear_op_info(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_I2C_RESULT_DATA_NACK:
            send_write_fail_response(self, e_pst);
            clear_op_info(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);

        case SIG_MEM_READ:
            chsm_defer(self, e_pst);
            break;

        case SIG_MEM_WRITE:
            chsm_defer(self, e_pst);
            break;
    }

    return chsm_ignored(self);
}

chsm_result_ten fram_top(chsm_tst *self, const cevent_tst  *e_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            fram_init(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s_idle);
    }

    return chsm_ignored(self);
}
