#ifndef BUS_DRIVER_H
#define BUS_DRIVER_H

#include "chsm.h"
#include <stdbool.h>
#include "events.h"

typedef struct
{
    chsm_tst    sm;

    uint16_t    tmp_u16;
} bus_driver_tst;

chsm_result_ten bus_driver_top(chsm_tst *self, const cevent_tst  *e_pst);

#endif