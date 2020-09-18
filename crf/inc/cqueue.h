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

typedef struct cqueue_tst cqueue_tst;

struct cqueue_tst
{
	const cevent_tst	**events;
	uint16_t			max;
	uint16_t			head;
	uint16_t			tail;
	uint16_t			free;
	
	int32_t 			(*put)(cqueue_tst *self, const cevent_tst *e_cpst);
	int32_t 			(*put_left)(cqueue_tst *self, const cevent_tst *e_cpst);
	const cevent_tst*	(*get)(cqueue_tst *self);
	const cevent_tst*	(*get_right)(cqueue_tst *self);
	uint32_t 			(*is_empty)(cqueue_tst *self);
};

int32_t 				cqueue_init(cqueue_tst *self, const cevent_tst **events, uint16_t max_event_count);

#endif /* INC_CQUEUE_H */
