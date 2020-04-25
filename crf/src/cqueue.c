/*
 * event_queue.c
 *
 *  Created on: 2019. jan. 26.
 *      Author: jszeman
 */

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <cevent.h>
#include <cqueue.h>

int32_t 			cqueue_init(cqueue_tst *self, const cevent_tst **events, uint16_t max_event_count);
int32_t 			cqueue_put(cqueue_tst *self, const cevent_tst *e);
const cevent_tst	*cqueue_get(cqueue_tst *self);
//uint32_t 			cqueue_is_empty(cqueue_tst *self);

int32_t cqueue_init(cqueue_tst *self, const cevent_tst **events, uint16_t max_event_count)
{
	assert(NULL != self);
	assert(NULL != events);
	assert(max_event_count > 0);

	self->events = events;
	self->max = max_event_count;
	self->head = 0;
	self->tail = 0;
	self->free = max_event_count;

	return 0;
}

int32_t cqueue_put(cqueue_tst *self, const cevent_tst *e)
{
	assert(NULL != self);

	if (0 == self->free)
	{
		return -1;
	}

	self->events[self->head] = e;
	self->head++;
	if (self->head >= self->max)
	{
		self->head = 0;
	}
	self->free--;

	return 0;
}

const cevent_tst *cqueue_get(cqueue_tst *self)
{
	const cevent_tst *e;

	assert(NULL != self);

	if (self->head == self->tail)
	{
		return NULL;
	}

	e = self->events[self->tail];
	self->tail++;
	if (self->tail >= self->max)
	{
		self->tail = 0;
	}
	self->free++;
	return e;
}

