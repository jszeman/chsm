#ifndef BUS_DRIVER_FUNCTIONS_H
#define BUS_DRIVER_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2020.07.26 07.12.40*/

#include "bus_driver.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>

void emit_event(bus_driver_tst *self, const cevent_tst *e_pst);
void send_data(bus_driver_tst *self, const cevent_tst *e_pst);


#endif
