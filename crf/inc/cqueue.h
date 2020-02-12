/*
 * event_queue.h
 *
 *  Created on: 2019. jan. 26.
 *      Author: jszeman
 */

#ifndef INC_CQUEUE_H_
#define INC_CQUEUE_H_

#include <cevent.h>
#include <stdint.h>
#include <stdlib.h>
#include "cevent.h"

typedef struct crf_event_queue_t
{
	const CEvent	**events;
	uint16_t		max;
	uint16_t		head;
	uint16_t		tail;
	uint16_t		free;
} CQueue;

int32_t 			cqueue_init(CQueue *this, const CEvent **events, uint16_t max_event_count);
int32_t 			cqueue_put(CQueue *this, const CEvent *e);
const CEvent		*cqueue_get(CQueue *this);
uint32_t 			cqueue_is_empty(CQueue *this);

#endif /* INC_CQUEUE_H_ */
