#include <stdint.h>
#include "cevent.h"
#include "cvalue.h"

static void set_value_in(cvalue_tst *self, int32_t data);
static void set_value_low(cvalue_tst *self, int32_t data);
static void set_value_high(cvalue_tst *self, int32_t data);

static void set_value_start(cvalue_tst *self, int32_t data)
{
    if (data < self->config.low_limit_i32)
    {
        self->low_cnt_u32++;
        self->in_cnt_u32 = 0;
        self->high_cnt_u32 = 0;
    }
    else if (data > self->config.high_limit_i32)
    {
        self->low_cnt_u32 = 0;
        self->in_cnt_u32 = 0;
        self->high_cnt_u32++;
    }
    else
    {
        self->low_cnt_u32 = 0;
        self->in_cnt_u32++;
        self->high_cnt_u32 = 0;
    }

    if (self->in_cnt_u32 == self->config.filter_count_u32)
    {
        self->config.send(self, self->config.in_range_event_pst);
        self->set_value = set_value_in;
    }

    if (self->low_cnt_u32 == self->config.filter_count_u32)
    {
        self->config.send(self, self->config.low_limit_event_pst);
        self->set_value = set_value_low;
    }

    if (self->high_cnt_u32 == self->config.filter_count_u32)
    {
        self->config.send(self, self->config.high_limit_event_pst);
        self->set_value = set_value_high;
    }
}

static void set_value_in(cvalue_tst *self, int32_t data)
{
    if (data < self->config.low_limit_i32)
    {
        self->low_cnt_u32++;
    }
    else if (data > self->config.high_limit_i32)
    {
        self->high_cnt_u32++;
    }
    else
    {
        self->low_cnt_u32 = 0;
        self->high_cnt_u32 = 0;
    }

    if (self->low_cnt_u32 == self->config.filter_count_u32)
    {
        self->config.send(self, self->config.low_limit_event_pst);
        self->in_cnt_u32 = 0;
        self->set_value = set_value_low;
    }

    if (self->high_cnt_u32 == self->config.filter_count_u32)
    {
        self->config.send(self, self->config.high_limit_event_pst);
        self->in_cnt_u32 = 0;
        self->set_value = set_value_high;
    }
}

static void set_value_low(cvalue_tst *self, int32_t data)
{
    if (data < self->config.low_limit_i32)
    {
        self->in_cnt_u32 = 0;
        self->high_cnt_u32 = 0;
    }
    else if (data > self->config.high_limit_i32)
    {
        self->high_cnt_u32++;
    }
    else
    {
        self->in_cnt_u32++;
    }

    if (self->in_cnt_u32 == self->config.filter_count_u32)
    {
        self->config.send(self, self->config.in_range_event_pst);
        self->low_cnt_u32 = 0;
        self->set_value = set_value_in;
    }

    if (self->high_cnt_u32 == self->config.filter_count_u32)
    {
        self->config.send(self, self->config.high_limit_event_pst);
        self->low_cnt_u32 = 0;
        self->set_value = set_value_high;
    }
}

static void set_value_high(cvalue_tst *self, int32_t data)
{
    if (data < self->config.low_limit_i32)
    {
        self->low_cnt_u32++;
    }
    else if (data > self->config.high_limit_i32)
    {
        self->low_cnt_u32 = 0;
        self->in_cnt_u32 = 0;
    }
    else
    {
        self->in_cnt_u32++;
    }

    if (self->in_cnt_u32 == self->config.filter_count_u32)
    {
        self->config.send(self, self->config.in_range_event_pst);
        self->high_cnt_u32 = 0;
        self->set_value = set_value_in;
    }

    if (self->low_cnt_u32 == self->config.filter_count_u32)
    {
        self->config.send(self, self->config.low_limit_event_pst);
        self->high_cnt_u32 = 0;
        self->set_value = set_value_low;
    }
}

bool cvalue_init(cvalue_tst *self)
{
    self->set_value = set_value_start;

    self->low_cnt_u32 = 0;
    self->in_cnt_u32 = 0;
    self->high_cnt_u32 = 0;

    return true;
}