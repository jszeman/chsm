#ifndef cAN_IF_H
#define cAN_IF_H

#include <stdint.h>
#include <stdbool.h>
#include "crf.h"
#include "cevent.h"
#include SIGNAL_CLASSES_H

/*
 * CAN INTERFACE SIGNALS
 */

typedef enum can_signals_ten
{
    SIG_CAN_FRAME = SIGNAL_FROM_CLASS(SIG_CLASS_CAN),
} can_signals_ten;

typedef struct can_frame_header_bits_tst
{
    uint32_t                    id_u12:     12;
    uint32_t                    rtr_u1:     1;
    uint32_t                    dlc_u4:     4;
    uint32_t                    rsvd_u15:   15;
} can_frame_header_bits_tst;

typedef union can_frame_header_tun
{
    uint32_t                    all_u32;
    can_frame_header_bits_tst   bit_st;
} can_frame_header_tun;

#define CAN_HDR(ID, RTR, DLC) ((can_frame_header_tun){.bit_st = {.id_u12 = (ID), .rtr_u1 = (RTR), .dlc_u4 = (DLC)}})

typedef struct can_mdl_bits_tst
{
    uint32_t                    d0_u8: 8;
    uint32_t                    d1_u8: 8;
    uint32_t                    d2_u8: 8;
    uint32_t                    d3_u8: 8;
} can_mdl_bits_tst;

typedef union can_mdl_tun
{
    uint32_t                    all_u32;
    can_mdl_bits_tst            bit_st;
} can_mdl_tun;

typedef struct can_mdh_bits_tst
{
    uint32_t                    d4_u8: 8;
    uint32_t                    d5_u8: 8;
    uint32_t                    d6_u8: 8;
    uint32_t                    d7_u8: 8;
} can_mdh_bits_tst;

typedef union can_mdh_tun
{
    uint32_t                    all_u32;
    can_mdh_bits_tst            bit_st;
} can_mdh_tun;

typedef struct can_frame_tst 
{
    cevent_tst                  super;
    can_frame_header_tun        header_un;
    can_mdl_tun                 mdl_un;
    can_mdh_tun                 mdh_un;
} can_frame_tst;

/*
 * Why can't frame data be declared as an uint8_t array?
 * 
 * There are architectures where sizeof(char)==16 so uint8_t is not even
 * defined. Also having 32 bit alignment is advantageous, but not trivial to
 * force in a portable way for uint8_t arrays.
 */

#define SIG_CAN_FRAME_TYPE           can_frame_tst



#endif
