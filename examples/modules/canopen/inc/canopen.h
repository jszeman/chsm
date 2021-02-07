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


/* Broadcast COB-IDs */
#define CO_NMT_BROADCAST_ID         0x000
#define CO_SYNC_BROADCAST_ID        0x080
#define CO_TIME_BROADCAST_ID        0x100

/* Function base COB-IDs. The node id must be added to generate a valid COB-ID*/
#define CO_EMCY                     0x080
#define CO_PDO1_TX              0x180
#define CO_PDO1_RX              0x200
#define CO_PDO2_TX              0x280
#define CO_PDO2_RX              0x300
#define CO_PDO3_TX              0x380
#define CO_PDO3_RX              0x400
#define CO_PDO4_TX              0x480
#define CO_PDO4_RX              0x500
#define CO_SDO_TX               0x580
#define CO_SDO_RX               0x600
#define CO_NMT                  0x700

#endif
