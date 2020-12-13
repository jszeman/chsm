#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "cevent.h"
#include "cvalue.h"


bool cvalue_is_ok(cvalue_tst *self)
{
	return  self->last_pst == self->config.in_range_event_pst;
}

static void set_value(cvalue_tst *self, int32_t data)
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

    if ((self->in_cnt_u32 == self->config.filter_count_u32) &&
        (self->last_pst != self->config.in_range_event_pst))
    {
        self->config.send(self->config.user_param_pv, self->config.in_range_event_pst);
        self->last_pst = self->config.in_range_event_pst;
    }
    else if ((self->low_cnt_u32 == self->config.filter_count_u32) &&
        (self->last_pst != self->config.low_limit_event_pst))
    {
        self->config.send(self->config.user_param_pv, self->config.low_limit_event_pst);
        self->last_pst = self->config.low_limit_event_pst;
    }
    else if ((self->high_cnt_u32 == self->config.filter_count_u32) &&
        (self->last_pst != self->config.high_limit_event_pst))
    {
        self->config.send(self->config.user_param_pv, self->config.high_limit_event_pst);
        self->last_pst = self->config.high_limit_event_pst;
    }
}

bool cvalue_init(cvalue_tst *self)
{
    self->set_value = set_value;

    self->low_cnt_u32 = 0;
    self->in_cnt_u32 = 0;
    self->high_cnt_u32 = 0;

    self->last_pst = NULL;

    return true;
}
