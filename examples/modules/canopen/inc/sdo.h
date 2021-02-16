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
    SIG_CANOPEN_WAIT_EXP_SDO_UL = SIGNAL_CLASS(CRF_SIGNAL_CLASS_MOD_INTERNAL),
    SIG_CANOPEN_WAIT_EXP_SDO_DL,
    SIG_CANOPEN_SEG_DL_START,
} canopen_sdo_internal_signals_ten;

#define SDO_TIMEOUT 3000

#define CO_SDO_DL_REQ_EXP_1B        ((1 << 5) | (3 << 2) | (1 << 1) | 1)
#define CO_SDO_DL_REQ_EXP_2B        ((1 << 5) | (2 << 2) | (1 << 1) | 1)
#define CO_SDO_DL_REQ_EXP_3B        ((1 << 5) | (1 << 2) | (1 << 1) | 1)
#define CO_SDO_DL_REQ_EXP_4B        ((1 << 5) | (0 << 2) | (1 << 1) | 1)
#define CO_SDO_DL_RESP_EXP          (3 << 5)

#define CO_SDO_UL_REQ               (2 << 5)
#define CO_SDO_UL_RESP_EXP_1B       ((2 << 5) | (3 << 2) | (1 << 1) | 1)
#define CO_SDO_UL_RESP_EXP_2B       ((2 << 5) | (2 << 2) | (1 << 1) | 1)
#define CO_SDO_UL_RESP_EXP_3B       ((2 << 5) | (1 << 2) | (1 << 1) | 1)
#define CO_SDO_UL_RESP_EXP_4B       ((2 << 5) | (0 << 2) | (1 << 1) | 1)

#define CO_SDO_ABORT                (4 << 5)

#define CO_SDO_ABORT_TOGGLE_BIT_NOT_ALTERNATED  0x05030000
#define CO_SDO_ABORT_TIMEOUT                    0x05040000
#define CO_SDO_ABORT_INVALID_COMMAND            0x05040001
#define CO_SDO_ABORT_INVALID_BLOCK_SIZE         0x05040002
#define CO_SDO_ABORT_INVALID_SEQUENCE_NUM       0x05040003
#define CO_SDO_ABORT_CRC_ERROR                  0x05040004
#define CO_SDO_ABORT_OUT_OF_MEMORY              0x05040005
#define CO_SDO_ABORT_UNSUPPORTED_ACCESS         0x06010000
#define CO_SDO_ABORT_WRITE_ONLY_OBJECT          0x06010001
#define CO_SDO_ABORT_READ_ONLY_OBJECT           0x06010002
#define CO_SDO_ABORT_OBJECT_NOT_FOUND           0x06020000
#define CO_SDO_ABORT_CANT_MAP_TO_PDO            0x06040041
#define CO_SDO_ABORT_PDO_LENGTH_LIMIT           0x06040042
#define CO_SDO_ABORT_PARAMETER_INCOMPATIBLE     0x06040043
#define CO_SDO_ABORT_INTERNAL_INCOMPATIBLITY    0x06040047
#define CO_SDO_ABORT_HW_ERROR                   0x06060000
#define CO_SDO_ABORT_LENGTH_MISMATCH            0x06070010
#define CO_SDO_ABORT_LENGTH_TOO_HIGH            0x06070012
#define CO_SDO_ABORT_LENGTH_TOO_LOW             0x06070013
#define CO_SDO_ABORT_SUBINDEX_NOT_EXISTS        0x06090011
#define CO_SDO_ABORT_INVALID_VALUE              0x06090030
#define CO_SDO_ABORT_VALUE_TOO_HIGH             0x06090031
#define CO_SDO_ABORT_VALUE_TOO_LOW              0x06090032
#define CO_SDO_ABORT_MAX_VALUE_LESS_THAN_MIN    0x06090036
#define CO_SDO_ABORT_RESOURCE_NOT_AVAILABLE     0x060A0023
#define CO_SDO_ABORT_GENERAL_ERROR              0x08000000
#define CO_SDO_ABORT_STORE_ERROR                0x08000020
#define CO_SDO_ABORT_LOCAL_STORE_ERROR          0x08000021
#define CO_SDO_ABORT_STATE_STORE_ERROR          0x08000022
#define CO_SDO_ABORT_NO_OBJ_DICT                0x08000023
#define CO_SDO_ABORT_NO_DATA                    0x08000024 

#endif
