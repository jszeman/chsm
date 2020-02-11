/*
 * event_pool.h
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#ifndef INC_CPOOL_H_
#define INC_CPOOL_H_

#include <stdint.h>
#include "cevent.h"

typedef struct crf_event_pool_t
{
	void			*pool;
	uint16_t		esize;
	uint16_t		ecnt;
	uint16_t		free;
	uint16_t		id;
} CPool;

int32_t cpool_init(CPool *this, void *buff, uint16_t event_size, uint16_t event_count, uint16_t id);
CEvent *cpool_new(CPool *this);
int32_t cpool_gc(CPool *this, CEvent *e);


#endif /* INC_CPOOL_H_ */
