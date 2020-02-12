/*
 * canopen_test.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "unity_fixture.h"
#include "crf.h"


TEST_GROUP(ep);

#define BUFF_SIZE 	32

CPool 				pool;
uint8_t				buff[BUFF_SIZE];

TEST_SETUP(ep)
{

}

TEST_TEAR_DOWN(ep)
{
}

/**
 * Allocate just one event and assert that:
 * 	- it is not NULL
 * 	- it is within the pool
 */
TEST(ep, new_1)
{
	CEvent *e;

	cpool_init(&pool, buff, 8, 4, 0x1000);
	e = cpool_new(&pool);
	TEST_ASSERT(NULL != e);
	TEST_ASSERT((uint8_t *)e >= (uint8_t *)buff);
	TEST_ASSERT((uint8_t *)e <= (uint8_t *)buff + 8*3);
}

/**
 * Allocate just 2 events and assert that:
 * 	- they are not NULL
 * 	- they are within the pool
 * 	- they are not the same
 */
TEST(ep, new_2)
{
	CEvent *e[2];

	cpool_init(&pool, buff, 8, 4, 0x1000);
	e[0] = cpool_new(&pool);
	e[1] = cpool_new(&pool);
	TEST_ASSERT(NULL != e[0]);
	TEST_ASSERT((uint8_t *)e[0] >= (uint8_t *)buff);
	TEST_ASSERT((uint8_t *)e[0] <= (uint8_t *)buff + 8*3);
	TEST_ASSERT(NULL != e[1]);
	TEST_ASSERT((uint8_t *)e[1] >= (uint8_t *)buff);
	TEST_ASSERT((uint8_t *)e[1] <= (uint8_t *)buff + 8*3);
	TEST_ASSERT_NOT_EQUAL(e[0], e[1]);
}

/**
 * Allocate 5 events from a pool of 4 and assert that
 * the fifth allocation returns NULL
 */
TEST(ep, overallocate)
{
	CEvent *e;

	cpool_init(&pool, buff, 8, 4, 0x1000);

	for (uint16_t i=0; i<5; i++)
	{
		e = cpool_new(&pool);
	}

	TEST_ASSERT(NULL == e);
}

/**
 * Allocate all events from the pool, perform garbage collection
 * on one element and check that now cpool_new will be successful
 */
TEST(ep, reuse_event)
{
	CEvent *e[6];
	CEvent *e3;

	cpool_init(&pool, buff, 8, 4, 0x1000);

	for (uint16_t i=0; i<5; i++)
	{
		e[i] = cpool_new(&pool);
	}

	TEST_ASSERT(NULL == e[4]);
	e3 = e[3];

	//printf("\ngc_info: %x\n", e3->gc_info);

	cpool_gc(&pool, e[3]);

	e[5] = cpool_new(&pool);

	TEST_ASSERT_EQUAL_HEX32(e3, e[5]);
}

TEST_GROUP_RUNNER(ep)
{
	RUN_TEST_CASE(ep, new_1);
	RUN_TEST_CASE(ep, new_2);
	RUN_TEST_CASE(ep, overallocate);
	RUN_TEST_CASE(ep, reuse_event);
//	RUN_TEST_CASE(ep, new_1);
//	RUN_TEST_CASE(ep, new_1);
//	RUN_TEST_CASE(ep, new_1);
//	RUN_TEST_CASE(ep, new_1);
//	RUN_TEST_CASE(ep, new_1);
//	RUN_TEST_CASE(ep, new_1);
//	RUN_TEST_CASE(ep, new_1);
//	RUN_TEST_CASE(ep, new_1);
//	RUN_TEST_CASE(ep, new_1);
//	RUN_TEST_CASE(ep, new_1);
//	RUN_TEST_CASE(ep, new_1);
//	RUN_TEST_CASE(ep, new_1);
//	RUN_TEST_CASE(ep, new_1);
}
