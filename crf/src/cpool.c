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



int32_t cpool_init(cpool_tst *self, void *buff, uint16_t event_size, uint16_t event_count, uint16_t id)
{
	assert(NULL != buff);
	assert(NULL != self);
	assert(0 != (id & CPOOL_ID_MASK));
	assert(id < 16);

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
		if (0 == ((cevent_tst *)e)->gc_info.pool_id)
		{
			((cevent_tst *)e)->gc_info.pool_id = self->id;
			((cevent_tst *)e)->gc_info.ref_cnt = 0;
			return (cevent_tst *)e;
		}
	}

	return NULL;
}

bool cpool_gc(cpool_tst *self, cevent_tst *e)
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

