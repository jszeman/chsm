/*
 * hsm.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */
#include <assert.h>
#include <cevent.h>
#include <chsm.h>
#include <stdlib.h>


const cevent_tst chsm_init_event_st = {.sig=C_SIG_INIT, .gc_info={0}};
const cevent_tst chsm_none_event_st = {.sig=C_SIG_NONE, .gc_info={0}};

static void send(chsm_tst *self, const cevent_tst *e_pst)
{
	(void)self;
	(void)e_pst;
}

void chsm_ctor(chsm_tst *self, chsm_state_tpft init_state_pft, const cevent_tst **events, uint16_t event_q_len, uint16_t defer_q_len)
{
	assert(NULL != self);
	assert(NULL != init_state_pft);

	self->send = send;
	self->next = NULL;
	self->state_handler_pft = init_state_pft;
	cqueue_init(&self->event_q_st, events, event_q_len);
	cqueue_init(&self->defer_q_st, events+event_q_len, defer_q_len);
}

void chsm_init(chsm_tst *self)
{
	assert(NULL != self);

	chsm_dispatch(self, &chsm_init_event_st);
}


void chsm_defer(chsm_tst *self, const cevent_tst *e_pst)
{
	self->defer_q_st.put(&self->defer_q_st, (cevent_tst *)e_pst);
}

void chsm_recall(chsm_tst *self, const cevent_tst *e_pst)
{
	const cevent_tst *ev_pst;

    (void)e_pst;

	while((ev_pst = self->defer_q_st.get_right(&self->defer_q_st)))
	{
		self->event_q_st.put_left(&self->event_q_st, (cevent_tst *)ev_pst);
	}
}

void chsm_dispatch(chsm_tst *self, const cevent_tst  *e_pst)
{
	assert(NULL != self);
	assert(NULL != e_pst);

	self->state_handler_pft(self, e_pst);
}
