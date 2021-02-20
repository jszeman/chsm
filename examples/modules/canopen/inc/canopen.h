#ifndef CANOPEN_H
#define CANOPEN_H

#include "crf.h"
#include SIGNAL_CLASSES_H
#include "sdo.h"
#include "od.h"
#include "sys_if.h"
#include <stdint.h>

/*
 * CANOPEN INTERFACE SIGNALS
 */

typedef enum canopen_signals_ten
{
    /* External signals */
    SIG_CANOPEN_NG_ACTIVE = SIGNAL_FROM_CLASS(SIG_CLASS_CANOPEN),
    SIG_CANOPEN_NG_INACTIVE,
} canopen_signals_ten;

#define SIG_CANOPEN_NG_ACTIVE_TYPE      cevent_tst
#define SIG_CANOPEN_NG_INACTIVE_TYPE    cevent_tst

typedef struct co_node_tst co_node_tst;

typedef struct co_node_cfg_tst
{
    uint8_t                 node_id_u8;
    uint16_t                guard_time_ms_u16;
    uint16_t                life_time_factor_u16;
    uint32_t                sdo_timeout_ms_u32;
    object_dictionary_tst*  od_pst;
} co_node_cfg_tst;

struct co_node_tst
{
    /* PUBLIC */
    chsm_tst                super;
    co_node_cfg_tst         config_st;

    /* PRIVATE */
    sdo_tst                 sdo_st;
    const cevent_tst*		sdo_events_ast[6];

    bool                    ng_toggle_state_b;
    uint8_t                 nmt_state_u8;

    uint32_t                timer_u32;

};

chsm_result_ten co_node_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);
bool co_timeout(chsm_tst *self, const cevent_tst *e_pst, uint32_t timeout_u32);

#define CO_LIFETIME                 (((co_node_tst *)self)->config_st.guard_time_ms_u16 * ((co_node_tst *)self)->config_st.life_time_factor_u16)

/* Broadcast COB-IDs */
#define CO_NMT_BROADCAST_ID         0x000
#define CO_SYNC_BROADCAST_ID        0x080
#define CO_TIME_BROADCAST_ID        0x100

/* Function base COB-IDs. The node id must be added to generate a valid COB-ID*/
#define CO_EMCY                     0x080
#define CO_PDO1_TX                  0x180
#define CO_PDO1_RX                  0x200
#define CO_PDO2_TX                  0x280
#define CO_PDO2_RX                  0x300
#define CO_PDO3_TX                  0x380
#define CO_PDO3_RX                  0x400
#define CO_PDO4_TX                  0x480
#define CO_PDO4_RX                  0x500
#define CO_SDO_TX                   0x580
#define CO_SDO_RX                   0x600
#define CO_NMT                      0x700


#define CO_NMT_CMD_START            0x01
#define CO_NMT_CMD_STOP             0x02
#define CO_NMT_CMD_PREOP            0x80
#define CO_NMT_CMD_RESET            0x81
#define CO_NMT_CMD_RESET_COMM       0x82

#define CO_NMT_STATE_PREOP          0x7f
#define CO_NMT_STATE_BOOTUP         0x00
#define CO_NMT_STATE_OPERATIONAL    0x05
#define CO_NMT_STATE_STOPPED        0x04



typedef enum canopen_internal_signals_ten
{
    /* Internal signals */
    SIG_I_CANOPEN_NG = SIGNAL_FROM_CLASS(CRF_SIGNAL_CLASS_MOD_INTERNAL),
} canopen_internal_signals_ten;

#endif
