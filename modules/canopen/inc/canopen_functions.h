#ifndef CANOPEN_FUNCTIONS_H
#define CANOPEN_FUNCTIONS_H

#include "chsm.h"
#include "cevent.h"
#include "canopen.h"
#include "stdbool.h"

/* The following functions shall be implemented by the user */

void co_callback (chsm_tst *self, const cevent_tst *e_pst);

void co_node_init (chsm_tst *self, const cevent_tst *e_pst);

void co_process_frame (chsm_tst *self, const cevent_tst *e_pst);

void co_reset_timer (chsm_tst *self, const cevent_tst *e_pst);

void co_send_bootup (chsm_tst *self, const cevent_tst *e_pst);

void co_send_ng_active (chsm_tst *self, const cevent_tst *e_pst);

void co_send_ng_inactive (chsm_tst *self, const cevent_tst *e_pst);



typedef enum chsm_test_machine_state_id_ten
{
    S_NG_INACTIVE = 1,
    S_NG_ACTIVE = 2,
} chsm_test_machine_state_id_ten;

/*
Signals:
    SIG_CAN_FRAME 
    SIG_I_CANOPEN_NG 
    SIG_SYS_TICK_1ms 

The following functions shall be declared and implemented by the user:

    co_timeout

*/

#endif