#ifndef CHSM_TEST_FUNCTIONS2_H
#define CHSM_TEST_FUNCTIONS2_H

/*Generated with CHSM v0.0.0 at 2022.11.20 06.11.20*/

#include "chsm_test_machine.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>


void d_func(chsm_tst *self, const cevent_tst *e_pst);

/*S11 entry function comment*/
void s11_entry(chsm_tst *self, const cevent_tst *e_pst);

void s11_exit(chsm_tst *self, const cevent_tst *e_pst);

void s11_init(chsm_tst *self, const cevent_tst *e_pst);

/*S1 entry function comment.*/
void s1_entry(chsm_tst *self, const cevent_tst *e_pst);

void s1_exit(chsm_tst *self, const cevent_tst *e_pst);

void s1_func(chsm_tst *self, const cevent_tst *e_pst);

void s1_init(chsm_tst *self, const cevent_tst *e_pst);

void s211_entry(chsm_tst *self, const cevent_tst *e_pst);

void s211_id(chsm_tst *self, const cevent_tst *e_pst);

void s211_init(chsm_tst *self, const cevent_tst *e_pst);

void s21_entry(chsm_tst *self, const cevent_tst *e_pst);

void s21_exit(chsm_tst *self, const cevent_tst *e_pst);

void s21_init(chsm_tst *self, const cevent_tst *e_pst);

void s2_entry(chsm_tst *self, const cevent_tst *e_pst);

void s2_exit(chsm_tst *self, const cevent_tst *e_pst);

void s2_init(chsm_tst *self, const cevent_tst *e_pst);

void s3_k_func(chsm_tst *self, const cevent_tst *e_pst);

void s3_l_func(chsm_tst *self, const cevent_tst *e_pst);

void s4_id(chsm_tst *self, const cevent_tst *e_pst);

void s_entry(chsm_tst *self, const cevent_tst *e_pst);

void s_exit(chsm_tst *self, const cevent_tst *e_pst);

void s_init(chsm_tst *self, const cevent_tst *e_pst);


bool cond(chsm_tst *self, const cevent_tst *e_pst);

bool j_guard(chsm_tst *self, const cevent_tst *e_pst);

bool s1_guard(chsm_tst *self, const cevent_tst *e_pst);

typedef enum chsm_test_machine_state_id_ten
{
    S11 = 2,
    S211 = 5,
    S3 = 6,
    S4 = 7,
} chsm_test_machine_state_id_ten;


/*
Signals:
    A     
    B     
    C     
    D     
    E     
    F     Comment for F
    G     Comment for G
    H     
    ID    
    J     
    K     
    L     
*/

/*
Other function notes:

k_guard:
    Parametric guard comment.

s11_func:

s11_g1:

s11_g2:

s11_g_guard1:
    Comment for the s11_g_guard1 func.
    
    Multiple lines.

s11_g_guard2:

s11_guard:

s11_id:

s211_exit:
*/
#endif
