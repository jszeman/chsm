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

int32_t cpool_init(CPool *this, void *buff, uint16_t event_size, uint16_t event_count, uint16_t id)
{
	assert(NULL != buff);
	assert(NULL != this);
	assert(0 != (id & CPOOL_ID_MASK));

	this->pool = buff;
	this->esize = event_size;
	this->ecnt = event_count;
	this->id = id;

	memset(this->pool, 0, (size_t)(this->esize * this->ecnt));

	return 0;
}

CEvent *cpool_new(CPool *this)
{
	void *e;

	assert(NULL != this);

	for (e = this->pool; e<this->pool + this->ecnt * this->esize; e+=this->esize)
	{
		if (0 == ((CEvent *)e)->gc_info)
		{
			((CEvent *)e)->gc_info = this->id;
			return (CEvent *)e;
		}
	}

	return NULL;
}

int32_t cpool_gc(CPool *this, CEvent *e)
{
	assert(NULL != this);
	assert(NULL != e);
	assert(this->id == (e->gc_info & CPOOL_ID_MASK));

	if (e->gc_info & CPOOL_REF_CNT_MASK)
	{
		e->gc_info--;
	}

	if (this->id == e->gc_info)
	{
		e->gc_info = 0;
	}

	return 0;
}

