/*Generated with CHSM v0.0.0 at 2022.12.04 20.29.16*/
#include "chsm_simple_machine.h"
#include "chsm_simple_machine_functions.h"


static void state_b(void *self, uint32_t e_u32);
static void state_a(void *self, uint32_t e_u32);

static void state_a(void *self, uint32_t e_u32)
{
    switch(e_u32)
    {
        case SMPL_SIG_EVENT_A:
            a_func(self, e_u32);
            break;

        case SMPL_SIG_EVENT_H:
            if(guard_h(self, e_u32, 2))
            {
                func_h(self, e_u32, 4, 5);
                a_exit(self, e_u32);
                c_entry(self, e_u32);
                b_entry(self, e_u32);
                return SIMPLE_SM_SET_STATE(self, state_b);
            }
            break;
    }

    if(guard_1(self, e_u32))
    {
        a_exit(self, e_u32);
        c_entry(self, e_u32);
        b_entry(self, e_u32);
        return SIMPLE_SM_SET_STATE(self, state_b);
    }

    return ;
}

static void state_b(void *self, uint32_t e_u32)
{
    switch(e_u32)
    {
        case SMPL_SIG_EVENT_B:
            b_func(self, e_u32);
            break;

        case SMPL_SIG_EVENT_F:
            b_to_a(self, e_u32);
            b_exit(self, e_u32);
            c_exit(self, e_u32);
            a_entry(self, e_u32);
            return SIMPLE_SM_SET_STATE(self, state_a);

        case SMPL_SIG_EVENT_K:
            func_k(self, e_u32);
            b_exit(self, e_u32);
            c_exit(self, e_u32);
            a_entry(self, e_u32);
            return SIMPLE_SM_SET_STATE(self, state_a);
    }

    return ;
}

void simple_machine_callback(void *self, uint32_t e_u32)
{
    switch(e_u32)
    {
        case SMPL_SIG_INIT:
            a_entry(self, e_u32);
            return SIMPLE_SM_SET_STATE(self, state_a);
    }

    return ;
}
