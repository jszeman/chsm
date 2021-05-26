#ifndef SYS_IF_H
#define SYS_IF_H

#include "crf.h"
#include SIGNAL_CLASSES_H

/*
 * SYSTEM SIGNALS
 */

typedef enum sys_signals_ten
{
    SIG_SYS_TICK_1ms = SIGNAL_FROM_CLASS(SIG_CLASS_SYS),
    SIG_SYS_TICK_10ms,
    SIG_SYS_TICK_100ms,
    SIG_SYS_TICK_1s,
    SIG_SYS_RESET
} sys_signals_ten;

/*
 * Constant event definitions.
 */
extern const cevent_tst sys_event_reset_st;
extern const cevent_tst sys_event_tick_1ms_st;
extern const cevent_tst sys_event_tick_10ms_st;
extern const cevent_tst sys_event_tick_100ms_st;
extern const cevent_tst sys_event_tick_1s_st;

#endif
