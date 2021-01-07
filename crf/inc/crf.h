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

typedef struct route_tst route_tst;

struct route_tst
{
	uint16_t			sig;		// Signal to route
	cqueue_tst			**targets;	// NULL terminated list of pointers to target queues
};

typedef struct crf_tst crf_tst;

struct crf_tst
{
	void*				(*new_event)(crf_tst *self, uint32_t size, signal_t sig);		//< Create a new event
	void				(*publish)(crf_tst *self, const cevent_tst* e);			//< Publish an event
	void				(*post)(crf_tst *self, cevent_tst* e, cqueue_tst *q); //< Put an event into an event queue
	bool				(*step)(crf_tst *self); //< Dispatch one event

	chsm_tst			**chsm_ap;		//< Pointer to the array of objects in the application
	cpool_tst			*pool_ast;			//< Pointer to an array of memory pools.
	uint16_t			pool_cnt_u16;
};

bool crf_init(crf_tst *self , chsm_tst **chsm_ap, cpool_tst *pool_ast, uint16_t pool_cnt);

#define TYPEOF(SIGNAL) SIGNAL##_TYPE

#define CRF_NEW_EVENT(event_type, sig)  crf.new_event(&crf, sizeof(event_type), sig)
#define CRF_NEW(SIGNAL)					CRF_NEW_EVENT(TYPEOF(SIGNAL), SIGNAL)
#define CRF_POST(event_ptr, hsm_ptr) 	crf.post(&crf, (cevent_tst *)event_ptr, (cqueue_tst *)(hsm_ptr))
#define CRF_STEP()						crf.step(&crf)

extern crf_tst crf;

#endif /* INC_CRF_H_ */
