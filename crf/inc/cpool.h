/*
 * event_pool.h
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#ifndef INC_CPOOL_H_
#define INC_CPOOL_H_

#include <stdint.h>
#include <stdbool.h>
#include "cevent.h"

typedef struct cpool_st
{
	void			*pool;	// Pointer to a user allocated buffer
	uint16_t		esize;	// Unit size of events in the pool
	uint16_t		ecnt;	// Max number of events in the pool
	uint16_t		free;	// Number of free events in the pool
	uint16_t		id;		// Pool id
} cpool_tst;

int32_t 	cpool_init(cpool_tst *self, void *buff, uint16_t event_size, uint16_t event_count, uint16_t id);
cevent_tst 	*cpool_new(cpool_tst *self);
bool 	    cpool_gc(cpool_tst *self, cevent_tst *e);


#endif /* INC_CPOOL_H_ */
