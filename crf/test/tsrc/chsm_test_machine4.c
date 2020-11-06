/*Generated with CHSM v0.0.0 at 2020.11.05 09.13.59*/
#include "cevent.h"
#include "chsm.h"
#include "chsm_test_machine.h"
#include "chsm_test_functions2.h"


static chsm_result_ten s3(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);
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
            s1_entry(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_J:
            chsm_exit_children(self, e_pst, ctx_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s3);

    }

    return chsm_handle_in_parent(self, ctx_pst, __top__4, s_exit);
}

static chsm_result_ten s1(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_C:
            chsm_exit_children(self, e_pst, ctx_pst);
            s1_exit(self, e_pst);
            s2_entry(self, e_pst);
            s2_init(self, e_pst);
            s21_entry(self, e_pst);
            s21_init(self, e_pst);
            s211_entry(self, e_pst);
            s211_init(self, e_pst);
            return chsm_transition(self, s211);

        case TEST_SIG_F:
            chsm_exit_children(self, e_pst, ctx_pst);
            s1_exit(self, e_pst);
            s2_entry(self, e_pst);
            s21_entry(self, e_pst);
            s211_entry(self, e_pst);
            s211_init(self, e_pst);
            return chsm_transition(self, s211);

        case TEST_SIG_B:
            chsm_exit_children(self, e_pst, ctx_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_A:
            chsm_exit_children(self, e_pst, ctx_pst);
            s1_exit(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

    }

    if(s1_guard(self, e_pst))
    {
        s1_func(self, e_pst);
        return chsm_handled(self);
    }

    if(j_guard(self, e_pst))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        s1_exit(self, e_pst);
        s2_entry(self, e_pst);
        s2_init(self, e_pst);
        s21_entry(self, e_pst);
        s21_init(self, e_pst);
        s211_entry(self, e_pst);
        s211_init(self, e_pst);
        return chsm_transition(self, s211);
    }

    return chsm_handle_in_parent(self, ctx_pst, s, s1_exit);
}

static chsm_result_ten s11(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_ID:
            s11_id(self, e_pst, 8);
            return chsm_handled(self);

        case TEST_SIG_G:
            if(s11_g_guard1(self, e_pst, 1))
            {
                s11_g1(self, e_pst, 2);
            }
            if(s11_g_guard2(self, e_pst, 4))
            {
                chsm_exit_children(self, e_pst, ctx_pst);
                s11_g2(self, e_pst, 3);
                s11_exit(self, e_pst);
                s1_exit(self, e_pst);
                s2_entry(self, e_pst);
                s21_entry(self, e_pst);
                s211_entry(self, e_pst);
                s211_init(self, e_pst);
                return chsm_transition(self, s211);
            }
            return chsm_handled(self);

        case TEST_SIG_K:
            chsm_defer(self, e_pst);
            return chsm_handled(self);

        case TEST_SIG_L:
            chsm_defer(self, e_pst);
            return chsm_handled(self);

        case TEST_SIG_D:
            if(cond(self, e_pst))
            {
                chsm_exit_children(self, e_pst, ctx_pst);
                d_func(self, e_pst);
                s11_exit(self, e_pst);
                s1_init(self, e_pst);
                s11_entry(self, e_pst);
                s11_init(self, e_pst);
                return chsm_transition(self, s11);
            }
            return chsm_handled(self);

        case TEST_SIG_H:
            chsm_exit_children(self, e_pst, ctx_pst);
            s11_exit(self, e_pst);
            s1_exit(self, e_pst);
            s_init(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

    }

    if(s11_guard(self, e_pst, 6))
    {
        s11_func(self, e_pst, 7);
        return chsm_handled(self);
    }

    if(k_guard(self, e_pst, 5))
    {
        chsm_exit_children(self, e_pst, ctx_pst);
        s11_exit(self, e_pst);
        s1_exit(self, e_pst);
        s2_entry(self, e_pst);
        s2_init(self, e_pst);
        s21_entry(self, e_pst);
        s21_init(self, e_pst);
        s211_entry(self, e_pst);
        s211_init(self, e_pst);
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
            s2_exit(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_F:
            chsm_exit_children(self, e_pst, ctx_pst);
            s2_exit(self, e_pst);
            s1_entry(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
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
            s21_exit(self, e_pst);
            s2_exit(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_B:
            chsm_exit_children(self, e_pst, ctx_pst);
            s211_entry(self, e_pst);
            s211_init(self, e_pst);
            return chsm_transition(self, s211);

        case TEST_SIG_A:
            chsm_exit_children(self, e_pst, ctx_pst);
            s21_exit(self, e_pst);
            s21_entry(self, e_pst);
            s21_init(self, e_pst);
            s211_entry(self, e_pst);
            s211_init(self, e_pst);
            return chsm_transition(self, s211);

    }

    return chsm_handle_in_parent(self, ctx_pst, s2, s21_exit);
}

static chsm_result_ten s211(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_ID:
            s211_id(self, e_pst);
            return chsm_handled(self);

        case TEST_SIG_H:
            chsm_exit_children(self, e_pst, ctx_pst);
            s211_exit(self, e_pst);
            s21_exit(self, e_pst);
            s2_exit(self, e_pst);
            s_init(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_D:
            chsm_exit_children(self, e_pst, ctx_pst);
            s211_exit(self, e_pst);
            s21_init(self, e_pst);
            s211_entry(self, e_pst);
            s211_init(self, e_pst);
            return chsm_transition(self, s211);

    }

    return chsm_handle_in_parent(self, ctx_pst, s21, s211_exit);
}

static chsm_result_ten s3(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_K:
            s3_k_func(self, e_pst);
            return chsm_handled(self);

        case TEST_SIG_L:
            s3_l_func(self, e_pst);
            return chsm_handled(self);

        case TEST_SIG_J:
            chsm_exit_children(self, e_pst, ctx_pst);
            s_init(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

    }

    return chsm_handle_in_parent(self, ctx_pst, s, NULL);
}

chsm_result_ten __top__4(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
            chsm_exit_children(self, e_pst, ctx_pst);
            s_entry(self, e_pst);
            s_init(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

    }

    return chsm_ignored(self);
}
