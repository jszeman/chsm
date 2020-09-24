/*
 * chsm_test_functions.c
 *
 *  Created on: 2019. febr. 4.
 *      Author: jszeman
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "chsm.h"
#include "cevent.h"
#include "chsm_test_machine.h"

static void load(test_hsm_tst *self, const char *str)
{
    while (*str)
    {
        *self->log = *str;
        self->log++;
        str++;
    }
}

void clear_log(test_hsm_tst  *self)
{
    memset(&self->log_buff, 0, sizeof(self->log_buff));
    self->log = self->log_buff;
}

void s_entry(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s_exit(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s_init(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s1_entry(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s1_exit(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s1_init(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s2_entry(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s2_exit(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s2_init(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s_sig1_handler(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s_sig2_handler(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s_sig3_handler(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void d_func(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s11_entry(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s11_exit(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s11_func(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s11_init(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s1_func(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s211_entry(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s211_exit(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s211_init(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s21_entry(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s21_exit(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s21_init(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}


void s11_id(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s211_id(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s11_g1(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s11_g2(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

bool cond(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
    return self->cond;
}

bool k_guard(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
    return self->k_guard;
}

bool j_guard(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
    return self->j_guard;
}

bool s11_guard(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
    return self->s11_guard;
}

bool s1_guard(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
    return self->s1_guard;
}

bool s211_i_guard(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
    return self->s211_i_guard;
}

bool s21_entry_guard(test_hsm_tst *self, cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
    return self->s21_entry_guard;
}

bool s11_g_guard1(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
    return self->s11_g_guard1;
}

bool s11_g_guard2(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
    return self->s11_g_guard2;
}

void s3_k_func(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void s3_id(test_hsm_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}