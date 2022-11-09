/*
 * hsm.h
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#ifndef INC_CHSM_H_
#define INC_CHSM_H_

#include <stdint.h>

#include "cevent.h"
#include "cqueue.h"

#define CHSM_MAX_NESTING_LEVEL	8

typedef enum
{
	C_SIG_NONE,
	C_SIG_INIT,
} chsm_signals_ten;

typedef enum
{
	C_RES_HANDLED,
	C_RES_TRANS,
	C_RES_PARENT,
	C_RES_IGNORED,
	C_RES_GUARDS,
} chsm_result_ten;

typedef struct chsm_st chsm_tst;

/*
 * This is the type for functions the generated state machine will call.
 */
typedef void (*chsm_user_func_tpft)(chsm_tst *self, const cevent_tst *e_pst);

/*
 * This is the type for functions that implement state behaviour. Used only internally by
 * the CRF library.
 * Return:
 * 		C_RES_HANDLED: The event was handled by the state or one of its ancestors
 *		C_RES_IGNORED: The event was ignored since there was no event hander defined for it
 *					   in the state or any of its ancestors
 *		C_RES_TRANS:   The event was handled and resulted in a state transition
 * 		handler function to its parent state, and return a pointer for its own exit function.
 * Params:
 * 		self: Pointer to the state machine.
 *		e_pst: Pointer to the event to be handled.
 *		ctx_pst: Pointer to a call context that stores some information about the states involved
 *			in the event processing.
 */
typedef chsm_result_ten (*chsm_state_tpft)(chsm_tst *self, const cevent_tst *e_pst);

/*
 * Call context structure
 * A pointer to such a structure will be passed to state handler functions.
 * In case a state handler wants its parent to handle an event, it should write
 * the pointer to the exit function in *exit_pft, then increase it.
 * If the parent state handler executes a state transition, it should call the functions from
 * the exit_stack_apft array to properly exit the currently active state.
 */
struct chsm_call_ctx_st
{
	chsm_user_func_tpft		exit_stack_apft[CHSM_MAX_NESTING_LEVEL];
	chsm_user_func_tpft		*exit_ppft;
	chsm_state_tpft			start_pft;
};

/*
 * State machine
 */
struct chsm_st
{
	cqueue_tst			event_q_st;
	cqueue_tst			defer_q_st;

	chsm_state_tpft		state_handler_pft;

	/** send
	 *		This function should be implemented by the application.
	 *
	 *		Params:
	 *			self:	Pointer to the state machine instance
	 *			e_pst:	Pointer to the event to be sent
	 *		Expected operation:
	 *			The function should post the event to all state machines 
	 *			that need it for their operation.
	 *
	 * 		The state machine implementation shall call this function, when an
	 *		event was created and need to be sent to the other parts of the system.
	 *		This function should distribute the event to all queues in the application
	 *		that may need it by calling their put method.
	 */
	void				(*send)(chsm_tst *self, const cevent_tst *e_pst);

	/** next
	 * 		If a state machine has embedded state machine(s), the next pointers
	 * 		shall be used to link them into list, so the framework can step
	 * 		throgh them.
	 */
	chsm_tst*			next;
};

void chsm_ctor(chsm_tst *self, chsm_state_tpft init_state_pft,
			   const cevent_tst **events, uint16_t event_q_len, uint16_t defer_q_len);
void chsm_init(chsm_tst *self);
void chsm_dispatch(chsm_tst *self, const cevent_tst *e_pst);
void chsm_defer(chsm_tst *self, const cevent_tst *e_pst);
void chsm_recall(chsm_tst *self, const cevent_tst *e_pst);

extern const cevent_tst chsm_init_event_st;

static inline chsm_result_ten chsm_transition(chsm_tst *self, chsm_state_tpft target)
{
	self->state_handler_pft = target;

    return C_RES_TRANS;
}

static inline chsm_result_ten chsm_ignored(chsm_tst *self)
{
    (void)self;
    return C_RES_IGNORED;
}

static inline chsm_result_ten chsm_handled(chsm_tst *self)
{
    (void)self;
    return C_RES_HANDLED;
	
}

#endif /* INC_CHSM_H_ */
