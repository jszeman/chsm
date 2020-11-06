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

/* 
 * cqueue_put may be called from multiple threads (or from interrupt handlers)
 * so it has to be thread safe. C11 provides the necessary atomic operation but
 * to prepare for the cases when C11 is not available the application have to 
 * provide the definition for the atomic functions.
 */

#if (__STDC_VERSION__ >= 201112L)
	#ifndef __STDC_NO_ATOMICS__
		#include <stdatomic.h>
		#define atomic_fetch_add_u16(obj, value) atomic_fetch_add(obj, value)
		#define BUILTIN_ATOMICS
	#endif
#endif

#ifndef BUILTIN_ATOMICS
/* Atomically replaces the value pointed by obj with the result of addition of
 * value to the old value of obj, and returns the value obj held previously.
 * The operation is read-modify-write operation.
 */
uint16_t atomic_fetch_add_u16(volatile uint16_t *obj, uint16_t value);
#endif

/* Thread safety analysis:
 *
 * 1.
 * Interrupt here will not cause any problem. If no space left, it will
 * simply restore the state the previous line left. If there is more space
 * left then self->head will be increased by one, but this won't affect the
 * rest of the code in this function.
 *
 * 2.
 * Only cqueue_put should be called from interrupt and this function will
 * not modify self->tail and self->max so this line only depends on the result
 * of the fetch operation.
 *
 * 3.
 * If an interrupt happens here it will fail since at this point we know that
 * there is no more space in the queue. This means self->head will not be
 * changed.
 *
 * 4.
 * The function is about to return, so an interrupt here will not cause any
 * problem.
 *
 * 5.
 * Same as 1.
 */

static int32_t cqueue_put(cqueue_tst *self, const cevent_tst *e)
{
	uint16_t head;

	assert(NULL != self);

	head = atomic_fetch_add_u16(&self->head, 1);

	// 1.

	if (head - self->tail >= self->max)  // 2.
	{
	    // 3.
	    self->head = head;
	    // 4.

		return -1;
	}

	// 5.
	self->events[head & self->mask] = e;

	return 0;
}

static int32_t cqueue_put_left(cqueue_tst *self, const cevent_tst *e_cpst)
{
	assert(NULL != self);

	if ((uint16_t)(self->head - self->tail) >= self->max)
	{
		return -1;
	}

	self->tail--;
	self->events[self->tail & self->mask] = e_cpst;

	return 0;
}

static const cevent_tst *cqueue_get(cqueue_tst *self)
{
	const cevent_tst *e;

	assert(NULL != self);

	if (self->head == self->tail)
	{
		return NULL;
	}

	e = self->events[self->tail & self->mask];
	self->tail++;

	return e;
}

static const cevent_tst *cqueue_get_right(cqueue_tst *self)
{
	const cevent_tst *e;

	assert(NULL != self);

	if (self->head == self->tail)
	{
		return NULL;
	}

	self->head--;
	e = self->events[self->head & self->mask];

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
	self->mask = max_event_count-1;

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
