/*
 * event_pool.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "crf.h"



// TODO: Make this thread safe. Basic idea:
//		1. Set pool_id
//		2. Increment gc_info atomically
//		3. Read ref_cnt:
//			if 1: Success, return the event
//			else: decrease ref_cnt and continue search
//
//		The second case can only happen if the main thread is in the if inside
//		the loop, but an interrupt have already allocated and sent the event.
static void *cpool_new(cpool_tst *self)
{
    uint8_t *e;

	assert(NULL != self);

	for (e = self->pool; e<self->pool + self->ecnt * self->esize; e+=self->esize)
	{
		if (0 == ((cevent_tst *)e)->gc_info.pool_id)
		{
			((cevent_tst *)e)->gc_info.pool_id = self->id;
			((cevent_tst *)e)->gc_info.ref_cnt = 0;
			return e;
		}
	}

	return NULL;
}

static bool cpool_gc(cpool_tst *self, cevent_tst *e)
{
	assert(NULL != self);
	assert(NULL != e);
	
	if (self->id != e->gc_info.pool_id)
	{
		return false;
	}

	if (e->gc_info.ref_cnt)
	{
		e->gc_info.ref_cnt--;
	}
	else
	{
		e->gc_info.pool_id = 0;
	}

	return true;
}

void cpool_init(cpool_tst *self, uint8_t *buff, uint16_t event_size, uint16_t event_count, uint16_t id)
{
	assert(NULL != buff);
	assert(NULL != self);
	assert(id < 16);

	self->pool = buff;
	self->esize = event_size;
	self->ecnt = event_count;
	self->id = id;

	self->new = cpool_new;
	self->gc = cpool_gc;

	memset(self->pool, 0, (size_t)(self->esize * self->ecnt));
}
