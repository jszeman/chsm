#ifndef BUS_DRIVER_FUNCTIONS_H
#define BUS_DRIVER_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2022.11.08 20.45.19*/

#include "bus_driver.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>


void emit_event(chsm_tst *self, const cevent_tst *e_pst);

void send_data(chsm_tst *self, const cevent_tst *e_pst);


typedef enum bus_driver_state_id_ten
{
    IDLE = 0,
    WAITING = 1,
} bus_driver_state_id_ten;


/*
Signals:
    SEND_DATA  
    START_WAIT 
    STOP_WAIT  
    TICK_1MS   
*/

/*
Other function notes:
*/
#endif
