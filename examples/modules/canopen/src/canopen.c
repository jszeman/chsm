/*Generated with CHSM v0.0.0 at 2021.02.08 21.39.50*/
#include "cevent.h"
#include "chsm.h"
#include "canopen.h"
#include "canopen_functions.h"


static chsm_result_ten s_canopen(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten s_canopen(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {
        case SIG_CAN_FRAME:
            co_process_frame(self, e_pst);
            break;

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
            co_send_bootup(self, e_pst);
            return chsm_transition(self, s_canopen);

        default:
            guards_only_b = false;
    }

    return chsm_ignored(self);
}
