#ifndef INC_CVALUE_H_
#define INC_CVALUE_H_

#include <stdint.h>
#include <stdbool.h>
#include "cevent.h"


typedef struct cvalue_tst cvalue_tst;
typedef struct cvalue_cfg_tst cvalue_cfg_tst;

struct cvalue_cfg_tst
{
    const cevent_tst    *low_limit_event_pst;
    const cevent_tst    *in_range_event_pst;
    const cevent_tst    *high_limit_event_pst;

    int32_t             low_limit_i32;
    int32_t             high_limit_i32;
    uint32_t            filter_count_u32;
    void                *user_param_pv;

    void                (*send)(void *user_param_pv, const cevent_tst *e_pst);
};

struct cvalue_tst
{
    cvalue_cfg_tst      config;
    uint32_t            low_cnt_u32;
    uint32_t            in_cnt_u32;
    uint32_t            high_cnt_u32;
    const cevent_tst    *last_pst;

    void                (*set_value)(cvalue_tst *self, int32_t data);
};

bool cvalue_init(cvalue_tst *self);
bool cvalue_is_ok(cvalue_tst *self);


#endif /* INC_CVALUE_H_ */
