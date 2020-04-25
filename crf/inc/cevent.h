#ifndef INC_CEVENT_H_
#define INC_CEVENT_H_

#include <stdint.h>

/**
 * Signal class registry:
 * 	0x0000 - 0x00ff:	HSM framework
 */

/** cevent - Base class for events
 *
 * Fields:
 * 		sig:		Event signal.
 * 		gc_info:	Used internally by CRF to keep track of the reference count
 * 					on the event, and also to identify the event pool that
 * 					handles the physical memory used by the event.
 */

typedef struct cevent_st
{
	uint16_t		sig;
	uint16_t		gc_info;
} cevent_tst;



#endif /* INC_CEVENT_H_ */
