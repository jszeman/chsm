#include "chsm_simple_machine.h"
#include "chsm_simple_machine_functions.h"
#include <stdint.h>

static void load(simple_sm_tst *self, const char *str)
{
    while (*str)
    {
        *self->log_pc = *str;
        self->log_pc++;
        str++;
    }
}

static void load_param(simple_sm_tst *self, const uint16_t i)
{
    char buff[6];

    *self->log_pc++ = '(';

    sprintf(buff, "%d", i);

    load(self, buff);

    *self->log_pc++ = ')';
    *self->log_pc++ = ' ';

}

void smpl_sm_clear_log(simple_sm_tst *self)
{
    memset(&self->log_ac, 0, sizeof(self->log_ac));
    self->log_pc = self->log_ac;
}


void a_entry(void *self, uint32_t e_u32)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_u32;
}

void a_exit(void *self, uint32_t e_u32)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_u32;
}

void a_func(void *self, uint32_t e_u32)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_u32;
}

void b_entry(void *self, uint32_t e_u32)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_u32;
}

void b_exit(void *self, uint32_t e_u32)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_u32;
}

void b_func(void *self, uint32_t e_u32)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_u32;
}

void b_to_a(void *self, uint32_t e_u32)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_u32;
}

void c_entry(void *self, uint32_t e_u32)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_u32;
}

void c_exit(void *self, uint32_t e_u32)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_u32;
}

void func_k(void *self, uint32_t e_u32)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_u32;
}


bool guard_1(void *self, uint32_t e_u32)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load(self, " ");

    (void)e_u32;

    return _self->guard_1;
}

bool guard_h(void *self, uint32_t e_u32, uint32_t param)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load_param(self, param);

    (void)e_u32;

    return _self->guard_h;
}

bool func_h(void *self, uint32_t e_u32, uint32_t p1_u32, uint16_t p2_u16)
{
    simple_sm_tst *_self = (simple_sm_tst *)self;
    
    load(self, __func__);
    load_param(self, p2_u16);

    (void)e_u32;
}