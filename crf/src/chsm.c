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


const cevent_tst chsm_init_event_st = {.sig=C_SIG_INIT, .gc_info=0};

static void send(chsm_tst *self, const cevent_tst *e_pst)
{

}

void chsm_ctor(chsm_tst *self, chsm_state_tpft init_state_pft, const cevent_tst **events, uint16_t max_event_count)
{
	assert(NULL != self);
	assert(NULL != init_state_pft);

	self->send = send;
	self->state_handler_pft = init_state_pft;
	cqueue_init(&self->events_st, events, max_event_count);
}

void chsm_init(chsm_tst *self)
{
	uint16_t result_u16;

	assert(NULL != self);

	chsm_dispatch(self, &chsm_init_event_st);
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
				self->state_handler_pft = ctx_st.start_pft;
				return;

			case C_RES_TRANS:
				return;

			case C_RES_PARENT:
				break;
		}
	}

	//TODO: error handling for nesting overflow	
}
