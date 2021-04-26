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

static void *cpool_new(cpool_tst *self)
{
    uint16_t head;

    assert(NULL != self);


    // Using for loop to make sure it terminates at max after self->ecnt steps.
    for (int i=0; i<self->ecnt; i++)
    {
        head = self->head;

        if (CPOOL_TERMINATOR == head) return NULL;

        /* Try to change self->head to the value in the cell it points to
         * if self->head still holds the value we read previously.
		 * If an interrupt here changes the value of self->head, the function call
		 * will fail and return false so the code inside the if can't modify the
		 * content of memory indexed by head.
         */
        if (atomic_compare_exchange_u16(&self->head, &head, *((uint16_t *)(self->pool + head))))
        {
            // At this point the change was successful, so we own the piece of memory indexed by head
            return (cevent_tst *)(self->pool + head);
        }
    }

    return NULL;
}

static bool cpool_gc(cpool_tst *self, const cevent_tst *e_pst)
{
    assert(NULL != self);
    assert(NULL != e_pst);

	uint8_t *e_pu8 = (uint8_t *)e_pst;

    if ((e_pu8 < self->pool) || (e_pu8 >= (self->pool + self->esize * self->ecnt))) return false; // The event is not from this pool

    uint16_t *p = (uint16_t *)e_pst;

    for (int i=0; i<self->ecnt; i++)
    {
        *p = self->head;

        if (atomic_compare_exchange_u16(&self->head, p, (uint8_t *)e_pst - self->pool))
        {
            return true;
        }
    }

    // Something went sideways
    return false;
}

void cpool_init(cpool_tst *self, uint8_t *buff, uint16_t event_size, uint16_t event_count)
{
    assert(NULL != buff);
    assert(NULL != self);

    self->pool = buff;
    self->esize = event_size;
    self->ecnt = event_count;

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
