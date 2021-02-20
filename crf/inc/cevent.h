#ifndef INC_CEVENT_H_
#define INC_CEVENT_H_

#include <stdint.h>

/** cevent - Base class for events
 *
 * Fields:
 * 		sig:		Event signal.
 * 		gc_info:	Used internally by CRF to keep track of the reference count
 * 					on the event, and also to identify the event pool that
 * 					handles the physical memory used by the event.
 */

#ifndef SIGNAL_T
#define SIGNAL_T uint16_t
#endif

typedef SIGNAL_T	signal_t;

typedef struct gc_info_tst
{
	uint16_t		ref_cnt: 12;
	uint16_t		pool_id: 4;
} gc_info_tst;

#define CEVENT_INVALID_POOL 0

typedef struct cevent_tst
{
	signal_t		sig;
	gc_info_tst		gc_info;
} cevent_tst;

#define CRF_SIGNAL_CLASS_MOD_INTERNAL 1
#define CRF_SIGNAL_CLASS_START 2
#define CRF_SIGNAL_CLASS_SIZE 256

#define SIGNAL_FROM_CLASS(CLASS) (CLASS * CRF_SIGNAL_CLASS_SIZE)

static inline void cevent_ref_cnt_inc(const cevent_tst *e_pst)
{
	if (CEVENT_INVALID_POOL != e_pst->gc_info.pool_id)
	{
		((cevent_tst *)e_pst)->gc_info.ref_cnt++;
	}
}

static inline void cevent_ref_cnt_dec(const cevent_tst *e_pst)
{
	if (CEVENT_INVALID_POOL != e_pst->gc_info.pool_id)
	{
		((cevent_tst *)e_pst)->gc_info.ref_cnt--;
	}
}

#endif /* INC_CEVENT_H_ */
