#ifndef CANOPEN_FUNCTIONS_H
#define CANOPEN_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2021.02.08 21.39.50*/

#include "canopen.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>


void co_process_frame(chsm_tst *self, const cevent_tst *e_pst);

void co_send_bootup(chsm_tst *self, const cevent_tst *e_pst);


typedef enum canopen_state_id_ten
{
    S_CANOPEN = 0,
} canopen_state_id_ten;


/*
Signals:
    SIG_CAN_FRAME 
*/

/*
Other function notes:
*/
#endif
