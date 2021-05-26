#include "signals.h"
#include "sys_if.h"
#include "cevent.h"

const cevent_tst sys_event_reset_st = {.sig = SIG_SYS_RESET};
const cevent_tst sys_event_tick_1ms_st = {.sig = SIG_SYS_TICK_1ms};
const cevent_tst sys_event_tick_10ms_st = {.sig = SIG_SYS_TICK_10ms};
const cevent_tst sys_event_tick_100ms_st = {.sig = SIG_SYS_TICK_100ms};
const cevent_tst sys_event_tick_1s_st = {.sig = SIG_SYS_TICK_1s};
