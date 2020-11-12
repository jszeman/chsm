#include "cbits.h"
#include <stdio.h>
#include <assert.h>

static void set_data(cbits_tst *self, uint32_t data_u32)
{
    uint32_t changed_u32;

    assert(self);

    if (NULL == self->config.send_pft) return;

    changed_u32 = data_u32 ^ self->prev_data_u32;

    for (uint32_t i=0; i<32; i++)
    {
        if (0 == changed_u32) return;

        uint32_t b_u32 = (1UL << i);

        if (!(changed_u32 & b_u32)) continue;

        if ((data_u32 & b_u32) && self->config.active_events_pst && self->config.active_events_pst[i].sig)
        {
            self->config.send_pft(self, self->config.active_events_pst + i);
        }
        else if (self->config.inactive_events_pst && self->config.inactive_events_pst[i].sig)
        {
            self->config.send_pft(self, self->config.inactive_events_pst + i);
        }
    }

    self->prev_data_u32 = data_u32;
}

bool cbits_init(cbits_tst *self, uint32_t data_u32)
{
    self->set_data = set_data;

    if (self->config.send_pft) return true;

    return false;
}