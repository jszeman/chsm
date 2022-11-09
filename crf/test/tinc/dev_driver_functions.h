#ifndef DEV_DRIVER_FUNCTIONS_H
#define DEV_DRIVER_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2022.11.08 20.45.11*/

#include "dev_driver.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>


void cancel_request(chsm_tst *self, const cevent_tst *e_pst);

void dev_drv_init(chsm_tst *self, const cevent_tst *e_pst);

void inc_timer(chsm_tst *self, const cevent_tst *e_pst);

void insert_data(chsm_tst *self, const cevent_tst *e_pst);

void send_read_request(chsm_tst *self, const cevent_tst *e_pst);

void send_write_request(chsm_tst *self, const cevent_tst *e_pst);

void start_timer(chsm_tst *self, const cevent_tst *e_pst);


bool response_complete(chsm_tst *self, const cevent_tst *e_pst);

bool timeout(chsm_tst *self, const cevent_tst *e_pst);

typedef enum dev_driver_state_id_ten
{
    IDLE = 0,
    WAIT_RESPONSE = 1,
} dev_driver_state_id_ten;


/*
Signals:
    CANCEL   
    READ     
    RECEIVE  
    TICK_1MS 
    WRITE    
*/

/*
Other function notes:
*/
#endif
