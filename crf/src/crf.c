#include <stdint.h>
#include <stdbool.h>

#include "cevent.h"
#include "cpool.h"
#include "cqueue.h"
#include "chsm.h"
#include "crf.h"
#include <assert.h>
#include <stdio.h>

#define CRF_POOL_CNT_MAX 4

static cevent_tst* new_event(crf_tst *self, uint32_t size)
{
    for(uint32_t i=0; i<CRF_MAX_POOL_COUNT; i++)
    {
        cpool_tst *pool = self->pool_ast+i;

        if (NULL == pool) return NULL;

        if (pool->esize >= size)
        {
            cevent_tst* e = cpool_new(pool);
            if (NULL != e)
            {
                return e;
            }
        }
    }

    return NULL;
}

static void	gc(crf_tst *self, cevent_tst* e)
{

    printf("\nGC info: %d\n", e->gc_info);
    for (int i=0; i<self->pool_cnt_u16; i++)
    {
        if (cpool_gc(self->pool_ast+i, e))
        {
            return;
        }
    }

    return;
}

bool crf_init(crf_tst *self , chsm_tst **chsm_ap, cpool_tst *pool_ast, uint16_t pool_cnt)
{
    if (NULL == self) return false;
    if (NULL == chsm_ap) return false;
    if (NULL == pool_ast) return false;

    self->new_event = new_event;
    self->gc = gc;
    self->chsm_ap = chsm_ap;
    self->pool_ast = pool_ast;
    self->pool_cnt_u16 = pool_cnt;

    return true;
}