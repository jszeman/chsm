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

static void* new_event(crf_tst *self, uint32_t size)
{
    for(uint32_t i=0; i<CRF_MAX_POOL_COUNT; i++)
    {
        cpool_tst *pool = self->pool_ast+i;

        if (NULL == pool) return NULL;

        if (pool->esize >= size)
        {
            void* e = pool->new(pool);
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
    for (int i=0; i<self->pool_cnt_u16; i++)
    {
        if (self->pool_ast[i].gc(self->pool_ast+i, e))
        {
            return;
        }
    }

    return;
}

/*
 * TODO: implement publish/subscribe method as a fallback, when the application
 * does'n provide a send function for a state macine
 */
static void	publish(crf_tst *self, const cevent_tst* e)
{

}

static void	post(crf_tst *self, cevent_tst* e, cqueue_tst *q)
{
    e->gc_info.ref_cnt++;
    q->put(q, e);
}

static void	step(crf_tst *self)
{
    const cevent_tst *e_pst = NULL;
    chsm_tst **hsm_pst;

    for (hsm_pst = self->chsm_ap; *hsm_pst; hsm_pst++)
    {
        e_pst = (*hsm_pst)->event_q_st.get(&((*hsm_pst)->event_q_st));
        if (e_pst)
        {
            chsm_dispatch(*hsm_pst, e_pst);
        }
    }
}

static void	start(crf_tst *self)
{

}

bool crf_init(crf_tst *self , chsm_tst **chsm_ap, cpool_tst *pool_ast, uint16_t pool_cnt)
{
    if (NULL == self) return false;
    if (NULL == chsm_ap) return false;
    if (NULL == pool_ast) return false;

    self->new_event = new_event;
    self->gc = gc;
    self->publish = publish;
    self->post = post;
    self->step = step;
    self->chsm_ap = chsm_ap;
    self->pool_ast = pool_ast;
    self->pool_cnt_u16 = pool_cnt;

    return true;
}