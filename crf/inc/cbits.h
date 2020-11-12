#ifndef INC_CBITS_H_
#define INC_CBITS_H_

#include <stdint.h>
#include <stdbool.h>
#include "cevent.h"

/* Cbits module
 * Provide some simple objects to generate events based on bits in a byte
 */

typedef struct cbits_tst      cbits_tst;
typedef struct cbits_cfg_tst  cbits_cfg_tst;

struct cbits_cfg_tst
{
    const cevent_tst          *active_events_pst;     // Pointer to a constant array of cevents.
    const cevent_tst          *inactive_events_pst;   // Pointer to a constant array of cevents.
    void                      (*send_pft)(cbits_tst *self, const cevent_tst *e_pst);
};

struct cbits_tst
{
    cbits_cfg_tst             config;

    void                      (*set_data)(cbits_tst *self, uint32_t data_u32);

    uint32_t                  prev_data_u32;
};

bool cbits_init(cbits_tst *self, uint32_t data_u32);

#endif /* INC_CBITS_H_ */
