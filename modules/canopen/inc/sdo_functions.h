#ifndef SDO_FUNCTIONS_H
#define SDO_FUNCTIONS_H

/*Generated with CHSM v0.0.0 at 2021.02.24 21.55.59*/

#include "sdo.h"
#include "chsm.h"
#include "cevent.h"
#include <stdbool.h>


void process_block_dl_end(chsm_tst *self, const cevent_tst *e_pst);

void process_block_finish_response(chsm_tst *self, const cevent_tst *e_pst);

void process_dl_segment(chsm_tst *self, const cevent_tst *e_pst);

void process_dl_subblock(chsm_tst *self, const cevent_tst *e_pst);

void process_sdo_request(chsm_tst *self, const cevent_tst *e_pst);

void process_ul_block_start(chsm_tst *self, const cevent_tst *e_pst);

void process_ul_segment(chsm_tst *self, const cevent_tst *e_pst);

void sdo_callback(chsm_tst *self, const cevent_tst *e_pst);

void sdo_init(chsm_tst *self, const cevent_tst *e_pst);

void sdo_reset_timer(chsm_tst *self, const cevent_tst *e_pst);

void send_block_finish(chsm_tst *self, const cevent_tst *e_pst);

void send_next_block(chsm_tst *self, const cevent_tst *e_pst);

void send_sdo_exp_dl_abort(chsm_tst *self, const cevent_tst *e_pst);

void send_sdo_exp_dl_response(chsm_tst *self, const cevent_tst *e_pst);

void send_sdo_exp_ul_abort(chsm_tst *self, const cevent_tst *e_pst);

void send_sdo_exp_ul_response(chsm_tst *self, const cevent_tst *e_pst);


bool is_abort_request(chsm_tst *self, const cevent_tst *e_pst);

bool is_not_abort_request(chsm_tst *self, const cevent_tst *e_pst);

bool process_ul_block_ack(chsm_tst *self, const cevent_tst *e_pst);

typedef enum sdo_state_id_ten
{
    S_IDLE = 0,
    S_SDO_WAIT_EXP_UL = 1,
    S_SDO_WAIT_EXP_DL = 2,
    S_SDO_SEGMENTED_DL = 3,
    S_SDO = 4,
    S_BUSY = 5,
    S_SDO_SEGMENTED_UL = 6,
    S_SDO_BLOCK_UL_WAIT_FOR_START = 7,
    S_SDO_BLOCK_UL_STARTED = 8,
    S_SDO_BLOCK_UL_FINISH = 9,
    S_BLOCK_TRANSFER = 10,
    S_SDO_BLOCK_DL_STARTED = 11,
    S_SDO_BLOCK_DL_FINISH = 12,
} sdo_state_id_ten;


/*
Signals:
    SIG_CANOPEN_BLOCK_DL_FINISH  
    SIG_CANOPEN_BLOCK_DL_START   
    SIG_CANOPEN_BLOCK_UL_END     
    SIG_CANOPEN_BLOCK_UL_FINISH  
    SIG_CANOPEN_BLOCK_UL_START   
    SIG_CANOPEN_BLOCK_UL_STARTED 
    SIG_CANOPEN_SEG_DL_END       
    SIG_CANOPEN_SEG_DL_START     
    SIG_CANOPEN_SEG_UL_END       
    SIG_CANOPEN_SEG_UL_START     
    SIG_CANOPEN_WAIT_EXP_SDO_DL  
    SIG_CANOPEN_WAIT_EXP_SDO_UL  
    SIG_CAN_FRAME                
    SIG_MEM_READ_FAIL            
    SIG_MEM_READ_SUCCESS         
    SIG_SYS_TICK_1ms             
*/

/*
Other function notes:

sdo_timeout:

send_sdo_abort:
*/
#endif
