#ifndef CHSM_TEST_MACHINE_H
#define CHSM_TEST_MACHINE_H

#include "chsm.h"
#include <stdbool.h>

#define TEST_HSM_LOG_SIZE 1024

enum test_events_en
{
    TEST_SIG1 = 256,
    TEST_SIG2,
    TEST_SIG3,

    TEST_SIG_A,
    TEST_SIG_B,
    TEST_SIG_C,
    TEST_SIG_D,
    TEST_SIG_E,
    TEST_SIG_F,
    TEST_SIG_G,
    TEST_SIG_H,
    TEST_SIG_ID,
    TEST_SIG_J,
    TEST_SIG_K,
    TEST_SIG_L,
    TEST_SIG_M,
    TEST_SIG_N
};

typedef struct test_hsm_tst test_hsm_tst;

struct test_hsm_tst
{
    chsm_tst    super;

    char        log_buff[TEST_HSM_LOG_SIZE];
    char        *log;

    bool        cond;
    bool        k_guard;
    bool        j_guard;
    bool        s11_guard;
    bool        s1_guard;
    bool        s211_i_guard;
    bool        s21_entry_guard;
    bool        s11_g_guard1;
    bool        s11_g_guard2;
    bool        s5_g1;

    chsm_state_tpft history_pft;
};

chsm_result_ten __top__1(chsm_tst *self, const cevent_tst  *e_pst);
chsm_result_ten __top__4(chsm_tst *self, const cevent_tst  *e_pst);

void clear_log(test_hsm_tst  *self);


void s11_func(chsm_tst *self, const cevent_tst *e_pst, uint16_t param);
void s11_g1(chsm_tst *self, const cevent_tst *e_pst, uint16_t param);
void s11_g2(chsm_tst *self, const cevent_tst *e_pst, uint16_t param);
bool k_guard(chsm_tst *self, const cevent_tst *e_pst, uint16_t param);
bool s11_guard(chsm_tst *self, const cevent_tst *e_pst, uint16_t param);
bool s11_g_guard1(chsm_tst *self, const cevent_tst *e_pst, uint16_t param);
bool s11_g_guard2(chsm_tst *self, const cevent_tst *e_pst, uint16_t param);
void s11_id(chsm_tst *self, const cevent_tst *e_pst, uint16_t param);
void s211_exit(chsm_tst *self, const cevent_tst *e_pst, uint16_t param);
bool s5_g1(chsm_tst *self, const cevent_tst *e_pst);
void s5_k_func1(chsm_tst *_self, const cevent_tst *e_pst, uint16_t param1, uint16_t param2);
chsm_state_tpft s_history(chsm_tst *_self, const cevent_tst *e_pst);

#endif
