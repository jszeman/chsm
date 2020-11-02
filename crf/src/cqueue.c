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

static int32_t cqueue_put(cqueue_tst *self, const cevent_tst *e)
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

static int32_t cqueue_put_left(cqueue_tst *self, const cevent_tst *e_cpst)
{
	assert(NULL != self);

	if (0 == self->free)
	{
		return -1;
	}

	self->tail--;
	if (self->tail >= self->max)
	{
		self->tail = self->max-1;
	}
	self->events[self->tail] = e_cpst;
	self->free--;

	return 0;
}

static const cevent_tst *cqueue_get(cqueue_tst *self)
{
	const cevent_tst *e;

	assert(NULL != self);

	if (self->free == self->max)
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

static const cevent_tst *cqueue_get_right(cqueue_tst *self)
{
	const cevent_tst *e;

	assert(NULL != self);

	if (self->free == self->max)
	{
		return NULL;
	}

	self->head--;
	if (self->head >= self->max)
	{
		self->head = self->max-1;
	}

	e = self->events[self->head];

	self->free++;
	return e;
}

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

	self->put = cqueue_put;
	self->put_left = cqueue_put_left;
	self->get = cqueue_get;
	self->get_right = cqueue_get_right;

#if 0 // just to test orthogonality
	self->put_left = cqueue_put;
	self->put = cqueue_put_left;
	self->get_right = cqueue_get;
	self->get = cqueue_get_right;
#endif

	return 0;
}
