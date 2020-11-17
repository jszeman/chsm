
#include "chsm_test_machine.h"
#include "chsm.h"
#include "cevent.h"

#include "chsm_test_functions.h"

#include <stdio.h>

/*
 * State machine 2
 */

static chsm_result_ten s(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s1(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

chsm_result_ten __top__2(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    //printf("%d -> %s\n", e_pst->sig, __func__);
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            s_entry((test_hsm_tst *)self, e_pst);
            s_init((test_hsm_tst *)self, e_pst);
            s1_entry((test_hsm_tst *)self, e_pst);
            s1_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s1);
    }

    return chsm_ignored(self);
}

chsm_result_ten s(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    //printf("%d -> %s\n", e_pst->sig, __func__);
    switch(e_pst->sig)
    {
        case TEST_SIG1:
            s_sig1_handler((test_hsm_tst *)self, e_pst);
            return chsm_handled(self);

        default:
            break;
    }

    return chsm_handle_in_parent(self, ctx_pst, __top__2, NULL, false);
}

chsm_result_ten s1(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    //printf("%d -> %s\n", e_pst->sig, __func__);
    switch(e_pst->sig)
    {
        case TEST_SIG2:
            s_sig2_handler((test_hsm_tst *)self, e_pst);
            return chsm_handled(self);

        default:
            break;
    }

    return chsm_handle_in_parent(self, ctx_pst, s, NULL, false);
}
