/*
 * chsm_test.h
 *
 *  Created on: 2019. febr. 4.
 *      Author: jszeman
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include <stdint.h>
#include "cevent.h"

enum test_events_en
{
    TEST_SIG_SEND_DATA = 512,
    TEST_SIG_START_WAIT,
    TEST_SIG_STOP_WAIT,
    TEST_SIG_DATA_RECEIVED,
    TEST_SIG_TICK_1MS,

    TEST_SIG_READ = 512,
    TEST_SIG_WRITE,
    TEST_SIG_RECEIVE,
    TEST_SIG_CANCEL,
};

#define TEST_SIG_READ_TYPE event_bus_data_tst
#define TEST_SIG_WRITE_TYPE event_bus_data_tst
#define TEST_SIG_RECEIVE_TYPE event_small_tst

typedef struct event_bus_data_st
{
    cevent_tst      e;
    uint16_t        id;
    uint8_t         dlc;
    uint8_t         rtr;
    uint8_t         data[8];
} event_bus_data_tst;

typedef struct event_small_st
{
    cevent_tst      e;
    uint16_t        value;
} event_small_tst;

typedef struct event_large_st
{
    cevent_tst      e;
    uint32_t        value[10];
} event_large_tst;



#endif /* #ifndef EVENTS_H_
 */