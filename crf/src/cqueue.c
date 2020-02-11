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

int32_t 			cqueue_init(CQueue *this, const CEvent **events, uint16_t max_event_count);
int32_t 			cqueue_put(CQueue *this, const CEvent *e);
const CEvent		*cqueue_get(CQueue *this);
//uint32_t 			cqueue_is_empty(CQueue *this);

int32_t cqueue_init(CQueue *this, const CEvent **events, uint16_t max_event_count)
{
	assert(NULL != this);
	assert(NULL != events);
	assert(max_event_count > 0);

	this->events = events;
	this->max = max_event_count;
	this->head = 0;
	this->tail = 0;
	this->free = max_event_count;

	return 0;
}

int32_t cqueue_put(CQueue *this, const CEvent *e)
{
	assert(NULL != this);

	if (0 == this->free)
	{
		return -1;
	}

	this->events[this->head] = e;
	this->head++;
	if (this->head >= this->max)
	{
		this->head = 0;
	}
	this->free--;

	return 0;
}

const CEvent *cqueue_get(CQueue *this)
{
	const CEvent *e;

	assert(NULL != this);

	if (this->head == this->tail)
	{
		return NULL;
	}

	e = this->events[this->tail];
	this->tail++;
	if (this->tail >= this->max)
	{
		this->tail = 0;
	}
	this->free++;
	return e;
}

