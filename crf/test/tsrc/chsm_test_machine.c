
#include "chsm_test_machine.h"
#include "chsm.h"
#include "cevent.h"

#include "chsm_test_functions.h"

#include <stdio.h>

/*
 * State machine 1
 */

static chsm_result_ten s(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

chsm_result_ten __top__1(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            s_entry((test_hsm_tst *)self, e_pst);
            s_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG1:
            s_sig1_handler((test_hsm_tst *)self, e_pst);
            return chsm_handled(self);

        default:
            break;
    }

    return chsm_handle_in_parent(self, ctx_pst, __top__1, NULL, false);
}
