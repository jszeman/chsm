#ifndef SDO_H
#define SDO_H

#include "crf.h"
#include SIGNAL_CLASSES_H
#include "od.h"
#include "sys_if.h"
#include "can_if.h"

#define SDO_BLOCK_SIZE 7
#define SDO_BLOCK_BUFFER_SIZE (SDO_BLOCK_SIZE * 7)

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

    uint8_t             block_buffer_au8[SDO_BLOCK_BUFFER_SIZE];
    uint8_t             subblock_counter_u8;

    uint32_t            counter_ms_u32;
} sdo_tst;

chsm_result_ten sdo_top(chsm_tst *self, const cevent_tst *e_pst);

od_entry_tst* find_od_entry(sdo_tst* self, uint32_t mlx_u32);

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
    SIG_CANOPEN_BLOCK_DL_START,
    SIG_CANOPEN_BLOCK_DL_END,
    SIG_CANOPEN_BLOCK_DL_FINISH,
} canopen_sdo_internal_signals_ten;

#define SDO_TIMEOUT (((sdo_tst *)self)->config_st.timeout_ms_u32)

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
