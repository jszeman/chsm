/*Generated with CHSM v0.0.0 at 2021.02.06 20.41.58*/
#include "cevent.h"
#include "chsm.h"
#include "canopen.h"
#include "canopen_functions.h"


static chsm_result_ten state_0(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten state_0(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
            guards_only_b = false;
    }


    return chsm_handle_in_parent(self, ctx_pst, co_node_top, NULL, guards_only_b);
}

chsm_result_ten co_node_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            return chsm_transition(self, state_0);

        default:
            guards_only_b = false;
    }

    return chsm_ignored(self);
}
