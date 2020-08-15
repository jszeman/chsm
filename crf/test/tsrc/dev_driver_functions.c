#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dev_driver.h"
#include "dev_driver_functions.h"
#include "crf.h"
#include <stdio.h>

static void load(dev_driver_tst *self, const char *str)
{
    while (*str)
    {
        *self->log = *str;
        self->log++;
        str++;
    }
}

void dev_driver_clear_log(dev_driver_tst  *self)
{
    memset(&self->log_buff, 0, sizeof(self->log_buff));
    self->log = self->log_buff;
}

void dev_drv_init(dev_driver_tst *self, const cevent_tst *e_pst)
{
    dev_driver_clear_log(self);
}

void cancel_request(dev_driver_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void inc_timer(dev_driver_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void insert_data(dev_driver_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void send_read_request(dev_driver_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void send_write_request(dev_driver_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}

void start_timer(dev_driver_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
}


bool response_complete(dev_driver_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
    return self->response_complete;
}

bool timeout(dev_driver_tst *self, const cevent_tst *e_pst)
{
    load(self, __func__);
    load(self, " ");
    return self->timeout;
}
