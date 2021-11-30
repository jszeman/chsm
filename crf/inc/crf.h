/*
 * cr.h
 *
 *  Created on: 2019. jan. 31.
 *      Author: jszeman
 *
 * C Active Object System
 */

#ifndef INC_CRF_H_
#define INC_CRF_H_

#include <stdint.h>
#include <stdbool.h>

#include "cevent.h"
#include "cpool.h"
#include "cqueue.h"
#include "chsm.h"

#define CRF_MAX_POOL_COUNT 4

typedef struct crf_tst crf_tst;

struct crf_tst
{
	void*				(*new_event)(crf_tst *self, uint32_t size, signal_t sig);
	void				(*publish)(crf_tst *self, const cevent_tst* e);
	void				(*post)(crf_tst *self, cevent_tst* e, cqueue_tst *q);
	bool				(*step)(crf_tst *self);
	void				(*gc)(crf_tst *self, const cevent_tst* e);


	chsm_tst			**chsm_ap;		//< Pointer to the array of objects in the application
	cpool_tst			*pool_ast;			//< Pointer to an array of memory pools.
	uint16_t			pool_cnt_u16;
};

bool crf_init(crf_tst *self , chsm_tst **chsm_ap, cpool_tst *pool_ast, uint16_t pool_cnt);

#define TYPEOF(SIGNAL) SIGNAL##_TYPE

#define CRF_NEW_EVENT(event_type, sig)  crf.new_event(&crf, sizeof(event_type), sig)
#define CRF_NEW(SIGNAL)					CRF_NEW_EVENT(TYPEOF(SIGNAL), SIGNAL)
#define CRF_POST(event_ptr, queue_ptr) 	crf.post(&crf, (cevent_tst *)event_ptr, (cqueue_tst *)(queue_ptr))
#define CRF_POST_TO_SELF(event_ptr) 	((cqueue_tst *)(self))->put_left((cqueue_tst *)(self), event_ptr)
#define CRF_STEP()						crf.step(&crf)
#define CRF_EMIT(event_ptr)				((chsm_tst *)self)->send((chsm_tst *)self, (const cevent_tst *)event_ptr)
#define CRF_GC(event_ptr)				crf.gc(&crf, (cevent_tst *)event_ptr)
#define CRF_SEND_FUNC(hsm_pst)			((chsm_tst *)hsm_pst)->send
#define CRF_SIG_VAR(SIGNAL, VAR, E_PST)	TYPEOF(SIGNAL)* VAR = (TYPEOF(SIGNAL)*)E_PST
#define CRF_SET_SEND_FUNC(hsm_pst, func) CRF_SEND_FUNC(hsm_pst) = (void (*)(chsm_tst *, const cevent_tst *))func

#define CRF_UNUSED(p) (void)(p)

extern crf_tst crf;

#endif /* INC_CRF_H_ */
