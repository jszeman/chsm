#ifndef SDO_H
#define SDO_H

#include "crf.h"
#include SIGNAL_CLASSES_H
#include "od.h"
#include "sys_if.h"
#include "can_if.h"

typedef struct sdo_cfg_tst
{
    uint8_t                 node_id_u8;
    uint32_t                timeout_ms_u32;
    object_dictionary_tst*  od_pst;
} sdo_cfg_tst;

typedef struct sdo_tst
{
    /* PUBLIC */
    chsm_tst            super;
    sdo_cfg_tst         config_st;

    /* PRIVATE */
    uint8_t             toggle_bit_u8;
    od_entry_tst*       active_obj_pst;
    uint32_t            segment_offset_u32;
    uint8_t             block_size_u8;
    uint8_t             last_block_msg_size_u8;

    uint32_t            counter_ms_u32;
} sdo_tst;

chsm_result_ten sdo_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

bool sdo_timeout(chsm_tst *self, const cevent_tst *e_pst, uint32_t timeout_u32);
void send_sdo_abort(chsm_tst *self, const cevent_tst *e_pst, uint32_t abort_code_u32);

typedef enum canopen_sdo_internal_signals_ten
{
    SIG_CANOPEN_WAIT_EXP_SDO_UL = SIGNAL_FROM_CLASS(CRF_SIGNAL_CLASS_MOD_INTERNAL),
    SIG_CANOPEN_WAIT_EXP_SDO_DL,
    SIG_CANOPEN_SEG_DL_START,
    SIG_CANOPEN_SEG_DL_END,
    SIG_CANOPEN_SEG_UL_START,
    SIG_CANOPEN_SEG_UL_END,
    SIG_CANOPEN_BLOCK_UL_START,
    SIG_CANOPEN_BLOCK_UL_END,
    SIG_CANOPEN_BLOCK_UL_STARTED,
    SIG_CANOPEN_BLOCK_UL_FINISH,
} canopen_sdo_internal_signals_ten;

#define SDO_TIMEOUT (((sdo_tst *)self)->config_st.timeout_ms_u32)

/* Expedited download */
#define CO_SDO_DL_REQ_EXP_1B        ((1 << 5) | (3 << 2) | (1 << 1) | 1)
#define CO_SDO_DL_REQ_EXP_2B        ((1 << 5) | (2 << 2) | (1 << 1) | 1)
#define CO_SDO_DL_REQ_EXP_3B        ((1 << 5) | (1 << 2) | (1 << 1) | 1)
#define CO_SDO_DL_REQ_EXP_4B        ((1 << 5) | (0 << 2) | (1 << 1) | 1)
#define CO_SDO_DL_RESP_EXP          (3 << 5)

/* Expedited upload */
#define CO_SDO_UL_REQ               (2 << 5)
#define CO_SDO_UL_RESP_EXP_1B       ((2 << 5) | (3 << 2) | (1 << 1) | 1)
#define CO_SDO_UL_RESP_EXP_2B       ((2 << 5) | (2 << 2) | (1 << 1) | 1)
#define CO_SDO_UL_RESP_EXP_3B       ((2 << 5) | (1 << 2) | (1 << 1) | 1)
#define CO_SDO_UL_RESP_EXP_4B       ((2 << 5) | (0 << 2) | (1 << 1) | 1)

/* Segmented download */
#define CO_SDO_DL_REQ_SEG_INIT_SI               ((1 << 5) | 1)
#define CO_SDO_DL_REQ_SEG_INIT                  (1 << 5)

#define IS_CO_SDO_DL_SEG_REQ(CMD)               (0 == (CMD & 0xE0))
#define CO_SDO_DL_SEG_REQ(T_BIT, SIZE, LAST)    ((T_BIT << 4) | ((7 - SIZE) << 1) | LAST)
#define CO_SDO_DL_SEG_RESP(T_BIT)               ((1 << 5) | (T_BIT << 4))

/* Segmented upload */
#define CO_SDO_UL_REQ_SEG_INIT                  CO_SDO_UL_REQ
#define CO_SDO_UL_RESP_SEG_INIT_SI              ((2 << 5) | 1)

#define IS_CO_SDO_UL_SEG_REQ(CMD)               (0x60 == (CMD & 0xE0))
#define CO_SDO_UL_REQ_SEG(T_BIT)                ((3 << 5) | (T_BIT << 4))
#define CO_SDO_UL_RESP_SEG(T_BIT, SIZE, LAST)   CO_SDO_DL_SEG_REQ(T_BIT, SIZE, LAST)

/* Block upload */
#define CO_SDO_UL_REQ_BLK_INIT(CRC)             ((5 << 5) | (CRC << 2))
#define CO_SDO_UL_RESP_BLK_INIT(CRC)            ((6 << 5) | (CRC << 2) | (1 << 1))
#define CO_SDO_UL_REQ_BLK_START                 ((5 << 5) | 3)    
#define CO_SDO_UL_RESP_BLK_SUBBLOCK(LAST, NO)   ((LAST << 7) | NO)
#define CO_SDO_UL_RESP_BLK_ACK                  (5 << 5 | 2)
#define CO_SDO_UL_BLK_END                       ((6 << 5) | 1)
#define CO_SDO_UL_BLK_END_RESPONSE              ((5 << 5) | 1)
#define CO_SDO_UL_BLK_FINISH(SIZE)              ((6 << 5) | ((7 - SIZE) << 2) | 1)
#define CO_SDO_UL_BLK_FINISH_RESP               ((5 << 5) | 1)

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
