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


const CEvent chsm_init_event = {.sig=C_SIG_INIT, .gc_info=0};
const CEvent chsm_entry_event = {.sig=C_SIG_ENTRY, .gc_info=0};

uint16_t chsm_top(CHsm *this, const CEvent *e)
{
	return C_IGNORED();
}

void chsm_ctor(CHsm *this, chsm_state_t init_state)
{
	assert(NULL != this);
	assert(NULL != init_state);

	this->state_handler = init_state;
}

void chsm_init(CHsm *this)
{
	uint16_t result;

	assert(NULL != this);

	do {
		result = this->state_handler(this, &chsm_entry_event);
	} while (C_RET_HANDLED != result);
}

void chsm_dispatch(CHsm *this, const CEvent *e)
{
	uint16_t result;
	chsm_state_t prev_state;
	uint16_t i;

	assert(NULL != this);
	assert(NULL != e);

	for (i=CHSM_MAX_NESTING_LEVEL; i; i--)
	{
		prev_state = this->state_handler;

		result = this->state_handler(this, e);

		switch(result)
		{
		case C_RET_NONE: return;
		case C_RET_HANDLED: return;
		case C_RET_TRANS: return;
		}
	}

	//assert(i);
}
