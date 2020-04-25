#ifndef CHSM_TEST_FUNCTIONS_H
#define CHSM_TEST_FUNCTIONS_H

#include "chsm_test_machine.h"
#include "chsm.h"
#include "cevent.h"

void s_entry(test_hsm_tst *self, const cevent_tst *e_pst);
void s_exit(test_hsm_tst *self, const cevent_tst *e_pst);
void s_init(test_hsm_tst *self, const cevent_tst *e_pst);
void s1_entry(test_hsm_tst *self, const cevent_tst *e_pst);
void s1_exit(test_hsm_tst *self, const cevent_tst *e_pst);
void s1_init(test_hsm_tst *self, const cevent_tst *e_pst);
void s2_entry(test_hsm_tst *self, const cevent_tst *e_pst);
void s2_exit(test_hsm_tst *self, const cevent_tst *e_pst);
void s2_init(test_hsm_tst *self, const cevent_tst *e_pst);
void s_sig1_handler(test_hsm_tst *self, const cevent_tst *e_pst);
void s_sig2_handler(test_hsm_tst *self, const cevent_tst *e_pst);
void s_sig3_handler(test_hsm_tst *self, const cevent_tst *e_pst);

#endif
