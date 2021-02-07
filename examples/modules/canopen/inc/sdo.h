#ifndef SDO_H
#define SDO_H

#include "crf.h"
#include "signals.h"
#include "od.h"

typedef struct sdo_cfg_tst
{
    uint8_t                 node_id_u8;
    object_dictionary_tst*  od_pst;
} sdo_cfg_tst;

typedef struct sdo_tst
{
    /* PUBLIC */
    chsm_tst            super;
    sdo_cfg_tst         config_st;

    /* PRIVATE */
} sdo_tst;

chsm_result_ten sdo_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

bool sdo_timeout(chsm_tst *self, const cevent_tst *e_pst, uint32_t timeout_u32);
void send_sdo_abort(chsm_tst *self, const cevent_tst *e_pst, uint32_t abort_code_u32);

typedef enum canopen_sdo_internal_signals_ten
{
    /* External signals */
    SIG_CANOPEN_WAIT_EXP_SDO_UL = SIGNAL_CLASS(SIG_INTERNAL),
    SIG_CANOPEN_WAIT_EXP_SDO_DL,
    SIG_CANOPEN_SEG_DL_START,
} canopen_sdo_internal_signals_ten;

#define SDO_TIMEOUT 3000
#define SDO_TIMEOUT_ABORT 0x12345678

#endif
