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

#include "cevent.h"
#include "cpool.h"
#include "cqueue.h"
#include "chsm.h"

typedef struct cobject_st
{
	cqueue_tst			events;
	chsm_tst			hsm;
} cobject_tst;

typedef struct crf_st
{
	cobject_tst			*objects;		//< Pointer to the array of objects in the application
	cpool_tst			*pools;			//< Pointer to an array of memory pools.
} crf_tst;

int32_t 			crf_init(crf_tst *self , cobject_tst *objects, cpool_tst  *pools);
cevent_tst  		_crf_event_new(crf_tst *self );
int32_t				crf_event_del(crf_tst *self , const cevent_tst  *e);
int32_t				crf_event_post(const cevent_tst  *e, cobject_tst *object);
const cevent_tst 	_crf_event_get(cpool_tst  *pool);
int32_t				crf_run(crf_tst *self );

#endif /* INC_CRF_H_ */
