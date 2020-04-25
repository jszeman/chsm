/*Generated with CHSM v0.0.0 at 2020.04.24 22.08.16*/

#include "cevent.h"
#include "chsm.h"
#include "chsm_test_machine.h"
#include "chsm_test_functions2.h"


static chsm_result_ten s211(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s21(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s2(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s11(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s1(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);
static chsm_result_ten s(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);

static chsm_result_ten s(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_E:
            chsm_exit_children(self, e_pst, ctx_pst);
            s1_entry((test_hsm_tst *)self, e_pst);
            s11_entry((test_hsm_tst *)self, e_pst);
            s11_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s11);

    }

    return chsm_handle_in_parent(self, ctx_pst, __top__4, s_exit);
}

static chsm_result_ten s1(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_C:
            chsm_exit_children(self, e_pst, ctx_pst);
            s1_exit((test_hsm_tst *)self, e_pst);
            s2_entry((test_hsm_tst *)self, e_pst);
            s2_init((test_hsm_tst *)self, e_pst);
            s21_entry((test_hsm_tst *)self, e_pst);
            s21_init((test_hsm_tst *)self, e_pst);
            s211_entry((test_hsm_tst *)self, e_pst);
            s211_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s211);

        case TEST_SIG_F:
            chsm_exit_children(self, e_pst, ctx_pst);
            s1_exit((test_hsm_tst *)self, e_pst);
            s2_entry((test_hsm_tst *)self, e_pst);
            s21_entry((test_hsm_tst *)self, e_pst);
            s211_entry((test_hsm_tst *)self, e_pst);
            s211_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s211);

        case TEST_SIG_B:
            chsm_exit_children(self, e_pst, ctx_pst);
            s11_entry((test_hsm_tst *)self, e_pst);
            s11_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_A:
            chsm_exit_children(self, e_pst, ctx_pst);
            s1_exit((test_hsm_tst *)self, e_pst);
            s1_entry((test_hsm_tst *)self, e_pst);
            s1_init((test_hsm_tst *)self, e_pst);
            s11_entry((test_hsm_tst *)self, e_pst);
            s11_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s11);

    }

    if(s1_guard((test_hsm_tst *)self, e_pst))
    {
        s1_func((test_hsm_tst *)self, e_pst);
        return chsm_handled(self);
    }

    if(j_guard((test_hsm_tst *)self, e_pst))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        s1_exit((test_hsm_tst *)self, e_pst);
        s2_entry((test_hsm_tst *)self, e_pst);
        s2_init((test_hsm_tst *)self, e_pst);
        s21_entry((test_hsm_tst *)self, e_pst);
        s21_init((test_hsm_tst *)self, e_pst);
        s211_entry((test_hsm_tst *)self, e_pst);
        s211_init((test_hsm_tst *)self, e_pst);
        return chsm_transition(self, s211);
    }

    return chsm_handle_in_parent(self, ctx_pst, s, s1_exit);
}

static chsm_result_ten s11(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_ID:
            s11_id((test_hsm_tst *)self, e_pst);
            return chsm_handled(self);

        case TEST_SIG_G:
            if(s11_g_guard1((test_hsm_tst *)self, e_pst))
            {
                s11_g1((test_hsm_tst *)self, e_pst);
            }
            if(s11_g_guard2((test_hsm_tst *)self, e_pst))
            {
                chsm_exit_children(self, e_pst, ctx_pst);
                s11_g2((test_hsm_tst *)self, e_pst);
                s11_exit((test_hsm_tst *)self, e_pst);
                s1_exit((test_hsm_tst *)self, e_pst);
                s2_entry((test_hsm_tst *)self, e_pst);
                s21_entry((test_hsm_tst *)self, e_pst);
                s211_entry((test_hsm_tst *)self, e_pst);
                s211_init((test_hsm_tst *)self, e_pst);
                return chsm_transition(self, s211);
            }
            return chsm_handled(self);

        case TEST_SIG_D:
            if(cond((test_hsm_tst *)self, e_pst))
            {
                chsm_exit_children(self, e_pst, ctx_pst);
                d_func((test_hsm_tst *)self, e_pst);
                s11_exit((test_hsm_tst *)self, e_pst);
                s1_init((test_hsm_tst *)self, e_pst);
                s11_entry((test_hsm_tst *)self, e_pst);
                s11_init((test_hsm_tst *)self, e_pst);
                return chsm_transition(self, s11);
            }
            return chsm_handled(self);

        case TEST_SIG_H:
            chsm_exit_children(self, e_pst, ctx_pst);
            s11_exit((test_hsm_tst *)self, e_pst);
            s1_exit((test_hsm_tst *)self, e_pst);
            s_init((test_hsm_tst *)self, e_pst);
            s1_entry((test_hsm_tst *)self, e_pst);
            s1_init((test_hsm_tst *)self, e_pst);
            s11_entry((test_hsm_tst *)self, e_pst);
            s11_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s11);

    }

    if(s11_guard((test_hsm_tst *)self, e_pst))
    {
        s11_func((test_hsm_tst *)self, e_pst);
        return chsm_handled(self);
    }

    if(k_guard((test_hsm_tst *)self, e_pst))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        s11_exit((test_hsm_tst *)self, e_pst);
        s1_exit((test_hsm_tst *)self, e_pst);
        s2_entry((test_hsm_tst *)self, e_pst);
        s2_init((test_hsm_tst *)self, e_pst);
        s21_entry((test_hsm_tst *)self, e_pst);
        s21_init((test_hsm_tst *)self, e_pst);
        s211_entry((test_hsm_tst *)self, e_pst);
        s211_init((test_hsm_tst *)self, e_pst);
        return chsm_transition(self, s211);
    }

    return chsm_handle_in_parent(self, ctx_pst, s1, s11_exit);
}

static chsm_result_ten s2(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_C:
            chsm_exit_children(self, e_pst, ctx_pst);
            s2_exit((test_hsm_tst *)self, e_pst);
            s1_entry((test_hsm_tst *)self, e_pst);
            s1_init((test_hsm_tst *)self, e_pst);
            s11_entry((test_hsm_tst *)self, e_pst);
            s11_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_F:
            chsm_exit_children(self, e_pst, ctx_pst);
            s2_exit((test_hsm_tst *)self, e_pst);
            s1_entry((test_hsm_tst *)self, e_pst);
            s11_entry((test_hsm_tst *)self, e_pst);
            s11_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s11);

    }

    return chsm_handle_in_parent(self, ctx_pst, s, s2_exit);
}

static chsm_result_ten s21(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_G:
            chsm_exit_children(self, e_pst, ctx_pst);
            s21_exit((test_hsm_tst *)self, e_pst);
            s2_exit((test_hsm_tst *)self, e_pst);
            s1_entry((test_hsm_tst *)self, e_pst);
            s1_init((test_hsm_tst *)self, e_pst);
            s11_entry((test_hsm_tst *)self, e_pst);
            s11_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_B:
            chsm_exit_children(self, e_pst, ctx_pst);
            s211_entry((test_hsm_tst *)self, e_pst);
            s211_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s211);

        case TEST_SIG_A:
            chsm_exit_children(self, e_pst, ctx_pst);
            s21_exit((test_hsm_tst *)self, e_pst);
            s21_entry((test_hsm_tst *)self, e_pst);
            s21_init((test_hsm_tst *)self, e_pst);
            s211_entry((test_hsm_tst *)self, e_pst);
            s211_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s211);

    }

    return chsm_handle_in_parent(self, ctx_pst, s2, s21_exit);
}

static chsm_result_ten s211(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_ID:
            s211_id((test_hsm_tst *)self, e_pst);
            return chsm_handled(self);

        case TEST_SIG_H:
            chsm_exit_children(self, e_pst, ctx_pst);
            s211_exit((test_hsm_tst *)self, e_pst);
            s21_exit((test_hsm_tst *)self, e_pst);
            s2_exit((test_hsm_tst *)self, e_pst);
            s_init((test_hsm_tst *)self, e_pst);
            s1_entry((test_hsm_tst *)self, e_pst);
            s1_init((test_hsm_tst *)self, e_pst);
            s11_entry((test_hsm_tst *)self, e_pst);
            s11_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_D:
            chsm_exit_children(self, e_pst, ctx_pst);
            s211_exit((test_hsm_tst *)self, e_pst);
            s21_init((test_hsm_tst *)self, e_pst);
            s211_entry((test_hsm_tst *)self, e_pst);
            s211_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s211);

    }

    return chsm_handle_in_parent(self, ctx_pst, s21, s211_exit);
}

chsm_result_ten __top__4(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            s_entry((test_hsm_tst *)self, e_pst);
            s_init((test_hsm_tst *)self, e_pst);
            s1_entry((test_hsm_tst *)self, e_pst);
            s1_init((test_hsm_tst *)self, e_pst);
            s11_entry((test_hsm_tst *)self, e_pst);
            s11_init((test_hsm_tst *)self, e_pst);
            return chsm_transition(self, s11);

    }

    return chsm_ignored(self);
}
