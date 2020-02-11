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

typedef struct crf_object_t
{
	CQueue			events;
	CHsm			hsm;
} CObject;

typedef struct crf_t
{
	CObject			*objects;		//< Pointer to the array of objects in the application
	CPool			*pools;			//< Pointer to an array of memory pools.
} Cfr;

int32_t 		cfr_init(Cfr *cfr, CObject *objects, CPool *pools);
CEvent 			*cfr_event_new(Cfr *cfr);
int32_t			cfr_event_del(Cfr *cfr, const CEvent *e);
int32_t			cfr_event_post(const CEvent *e, CObject *object);
const CEvent	*cfr_event_get(CPool *pool);
int32_t			cfr_run(Cfr *cfr);

#endif /* INC_CRF_H_ */
