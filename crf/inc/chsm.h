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

#define CHSM_MAX_NESTING_LEVEL	4

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
} chsm_result_ten;

typedef struct chsm_st chsm_tst;
typedef struct chsm_call_ctx_st chsm_call_ctx_tst;

/*
 * This is the type for functions the generated state machine will call.
 */
typedef void (*chsm_user_func_tpft)(chsm_tst *self, const cevent_tst *e_pst);

/*
 * This is the type for functions that implement state behaviour.
 * Return:
 * 		C_RES_HANDLED: The event was handled by the state or one of its ancestors
 *		C_RES_IGNORED: The event was ignored since there was no event hander defined for it
 *					   in the state or any of its ancestors
 *		C_RES_TRANS:   The event was handled and resulted in a state transition
 * 		handler function to its parent state, and return a pointer for its own exit function.
 * Params:
 * 		self: Pointer to the state machine.
 *		e_pst: Pointer to the event to be handled.
 *		funcs_apft: A NULL terminated array of function pointers, that store the pointers to the exit functions of
 *			children states which could not handle the event.
 */
typedef chsm_result_ten (*chsm_state_tpft)(chsm_tst *self, const cevent_tst *e_pst, chsm_call_ctx_tst *ctx_pst);

/*
 * Call context structure
 * A pointer to such a structure will be passed to state handler functions.
 * In case a state handler wants to defer event processing to its parent, it should write
 * the pointer to the exit function in *exit_pft, then increase it.
 * If the parent state handler executes a state transition, it should call the functions from
 * the exit_stack_apft array.
 */
typedef struct chsm_call_ctx_st
{
	chsm_user_func_tpft		exit_stack_apft[CHSM_MAX_NESTING_LEVEL];
	chsm_user_func_tpft		*exit_ppft;
	chsm_state_tpft			start_pft;
} chsm_call_ctx_tst;

/*
 * State machine
 */
struct chsm_st
{
	cqueue_tst			event_q_st;
	cqueue_tst			defer_q_st;

	chsm_state_tpft		state_handler_pft;

	/** send
	 * 		The state machine implementation shall call this function, when an
	 *		event was created and need to be sent to the other parts of the system.
	 *		This function should distribute the event to all queues in the application
	 *		that may need it by calling theri put method.
	 */
	void				(*send)(chsm_tst *self, const cevent_tst *e_pst);
};

void chsm_ctor(chsm_tst *self, chsm_state_tpft init_state_pft,
			   const cevent_tst **events, uint16_t event_q_len, uint16_t defer_q_len);
void chsm_init(chsm_tst *self);
void chsm_dispatch(chsm_tst *self, const cevent_tst *e_pst);
void chsm_defer(chsm_tst *self, const cevent_tst *e_pst);
void chsm_recall(chsm_tst *self, const cevent_tst *e_pst);

void chsm_exit_children(chsm_tst *self, const cevent_tst  *e_pst, chsm_call_ctx_tst *ctx_pst);

extern const cevent_tst chsm_init_event_st;

static inline chsm_result_ten chsm_handle_in_parent(chsm_tst *self, chsm_call_ctx_tst *ctx_pst, chsm_state_tpft parent, void *exit_func)
{
	self->state_handler_pft = parent;
	if (exit_func)
	{
		*(ctx_pst->exit_ppft) = (chsm_user_func_tpft)exit_func;
		ctx_pst->exit_ppft++;
	}
    return C_RES_PARENT;
}

static inline chsm_result_ten chsm_transition(chsm_tst *self, chsm_state_tpft target)
{
	self->state_handler_pft = target;

    return C_RES_TRANS;
}

static inline chsm_result_ten chsm_ignored(chsm_tst *self)
{
    return C_RES_IGNORED;
}

static inline chsm_result_ten chsm_handled(chsm_tst *self)
{
    return C_RES_HANDLED;
}

#endif /* INC_CHSM_H_ */
