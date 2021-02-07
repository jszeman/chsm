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

	while(ev_pst = self->defer_q_st.get_right(&self->defer_q_st))
	{
		self->event_q_st.put_left(&self->event_q_st, (cevent_tst *)ev_pst);
	}
}

void chsm_exit_children(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst)
{
	chsm_user_func_tpft *exit_ppft;

	exit_ppft = ctx_pst->exit_stack_apft;

	for (int i=0; i<CHSM_MAX_NESTING_LEVEL; i++)
	{
		if (NULL != *exit_ppft)
		{
			(*exit_ppft)(self, e_pst);
		}
		else
		{
			return;
		}

		exit_ppft++;
	}
}

void chsm_dispatch(chsm_tst *self, const cevent_tst  *e_pst)
{
	assert(NULL != self);
	assert(NULL != e_pst);

	chsm_result_ten 	result_en;
	chsm_call_ctx_tst 	ctx_st = {0};

	ctx_st.exit_ppft = ctx_st.exit_stack_apft;
	ctx_st.start_pft = self->state_handler_pft;

	for (int i=0; i<CHSM_MAX_NESTING_LEVEL; i++)
	{
		result_en = self->state_handler_pft(self, e_pst, &ctx_st);

		switch(result_en)
		{
			case C_RES_HANDLED:
			case C_RES_IGNORED:
				/* In these cases the event was handled in an ancestor so we have to
				 * restore the original state then finish the event processing.
				 */
				self->state_handler_pft = ctx_st.start_pft;
				return;

			case C_RES_TRANS:
				/* The event resulted in a transition. Don't need to do anything else. */
				return;

			case C_RES_PARENT:
				/* The event was not handled by the current state, so it changed the
				 * state pointer to its parent. This means that we need to call
				 * the new state handler with the same event.
				 */
				break;

			case C_RES_GUARDS:
				/* The event was handled without transitioning into a new state,
				 * so the original event shall not be handled in the parents, but
				 * we still want to run all the guards.
				 */
				e_pst = &chsm_none_event_st;
				break;
		}
	}

	//TODO: error handling for nesting overflow	
}
