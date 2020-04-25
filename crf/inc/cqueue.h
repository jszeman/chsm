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

typedef struct cqueue_st
{
	const cevent_tst	**events;
	uint16_t			max;
	uint16_t			head;
	uint16_t			tail;
	uint16_t			free;
} cqueue_tst;

int32_t 			cqueue_init(cqueue_tst *self, const cevent_tst **events, uint16_t max_event_count);
int32_t 			cqueue_put(cqueue_tst *self, const cevent_tst *e);
const cevent_tst	*cqueue_get(cqueue_tst *self);
uint32_t 			cqueue_is_empty(cqueue_tst *self);

#endif /* INC_CQUEUE_H */
