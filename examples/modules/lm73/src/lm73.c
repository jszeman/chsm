/*Generated with CHSM v0.0.0 at 2020.12.30 06.44.14*/
#include "cevent.h"
#include "chsm.h"
#include "lm73.h"
#include "lm73_functions.h"


static chsm_result_ten s_lm73(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten s_lm73(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
        guards_only_b=false;
    }

    return chsm_handle_in_parent(self, ctx_pst, lm73_top, NULL, guards_only_b);
}

chsm_result_ten lm73_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    bool guards_only_b=true;
    switch(e_pst->sig)
    {

        default:
        guards_only_b=false;
    }

    return chsm_ignored(self);
}
