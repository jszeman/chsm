#ifndef CHSM_SWITCH_CASE_MACHINE_H
#define CHSM_SWITCH_CASE_MACHINE_H

#include "chsm_switch_case_machine_functions.h"

struct swcm_tst
{
    swcm_state_ten state_en;
    char    log_ac[1024];
    char*   log_pc;

    bool    guard_1;
    bool    guard_h;
};

/* Function declarations that the code generator can't guess */
bool swc_guard_h(swcm_tst *self, swcm_event_ten event_en, uint32_t param);
void swc_func_h(swcm_tst *self, swcm_event_ten event_en, uint32_t p1_u32, uint16_t p2_u16);

void swc_machine(swcm_tst *self, swcm_event_ten event_en);

void swc_sm_clear_log(swcm_tst *self);

#endif