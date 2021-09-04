#include "dacx0501_functions.h"
#include "dacx0501.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

const cevent_tst dacx0501_error_event_st = {.sig = SIG_DACX0501_ERROR};

void dacx0501_1ms_callack(chsm_tst *_self, const cevent_tst *e_pst)
{
    dacx0501_tst* self = (dacx0501_tst *)_self;

    self->timer_cnt_u32++;
}




