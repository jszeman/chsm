#ifndef CHSM_TEST_FUNCTIONS2_H
#define CHSM_TEST_FUNCTIONS2_H

#include "chsm.h"
#include "cevent.h"
#include "chsm_test_machine.h"
#include "stdbool.h"

/* The following functions shall be implemented by the user */

void s5_entry1 (chsm_tst *self, const cevent_tst *e_pst);

void s5_entry2 (chsm_tst *self, const cevent_tst *e_pst);

void s1_init (chsm_tst *self, const cevent_tst *e_pst);

void s11_init (chsm_tst *self, const cevent_tst *e_pst);

void s5_g1_f1 (chsm_tst *self, const cevent_tst *e_pst);

void s11_exit (chsm_tst *self, const cevent_tst *e_pst);

void s5_c_func1 (chsm_tst *self, const cevent_tst *e_pst);

void s211_id (chsm_tst *self, const cevent_tst *e_pst);

void s2_init (chsm_tst *self, const cevent_tst *e_pst);

void s5_k_func2 (chsm_tst *self, const cevent_tst *e_pst);

void s6_entry (chsm_tst *self, const cevent_tst *e_pst);

void s2_entry (chsm_tst *self, const cevent_tst *e_pst);

void s_init (chsm_tst *self, const cevent_tst *e_pst);

void s21_exit (chsm_tst *self, const cevent_tst *e_pst);

void s1_func (chsm_tst *self, const cevent_tst *e_pst);

void s_entry (chsm_tst *self, const cevent_tst *e_pst);

void s3_k_func (chsm_tst *self, const cevent_tst *e_pst);

void s211_init (chsm_tst *self, const cevent_tst *e_pst);

void s2_exit (chsm_tst *self, const cevent_tst *e_pst);

void d_func (chsm_tst *self, const cevent_tst *e_pst);

void s5_c_cfunc2 (chsm_tst *self, const cevent_tst *e_pst);

/*
S11 entry function comment
*/
void s11_entry (chsm_tst *self, const cevent_tst *e_pst);

void s3_l_func (chsm_tst *self, const cevent_tst *e_pst);

void s5_l_func (chsm_tst *self, const cevent_tst *e_pst);

void s21_entry (chsm_tst *self, const cevent_tst *e_pst);

void s_exit (chsm_tst *self, const cevent_tst *e_pst);

void s21_init (chsm_tst *self, const cevent_tst *e_pst);

void s_save_history (chsm_tst *self, const cevent_tst *e_pst);

/*
S1 entry function comment.
*/
void s1_entry (chsm_tst *self, const cevent_tst *e_pst);

void s211_entry (chsm_tst *self, const cevent_tst *e_pst);

void s1_exit (chsm_tst *self, const cevent_tst *e_pst);

void s4_id (chsm_tst *self, const cevent_tst *e_pst);

void s5_g1_f2 (chsm_tst *self, const cevent_tst *e_pst);


bool j_guard (chsm_tst *self, const cevent_tst *e_pst);

bool s5_g1 (chsm_tst *self, const cevent_tst *e_pst);

bool s1_guard (chsm_tst *self, const cevent_tst *e_pst);

bool cond (chsm_tst *self, const cevent_tst *e_pst);


typedef enum chsm_test_machine_state_id_ten
{
    S11 = 2,
    S211 = 5,
    S3 = 6,
    S4 = 7,
    S5 = 8,
    S6 = 10,
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
    M 
    N 

The following functions shall be declared and implemented by the user:

    s11_g_guard1
        Comment for the s11_g_guard1 func.

        Multiple lines.

    s11_guard

    k_guard
        Parametric guard comment.

    s11_g1

    s11_func

    s11_g_guard2

    s11_g2

    s11_id

    s211_exit

    s5_k_func1

*/

#endif