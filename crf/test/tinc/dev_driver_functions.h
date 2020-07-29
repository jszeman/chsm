#ifndef DEV_DRIVER_FUNCTIONS_H
#define DEV_DRIVER_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2020.07.29 06.46.58*/

#include "dev_driver.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>

void cancel_request(dev_driver_tst *self, const cevent_tst *e_pst);
void dev_drv_init(dev_driver_tst *self, const cevent_tst *e_pst);
void inc_timer(dev_driver_tst *self, const cevent_tst *e_pst);
void insert_data(dev_driver_tst *self, const cevent_tst *e_pst);
void send_read_request(dev_driver_tst *self, const cevent_tst *e_pst);
void send_write_request(dev_driver_tst *self, const cevent_tst *e_pst);
void start_timer(dev_driver_tst *self, const cevent_tst *e_pst);

bool response_complete(dev_driver_tst *self, const cevent_tst *e_pst);
bool timeout(dev_driver_tst *self, const cevent_tst *e_pst);

#endif
