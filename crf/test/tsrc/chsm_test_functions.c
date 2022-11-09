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

static void load_param(test_hsm_tst *self, const uint16_t i)
{
    char buff[6];

    *self->log++ = '(';

    sprintf(buff, "%d", i);

    load(self, buff);

    *self->log++ = ')';
    *self->log++ = ' ';
}


void clear_log(test_hsm_tst *self)
{
    memset(&self->log_buff, 0, sizeof(self->log_buff));
    self->log = self->log_buff;
}

void s_entry(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s_exit(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s1_entry(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s1_exit(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s1_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s2_entry(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s2_exit(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s2_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s_sig1_handler(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s_sig2_handler(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s_sig3_handler(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void d_func(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s11_entry(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s11_exit(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s11_func(chsm_tst *_self, const cevent_tst *e_pst, uint16_t param)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
    (void)param;
}

void s11_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s1_func(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s211_entry(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s211_exit(chsm_tst *_self, const cevent_tst *e_pst, uint16_t param)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load_param(self, param);

    (void)e_pst;
}

void s211_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s21_entry(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s21_exit(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s21_init(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}


void s11_id(chsm_tst *_self, const cevent_tst *e_pst, uint16_t param)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
    (void)param;
}

void s211_id(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}


void s4_id(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s11_g1(chsm_tst *_self, const cevent_tst *e_pst, uint16_t param)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
    (void)param;
}

void s11_g2(chsm_tst *_self, const cevent_tst *e_pst, uint16_t param)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
    (void)param;
}

bool cond(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");
    return self->cond;

    (void)e_pst;
}

bool k_guard(chsm_tst *_self, const cevent_tst *e_pst, uint16_t param)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");
    return self->k_guard;

    (void)e_pst;
    (void)param;
}

bool j_guard(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");
    return self->j_guard;

    (void)e_pst;
}

bool s11_guard(chsm_tst *_self, const cevent_tst *e_pst, uint16_t param)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");
    return self->s11_guard;

    (void)e_pst;
    (void)param;
}

bool s1_guard(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");
    return self->s1_guard;

    (void)e_pst;
}

bool s211_i_guard(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");
    return self->s211_i_guard;

    (void)e_pst;
}

bool s21_entry_guard(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");
    return self->s21_entry_guard;

    (void)e_pst;
}

bool s11_g_guard1(chsm_tst *_self, const cevent_tst *e_pst, uint16_t param)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");
    return self->s11_g_guard1;

    (void)e_pst;
    (void)param;
}

bool s11_g_guard2(chsm_tst *_self, const cevent_tst *e_pst, uint16_t param)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");
    return self->s11_g_guard2;

    (void)e_pst;
    (void)param;
}

void s3_k_func(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}

void s3_l_func(chsm_tst *_self, const cevent_tst *e_pst)
{
    test_hsm_tst *self = (test_hsm_tst *)_self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_pst;
}