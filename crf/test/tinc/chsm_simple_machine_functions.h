#ifndef CHSM_SIMPLE_MACHINE_FUNCTIONS_H
#define CHSM_SIMPLE_MACHINE_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2022.12.04 20.29.16*/

#include "chsm_simple_machine.h"
#include <stdbool.h>


void a_entry(void *self, uint32_t e_u32);

void a_exit(void *self, uint32_t e_u32);

void a_func(void *self, uint32_t e_u32);

void b_entry(void *self, uint32_t e_u32);

void b_exit(void *self, uint32_t e_u32);

void b_func(void *self, uint32_t e_u32);

void b_to_a(void *self, uint32_t e_u32);

void c_entry(void *self, uint32_t e_u32);

void c_exit(void *self, uint32_t e_u32);

void func_k(void *self, uint32_t e_u32);


bool guard_1(void *self, uint32_t e_u32);

typedef enum chsm_simple_machine_state_id_ten
{
    STATE_A = 0,
    STATE_B = 1,
} chsm_simple_machine_state_id_ten;


/*
Signals:
    EVENT_A 
    EVENT_B 
    EVENT_F 
    EVENT_H 
    EVENT_K 
*/

/*
Other function notes:

func_h:

guard_h:
*/
#endif
