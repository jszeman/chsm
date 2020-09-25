#ifndef CHSM_TEST_FUNCTIONS2_H
#define CHSM_TEST_FUNCTIONS2_H

/*Generated with CHSM v0.0.0 at 2020.09.25 05.47.46*/

#include "chsm_test_machine.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>

void d_func(chsm_tst *self, const cevent_tst *e_pst);
void s11_entry(chsm_tst *self, const cevent_tst *e_pst);
void s11_exit(chsm_tst *self, const cevent_tst *e_pst);
void s11_func(chsm_tst *self, const cevent_tst *e_pst);
void s11_g1(chsm_tst *self, const cevent_tst *e_pst);
void s11_g2(chsm_tst *self, const cevent_tst *e_pst);
void s11_id(chsm_tst *self, const cevent_tst *e_pst);
void s11_init(chsm_tst *self, const cevent_tst *e_pst);
void s1_entry(chsm_tst *self, const cevent_tst *e_pst);
void s1_exit(chsm_tst *self, const cevent_tst *e_pst);
void s1_func(chsm_tst *self, const cevent_tst *e_pst);
void s1_init(chsm_tst *self, const cevent_tst *e_pst);
void s211_entry(chsm_tst *self, const cevent_tst *e_pst);
void s211_exit(chsm_tst *self, const cevent_tst *e_pst);
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
void s_entry(chsm_tst *self, const cevent_tst *e_pst);
void s_exit(chsm_tst *self, const cevent_tst *e_pst);
void s_init(chsm_tst *self, const cevent_tst *e_pst);

bool cond(chsm_tst *self, const cevent_tst *e_pst);
bool j_guard(chsm_tst *self, const cevent_tst *e_pst);
bool k_guard(chsm_tst *self, const cevent_tst *e_pst);
bool s11_g_guard1(chsm_tst *self, const cevent_tst *e_pst);
bool s11_g_guard2(chsm_tst *self, const cevent_tst *e_pst);
bool s11_guard(chsm_tst *self, const cevent_tst *e_pst);
bool s1_guard(chsm_tst *self, const cevent_tst *e_pst);

#endif
