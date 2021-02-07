#ifndef CANOPEN_H
#define CANOPEN_H

#include "crf.h"
#include "signals.h"
#include "sdo.h"
#include "od.h"
#include <stdint.h>

typedef struct co_node_cfg_tst
{
    uint8_t                 node_id_u8;
    object_dictionary_tst*  od_pst;
} co_node_cfg_tst;

typedef struct co_node_tst
{
    /* PUBLIC */
    chsm_tst                super;
    co_node_cfg_tst         config_st;

    /* PRIVATE */
    sdo_tst                 sdo_st;

} co_node_tst;

chsm_result_ten co_node_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

/*
The co_node_st is designed to behave like a queue, so that other modules can
send events to it. Only the put function is implemented and it peeks into the
event to determine which internal state machine will process the event. So the
SDO hsm will only receive relevant events and so on.
*/

#endif
