/*
 * event_queue.h
 *
 *  Created on: 2019. jan. 26.
 *      Author: jszeman
 */

#ifndef INC_CQUEUE_H
#define INC_CQUEUE_H

#include <stdint.h>
#include <stdlib.h>
#include "cevent.h"
#include "atomic_ops.h"

/*
 * Thread safe multi-producer single consumer event queue
 * 
 * The application should only ever need to call the put method directly
 * to place events into the queue. Calling this function from interrupts
 * and other threads is safe.
 * All the other methods sould only be called from the main thread.
 */

typedef struct cqueue_tst cqueue_tst;

struct cqueue_tst
{
	int32_t 			(*put)(cqueue_tst *self, const cevent_tst *e_cpst);
	int32_t 			(*put_left)(cqueue_tst *self, const cevent_tst *e_cpst);
	const cevent_tst*	(*get)(cqueue_tst *self);
	const cevent_tst*	(*get_right)(cqueue_tst *self);

	const cevent_tst	**events;
	uint16_t			max;
	atomic_uint16_t	 	head;
	atomic_uint16_t		tail;
	uint16_t 			mask;
	uint32_t			fault_cnt;
};

int32_t 				cqueue_init(cqueue_tst *self, const cevent_tst **events, uint16_t max_event_count);

#endif /* INC_CQUEUE_H */
