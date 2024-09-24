#ifndef SDO_FUNCTIONS_H
#define SDO_FUNCTIONS_H

#include "chsm.h"
#include "cevent.h"
#include "sdo.h"
#include "stdbool.h"

/* The following functions shall be implemented by the user */

void process_block_dl_end (chsm_tst *self, const cevent_tst *e_pst);

void process_block_finish_response (chsm_tst *self, const cevent_tst *e_pst);

void process_dl_segment (chsm_tst *self, const cevent_tst *e_pst);

void process_dl_subblock (chsm_tst *self, const cevent_tst *e_pst);

void process_sdo_request (chsm_tst *self, const cevent_tst *e_pst);

void process_ul_block_start (chsm_tst *self, const cevent_tst *e_pst);

void process_ul_segment (chsm_tst *self, const cevent_tst *e_pst);

void sdo_callback (chsm_tst *self, const cevent_tst *e_pst);

void sdo_init (chsm_tst *self, const cevent_tst *e_pst);

void sdo_reset_timer (chsm_tst *self, const cevent_tst *e_pst);

void send_block_finish (chsm_tst *self, const cevent_tst *e_pst);

void send_next_block (chsm_tst *self, const cevent_tst *e_pst);

void send_sdo_exp_dl_abort (chsm_tst *self, const cevent_tst *e_pst);

void send_sdo_exp_dl_response (chsm_tst *self, const cevent_tst *e_pst);

void send_sdo_exp_ul_abort (chsm_tst *self, const cevent_tst *e_pst);

void send_sdo_exp_ul_response (chsm_tst *self, const cevent_tst *e_pst);


bool is_abort_request (chsm_tst *self, const cevent_tst *e_pst);

bool is_not_abort_request (chsm_tst *self, const cevent_tst *e_pst);

bool process_ul_block_ack (chsm_tst *self, const cevent_tst *e_pst);


typedef enum 
{
    S_IDLE = 0,
    S_SDO_WAIT_EXP_UL = 1,
    S_SDO_WAIT_EXP_DL = 2,
    S_SDO_SEGMENTED_DL = 3,
    S_SDO_SEGMENTED_UL = 6,
    S_SDO_BLOCK_UL_WAIT_FOR_START = 7,
    S_SDO_BLOCK_UL_STARTED = 8,
    S_SDO_BLOCK_UL_FINISH = 9,
    S_SDO_BLOCK_DL_STARTED = 11,
    S_SDO_BLOCK_DL_FINISH = 12,
} ;

/*
Signals:
    SIG_CAN_FRAME     
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
    SIG_MEM_READ_FAIL     
    SIG_MEM_READ_SUCCESS 
    SIG_SYS_TICK_1ms 

The following functions shall be declared and implemented by the user:

    send_sdo_abort

    sdo_timeout

*/

#endif