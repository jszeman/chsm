#ifndef CHSM_SWITCH_CASE_MACHINE_FUNCTIONS_H
#define CHSM_SWITCH_CASE_MACHINE_FUNCTIONS_H

#include "stdint.h"
#include "stdbool.h"

typedef enum swcm_state_ten
{
    SWCM_STATE_TOP,
    SWCM_STATE_A,
    SWCM_STATE_B,
} swcm_state_ten;

typedef enum swcm_event_ten
{
    SWCM_SIG_NOP,
    SWCM_SIG_INIT,
    SWCM_SIG_EVENT_A, 
    SWCM_SIG_EVENT_B, 
    SWCM_SIG_EVENT_F, 
    SWCM_SIG_EVENT_H, 
    SWCM_SIG_EVENT_K, 
} swcm_event_ten;
 
/* The following functions shall be implemented by the user */

void swc_a_entry(swcm_tst *self, swcm_event_ten event_en);

void swc_a_exit(swcm_tst *self, swcm_event_ten event_en);

void swc_a_func(swcm_tst *self, swcm_event_ten event_en);

void swc_b_entry(swcm_tst *self, swcm_event_ten event_en);

void swc_b_exit(swcm_tst *self, swcm_event_ten event_en);

void swc_b_func(swcm_tst *self, swcm_event_ten event_en);

void swc_b_to_a(swcm_tst *self, swcm_event_ten event_en);

void swc_c_entry(swcm_tst *self, swcm_event_ten event_en);

void swc_c_exit(swcm_tst *self, swcm_event_ten event_en);

void swc_func_k(swcm_tst *self, swcm_event_ten event_en);


bool swc_guard_1(swcm_tst *self, swcm_event_ten event_en);


/*
The following functions shall be declared and implemented by the user:

    swc_func_h


The following guards shall be declared and implemented by the user:

    swc_guard_h

*/

#endif