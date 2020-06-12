#ifndef DEV_DRIVER_H
#define DEV_DRIVER_H

#include "chsm.h"
#include <stdbool.h>

typedef struct
{
    chsm_tst    sm;
} dev_driver_tst;

chsm_result_ten dev_driver_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

#endif