/*Generated with CHSM v0.0.0 at 2022.11.20 06.11.20*/
#include "cevent.h"
#include "chsm.h"
#include "chsm_test_machine.h"
#include "chsm_test_functions2.h"


static chsm_result_ten s4(chsm_tst *self, const cevent_tst *e_pst);
static chsm_result_ten s3(chsm_tst *self, const cevent_tst *e_pst);
static chsm_result_ten s211(chsm_tst *self, const cevent_tst *e_pst);
static chsm_result_ten s11(chsm_tst *self, const cevent_tst *e_pst);

static chsm_result_ten s11(chsm_tst *self, const cevent_tst *e_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_ID:
            s11_id(self, e_pst, 8);
            break;

        case TEST_SIG_G:
            if(s11_g_guard1(self, e_pst, 1))
            {
                s11_g1(self, e_pst, 2);
            }
            if(s11_g_guard2(self, e_pst, 4))
            {
                s11_g2(self, e_pst, 3);
                s11_exit(self, e_pst);
                s1_exit(self, e_pst);
                s2_entry(self, e_pst);
                s21_entry(self, e_pst);
                s211_entry(self, e_pst);
                s211_init(self, e_pst);
                return chsm_transition(self, s211);
            }
            break;

        case TEST_SIG_K:
            chsm_defer(self, e_pst);
            break;

        case TEST_SIG_L:
            chsm_defer(self, e_pst);
            break;

        case TEST_SIG_D:
            if(cond(self, e_pst))
            {
                d_func(self, e_pst);
                s11_exit(self, e_pst);
                s1_init(self, e_pst);
                s11_entry(self, e_pst);
                s11_init(self, e_pst);
                return chsm_transition(self, s11);
            }
            break;

        case TEST_SIG_H:
            s11_exit(self, e_pst);
            s1_exit(self, e_pst);
            s_init(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_C:
            s11_exit(self, e_pst);
            s1_exit(self, e_pst);
            s2_entry(self, e_pst);
            s2_init(self, e_pst);
            s21_entry(self, e_pst);
            s21_init(self, e_pst);
            s211_entry(self, e_pst);
            s211_init(self, e_pst);
            return chsm_transition(self, s211);

        case TEST_SIG_F:
            s11_exit(self, e_pst);
            s1_exit(self, e_pst);
            s2_entry(self, e_pst);
            s21_entry(self, e_pst);
            s211_entry(self, e_pst);
            s211_init(self, e_pst);
            return chsm_transition(self, s211);

        case TEST_SIG_B:
            s11_exit(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_A:
            s11_exit(self, e_pst);
            s1_exit(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_E:
            s11_exit(self, e_pst);
            s1_exit(self, e_pst);
            s1_entry(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_J:
            s11_exit(self, e_pst);
            s1_exit(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s3);
    }

    if(s11_guard(self, e_pst, 6))
    {
        s11_func(self, e_pst, 7);
    }

    if(k_guard(self, e_pst, 5))
    {
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

    if(s1_guard(self, e_pst))
    {
        s1_func(self, e_pst);
    }

    if(j_guard(self, e_pst))
    {
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

    return chsm_ignored(self);
}

static chsm_result_ten s211(chsm_tst *self, const cevent_tst *e_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_ID:
            s211_id(self, e_pst);
            break;

        case TEST_SIG_H:
            s211_exit(self, e_pst, 6);
            s21_exit(self, e_pst);
            s2_exit(self, e_pst);
            s_init(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_D:
            s211_exit(self, e_pst, 6);
            s21_init(self, e_pst);
            s211_entry(self, e_pst);
            s211_init(self, e_pst);
            return chsm_transition(self, s211);

        case TEST_SIG_G:
            s211_exit(self, e_pst, 6);
            s21_exit(self, e_pst);
            s2_exit(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_B:
            s211_exit(self, e_pst, 6);
            s211_entry(self, e_pst);
            s211_init(self, e_pst);
            return chsm_transition(self, s211);

        case TEST_SIG_A:
            s211_exit(self, e_pst, 6);
            s21_exit(self, e_pst);
            s21_entry(self, e_pst);
            s21_init(self, e_pst);
            s211_entry(self, e_pst);
            s211_init(self, e_pst);
            return chsm_transition(self, s211);

        case TEST_SIG_C:
            s211_exit(self, e_pst, 6);
            s21_exit(self, e_pst);
            s2_exit(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_F:
            s211_exit(self, e_pst, 6);
            s21_exit(self, e_pst);
            s2_exit(self, e_pst);
            s1_entry(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_E:
            s211_exit(self, e_pst, 6);
            s21_exit(self, e_pst);
            s2_exit(self, e_pst);
            s1_entry(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_J:
            s211_exit(self, e_pst, 6);
            s21_exit(self, e_pst);
            s2_exit(self, e_pst);
            chsm_recall(self, e_pst);
            return chsm_transition(self, s3);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s3(chsm_tst *self, const cevent_tst *e_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_K:
            s3_k_func(self, e_pst);
            break;

        case TEST_SIG_L:
            s3_l_func(self, e_pst);
            break;

        case TEST_SIG_J:
            s_init(self, e_pst);
            s1_entry(self, e_pst);
            s1_init(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_G:
            if(s11_g_guard2(self, e_pst, 4))
            {
                s11_g2(self, e_pst, 3);
                return chsm_transition(self, s4);
            }
            if(s11_g_guard1(self, e_pst, 1))
            {
                s11_g1(self, e_pst, 2);
                return chsm_transition(self, s4);
            }
            break;

        case TEST_SIG_E:
            s1_entry(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);
    }

    return chsm_ignored(self);
}

static chsm_result_ten s4(chsm_tst *self, const cevent_tst *e_pst)
{
    switch(e_pst->sig)
    {
        case TEST_SIG_ID:
            s4_id(self, e_pst);
            break;

        case TEST_SIG_E:
            s1_entry(self, e_pst);
            s11_entry(self, e_pst);
            s11_init(self, e_pst);
            return chsm_transition(self, s11);

        case TEST_SIG_J:
            chsm_recall(self, e_pst);
            return chsm_transition(self, s3);
    }

    return chsm_ignored(self);
}

chsm_result_ten __top__4(chsm_tst *self, const cevent_tst *e_pst)
{
    switch(e_pst->sig)
    {
        case C_SIG_INIT:
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
