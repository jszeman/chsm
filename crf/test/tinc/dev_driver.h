#ifndef DEV_DRIVER_H
#define DEV_DRIVER_H

#include "chsm.h"
#include <stdbool.h>
#include "events.h"

#define DEV_DRV_LOG_SIZE 1024

typedef struct
{
    chsm_tst    sm;
    
    char        log_buff[DEV_DRV_LOG_SIZE];
    char        *log;

    bool        response_complete;
    bool        timeout;

} dev_driver_tst;

void dev_driver_clear_log(dev_driver_tst  *self);

chsm_result_ten dev_driver_top(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

#endif