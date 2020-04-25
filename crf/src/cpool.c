/*
 * event_pool.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "crf.h"

#define CPOOL_ID_MASK		0xF000
#define CPOOL_REF_CNT_MASK	(~CPOOL_ID_MASK)

int32_t cpool_init(cpool_tst *self, void *buff, uint16_t event_size, uint16_t event_count, uint16_t id)
{
	assert(NULL != buff);
	assert(NULL != self);
	assert(0 != (id & CPOOL_ID_MASK));

	self->pool = buff;
	self->esize = event_size;
	self->ecnt = event_count;
	self->id = id;

	memset(self->pool, 0, (size_t)(self->esize * self->ecnt));

	return 0;
}

cevent_tst *cpool_new(cpool_tst *self)
{
	void *e;

	assert(NULL != self);

	for (e = self->pool; e<self->pool + self->ecnt * self->esize; e+=self->esize)
	{
		if (0 == ((cevent_tst *)e)->gc_info)
		{
			((cevent_tst *)e)->gc_info = self->id;
			return (cevent_tst *)e;
		}
	}

	return NULL;
}

int32_t cpool_gc(cpool_tst *self, cevent_tst *e)
{
	assert(NULL != self);
	assert(NULL != e);
	assert(self->id == (e->gc_info & CPOOL_ID_MASK));

	if (e->gc_info & CPOOL_REF_CNT_MASK)
	{
		e->gc_info--;
	}

	if (self->id == e->gc_info) // Ref counter is 0
	{
		e->gc_info = 0;
	}

	return 0;
}

