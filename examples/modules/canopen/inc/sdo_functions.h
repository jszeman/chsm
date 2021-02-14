#ifndef SDO_FUNCTIONS_H
#define SDO_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2021.02.14 22.00.55*/

#include "sdo.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>


void process_sdo_request(chsm_tst *self, const cevent_tst *e_pst);

void send_sdo_exp_dl_abort(chsm_tst *self, const cevent_tst *e_pst);

void send_sdo_exp_dl_response(chsm_tst *self, const cevent_tst *e_pst);

void send_sdo_exp_ul_abort(chsm_tst *self, const cevent_tst *e_pst);

void send_sdo_exp_ul_response(chsm_tst *self, const cevent_tst *e_pst);


bool is_abort_request(chsm_tst *self, const cevent_tst *e_pst);

bool is_not_abort_request(chsm_tst *self, const cevent_tst *e_pst);

typedef enum sdo_state_id_ten
{
    S_IDLE = 0,
    S_SDO_WAIT_EXP_UL = 1,
    S_SDO_WAIT_EXP_DL = 2,
    S_SDO_WAIT_SEG_DL = 3,
    S_SDO = 4,
    S_BUSY = 5,
} sdo_state_id_ten;


/*
Signals:
    SIG_CANOPEN_SEG_DL_START    
    SIG_CANOPEN_WAIT_EXP_SDO_DL 
    SIG_CANOPEN_WAIT_EXP_SDO_UL 
    SIG_CAN_FRAME               
    SIG_MEM_READ_FAIL           
    SIG_MEM_READ_SUCCESS        
*/

/*
Other function notes:

sdo_timeout:

send_sdo_abort:
*/
#endif
