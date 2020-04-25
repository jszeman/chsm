#ifndef CHSM_TEST_FUNCTIONS2_H
#define CHSM_TEST_FUNCTIONS2_H

/*Generated with CHSM v0.0.0 at 2020.04.24 22.08.16*/

#include "chsm_test_machine.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>

void d_func(test_hsm_tst *self, const cevent_tst *e_pst);
void s11_entry(test_hsm_tst *self, const cevent_tst *e_pst);
void s11_exit(test_hsm_tst *self, const cevent_tst *e_pst);
void s11_func(test_hsm_tst *self, const cevent_tst *e_pst);
void s11_g1(test_hsm_tst *self, const cevent_tst *e_pst);
void s11_g2(test_hsm_tst *self, const cevent_tst *e_pst);
void s11_id(test_hsm_tst *self, const cevent_tst *e_pst);
void s11_init(test_hsm_tst *self, const cevent_tst *e_pst);
void s1_entry(test_hsm_tst *self, const cevent_tst *e_pst);
void s1_exit(test_hsm_tst *self, const cevent_tst *e_pst);
void s1_func(test_hsm_tst *self, const cevent_tst *e_pst);
void s1_init(test_hsm_tst *self, const cevent_tst *e_pst);
void s211_entry(test_hsm_tst *self, const cevent_tst *e_pst);
void s211_exit(test_hsm_tst *self, const cevent_tst *e_pst);
void s211_id(test_hsm_tst *self, const cevent_tst *e_pst);
void s211_init(test_hsm_tst *self, const cevent_tst *e_pst);
void s21_entry(test_hsm_tst *self, const cevent_tst *e_pst);
void s21_exit(test_hsm_tst *self, const cevent_tst *e_pst);
void s21_init(test_hsm_tst *self, const cevent_tst *e_pst);
void s2_entry(test_hsm_tst *self, const cevent_tst *e_pst);
void s2_exit(test_hsm_tst *self, const cevent_tst *e_pst);
void s2_init(test_hsm_tst *self, const cevent_tst *e_pst);
void s_entry(test_hsm_tst *self, const cevent_tst *e_pst);
void s_exit(test_hsm_tst *self, const cevent_tst *e_pst);
void s_init(test_hsm_tst *self, const cevent_tst *e_pst);

bool cond(test_hsm_tst *self, const cevent_tst *e_pst);
bool j_guard(test_hsm_tst *self, const cevent_tst *e_pst);
bool k_guard(test_hsm_tst *self, const cevent_tst *e_pst);
bool s11_g_guard1(test_hsm_tst *self, const cevent_tst *e_pst);
bool s11_g_guard2(test_hsm_tst *self, const cevent_tst *e_pst);
bool s11_guard(test_hsm_tst *self, const cevent_tst *e_pst);
bool s1_guard(test_hsm_tst *self, const cevent_tst *e_pst);

#endif
