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

static void *cpool_new(cpool_tst *self)
{
    uint8_t *e;

	assert(NULL != self);

	if (NULL == self->head) return NULL;

	e = self->head;

	self->head = *((uint8_t **)e);

	((cevent_tst *)e)->gc_info = (gc_info_tst){.pool_id = self->id, .ref_cnt = 0};

	return e;
}

static bool cpool_gc(cpool_tst *self, cevent_tst *e)
{
	assert(NULL != self);
	assert(NULL != e);
	
	if (self->id != e->gc_info.pool_id)
	{
		return false;
	}

	uint8_t **p = (uint8_t **)e;
	*p = self->head;
	self->head = (uint8_t *)e;

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

	self->head = buff;

	/* Use the begining of each block as a pointer and make a linked list from
	 * the items.
	 * Do not touch the last element so it becomes a NULL pointer;
	 */
	for (int i=0; i<(event_count - 1); i++)
	{
		uint8_t **p = (uint8_t **)(buff + event_size * i);
		*p = buff + event_size * (i + 1);
	}
}
