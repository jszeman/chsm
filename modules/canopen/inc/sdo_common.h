#ifndef SDO_COMMON_H
#define SDO_COMMON_H

/*
 * SDO related internal definitions and macros
 */

#include "crf.h"
#include "sdo.h"
#include <stdint.h>

extern const cevent_tst sdo_seg_dl_start_event;
extern const cevent_tst sdo_seg_dl_end_event;
extern const cevent_tst sdo_seg_ul_start_event;
extern const cevent_tst sdo_seg_ul_end_event;
extern const cevent_tst sdo_block_ul_start_event;
extern const cevent_tst sdo_block_ul_end_event;
extern const cevent_tst sdo_block_ul_started_event;
extern const cevent_tst sdo_block_ul_finish_event;
extern const cevent_tst sdo_block_dl_start_event;
extern const cevent_tst sdo_block_dl_end_event;
extern const cevent_tst sdo_block_dl_finish_event;

typedef struct sdo_hdr_bits_tst
{
    uint32_t        command_u8:     8;
    uint32_t        index_u16:      16;
    uint32_t        subindex_u8:    8;
} sdo_hdr_bits_tst;

typedef union sdo_hdr_tun
{
    uint32_t        all_u32;
    sdo_hdr_bits_tst    bit_st;
} sdo_hdr_tun;

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
#define CO_SDO_UL_RESP_BLK_SUBBLOCK(LAST, NO)   ((LAST << 7) | (NO + 1))
#define CO_SDO_UL_RESP_BLK_ACK                  (5 << 5 | 2)
#define CO_SDO_UL_BLK_END                       ((6 << 5) | 1)
#define CO_SDO_UL_BLK_END_RESPONSE              ((5 << 5) | 1)
#define CO_SDO_UL_BLK_FINISH(SIZE)              ((6 << 5) | ((7 - SIZE) << 2) | 1)
#define CO_SDO_UL_BLK_FINISH_RESP               ((5 << 5) | 1)

/* Block download */
#define CO_SDO_DL_REQ_BLK_INIT(CRC, SIZE)       ((6 << 5) | (CRC << 2) | (SIZE << 1))
#define CO_SDO_DL_RESP_BLK_INIT(CRC)            ((5 << 5) | (CRC << 2))
#define CO_SDO_DL_REQ_BLK_SUBBLOCK(LAST, NO)    ((LAST << 7) | (NO + 1))
#define CO_SDO_DL_REQ_BLK_ACK                   ((5 << 5) | 2)
#define CO_SDO_DL_BLK_FINISH(SIZE)              ((6 << 5) | ((7 - SIZE) << 2) | 1)
#define CO_SDO_DL_BLK_FINISH_RESP               ((5 << 5) | 1)
#define IS_CO_SDO_DL_BLK_FINISH(D0)             (((6 << 5) | 1) == (0xE3 & D0))

#define CO_SDO_ABORT                (4 << 5)

static inline uint8_t min(uint8_t a, uint16_t b)
{
    return (a < b) ? a : b; 
}

void send_sdo_abort(chsm_tst *self, const cevent_tst *e_pst, uint32_t abort_code_u32);
void sdo_abort(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst* r_pst, uint32_t abort_code_u32);


void handle_ul(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst);
void handle_block_dl_init(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst);

void handle_exp_dl(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst, uint16_t len_u16);
void handle_seg_dl(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst);

void handle_block_ul_init(sdo_tst* self, can_frame_tst* f_pst, can_frame_tst *r_pst, od_entry_tst* od_entry_pst);

#endif