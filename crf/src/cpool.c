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

#if (__STDC_VERSION__ >= 201112L)
    #ifndef __STDC_NO_ATOMICS__
        #include <stdatomic.h>
        #define atomic_compare_exchange_u16(obj, expected, desired) atomic_compare_exchange_weak(obj, expected, desired)
        #define BUILTIN_ATOMICS
    #endif
#endif

#ifndef BUILTIN_ATOMICS
/* Atomically replaces the value pointed by obj with the result of addition of
 * value to the old value of obj, and returns the value obj held previously.
 * The operation is read-modify-write operation.
 */
bool atomic_compare_exchange_u16(volatile uint16_t *obj, uint16_t *expected, uint16_t desired);
#endif


/* Only cpool_new should be implemented to be thread safe
 *
 */
static void *cpool_new(cpool_tst *self)
{
    uint16_t head;
    cevent_tst *e;

	assert(NULL != self);

	while(0xffff != self->head)
	{
	    head = self->head;

	    if (0xffff == head) return NULL;

	    /* Try to change self->head to the value in the cell it points to
	     * if self->head still holds the value we read previously.
	     * */
	    if (atomic_compare_exchange_u16(&self->head, &head, *((uint16_t *)(self->pool + head))))
	    {
	        e = (cevent_tst *)(self->pool + head);
	        e->gc_info = (gc_info_tst){.pool_id = self->id, .ref_cnt = 0};
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

	uint16_t *p = (uint16_t *)e;
	*p = self->head;
	self->head = (uint8_t *)e - self->pool;

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

	self->head = 0;

	/* Every block starts with the offset of the next free block, like
	 * a linked list with indices. Pointers are not used here because
	 * C11 is a bit fuzzy about the handling of atomic pointer types.
	 * They work with GCC as of now but best not to press it. C17 modified
	 * the wording for compare_exchange but let's make sure it works.
	 */
	for (int i=0; i<event_count; i++)
	{
		uint16_t *p = (uint16_t *)(buff + event_size * i);
		*p = event_size * (i + 1);
	}

    uint16_t *p = (uint16_t *)(buff + event_size * (event_count - 1));
    *p = 0xffff;
}
