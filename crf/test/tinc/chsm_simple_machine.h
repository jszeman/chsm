#ifndef CHSM_SIMPLE_MACHINE_H
#define CHSM_SIMPLE_MACHINE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct simple_sm_tst
{
    void (*callback)(void* self, uint32_t event);
} simple_sm_tst;

#define SIMPLE_SM_SET_STATE(chsm_obj_ptr, chsm_func) ((simple_sm_tst *)(chsm_obj_ptr))->callback = chsm_func

void simple_machine_callback(void *chsm_obj, uint32_t chsm_event);

#endif