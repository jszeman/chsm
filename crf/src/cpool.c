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
#include <stdio.h>
#include "atomic_ops.h"

#define CPOOL_TERMINATOR 0xffff


/* Only cpool_new should be called from interrupt context, so only
 * this function is implemented to be threaf safe.
 */
static void *cpool_new(cpool_tst *self)
{
    uint16_t head;
    cevent_tst *e;

    assert(NULL != self);


    // Using for loop to make sure it terminates at max after self->ecnt steps.
    for (int i=0; i<self->ecnt; i++)
    {
        head = self->head;

        if (CPOOL_TERMINATOR == head) return NULL;
        /* Try to change self->head to the value in the cell it points to
         * if self->head still holds the value we read previously.
         */
        if (atomic_compare_exchange_u16(&self->head, &head, *((uint16_t *)(self->pool + head))))
        {
            
            // At this point the change was successful, so we own the piec of memory indexed by head
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
    *p = CPOOL_TERMINATOR;
}
