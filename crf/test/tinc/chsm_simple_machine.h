#ifndef CHSM_SIMPLE_MACHINE_H
#define CHSM_SIMPLE_MACHINE_H

#include <stdbool.h>
#include <stdint.h>

#define SMPL_SIG_INIT    0x55aa
#define SMPL_SIG_EVENT_A 0
#define SMPL_SIG_EVENT_B 1
#define SMPL_SIG_EVENT_F 2
#define SMPL_SIG_EVENT_H 3
#define SMPL_SIG_EVENT_K 4

typedef struct simple_sm_tst
{
    void (*callback)(void* self, uint32_t event_u32);
    char    log_ac[1024];
    char*   log_pc;

    bool    guard_1;
    bool    guard_h;
} simple_sm_tst;

/* Function declarations that the code generator can not guess */
bool guard_h(void *self, uint32_t e_u32, uint32_t param);
bool func_h(void *self, uint32_t e_u32, uint32_t p1_u32, uint16_t p2_u16);

#define SIMPLE_SM_SET_STATE(obj_ptr, func) ((simple_sm_tst *)(obj_ptr))->callback = func

void simple_machine_callback(void *self, uint32_t e_u32);

void smpl_sm_clear_log(simple_sm_tst *self);

#endif