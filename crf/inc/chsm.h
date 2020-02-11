/*
 * hsm.h
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#ifndef INC_CHSM_H_
#define INC_CHSM_H_

#include <cevent.h>
#include <stdint.h>

#define CHSM_MAX_NESTING_LEVEL	4

enum chsm_events
{
	C_SIG_NONE,
	C_SIG_INIT,
	C_SIG_ENTRY,
	C_SIG_EXIT,
};

enum chsm_result
{
	C_RET_NONE,
	C_RET_HANDLED,
	C_RET_SUPER,
	C_RET_ENTRY,
	C_RET_EXIT,
	C_RET_TRANS,
};

typedef struct crf_hsm_t CHsm;

typedef uint16_t (*chsm_state_t)(CHsm *this, const CEvent *e);

typedef struct crf_hsm_t
{
	chsm_state_t		state_handler;
} CHsm;

void chsm_ctor(CHsm *this, chsm_state_t init_state);
void chsm_init(CHsm *this);
void chsm_dispatch(CHsm *this, const CEvent *e);

#define C_HANDLED()			C_RET_HANDLED
#define C_ENTRY(_child)		(this->state_handler = _child, 	C_RET_ENTRY)
#define C_EXIT()			(this->state_handler = NULL, 	C_RET_EXIT)
#define C_SUPER(_super)		(this->state_handler = _super, 	C_RET_SUPER)
#define C_TRANS(_target)	(this->state_handler = _target, C_RET_TRANS)
#define C_IGNORED()			C_RET_NONE

extern const CEvent chsm_init_event;
extern const CEvent chsm_entry_event;

#endif /* INC_CHSM_H_ */
