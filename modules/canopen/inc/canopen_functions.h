#ifndef CANOPEN_FUNCTIONS_H
#define CANOPEN_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2022.11.17 20.21.44*/

#include "canopen.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>


void co_callback(chsm_tst *self, const cevent_tst *e_pst);

void co_node_init(chsm_tst *self, const cevent_tst *e_pst);

void co_process_frame(chsm_tst *self, const cevent_tst *e_pst);

void co_reset_timer(chsm_tst *self, const cevent_tst *e_pst);

void co_send_bootup(chsm_tst *self, const cevent_tst *e_pst);

void co_send_ng_active(chsm_tst *self, const cevent_tst *e_pst);

void co_send_ng_inactive(chsm_tst *self, const cevent_tst *e_pst);


typedef enum canopen_state_id_ten
{
    S_NG_INACTIVE = 1,
    S_NG_ACTIVE = 2,
} canopen_state_id_ten;


/*
Signals:
    SIG_CAN_FRAME    
    SIG_I_CANOPEN_NG 
    SIG_SYS_TICK_1ms 
*/

/*
Other function notes:

co_timeout:
*/
#endif
