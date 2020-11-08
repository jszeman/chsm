/*
 * canopen_test.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "unity_fixture.h"
#include "crf.h"


TEST_GROUP(ep);

#define BUFF_SIZE 	32

cpool_tst  				pool;
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
	cevent_tst  *e;

	cpool_init(&pool, buff, 8, 4);
	e = pool.new(&pool);
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
	cevent_tst  *e[2];

	cpool_init(&pool, buff, 8, 4);
	e[0] = pool.new(&pool);
	e[1] = pool.new(&pool);
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
	cevent_tst  *e;

	cpool_init(&pool, buff, 8, 4);

	for (uint16_t i=0; i<5; i++)
	{
		e = pool.new(&pool);
	}

	TEST_ASSERT(NULL == e);
}

/**
 * Allocate all events from the pool, perform garbage collection
 * on one element and check that now cpool_new will be successful
 */
TEST(ep, reuse_event)
{
	cevent_tst  *e[6];
	cevent_tst  *e3;
	bool gc_result_b;

	cpool_init(&pool, buff, 8, 4);

	for (uint16_t i=0; i<5; i++)
	{
		e[i] = pool.new(&pool);
	}

	TEST_ASSERT(NULL == e[4]);
	e3 = e[3];

	//printf("\ngc_info: %x\n", e3->gc_info);

	gc_result_b = pool.gc(&pool, e[3]);

	e[5] = pool.new(&pool);

	TEST_ASSERT_EQUAL(e3, e[5]);
	TEST_ASSERT_TRUE(gc_result_b);
}

/**
 * Call cpool_gc on an event with a different id and check
 * that it returns 0;
 */
TEST(ep, ignore_foreign_event)
{
	cevent_tst  e = {.sig = 0xabcd, .gc_info = {.pool_id = 0, .ref_cnt = 10}};
	bool gc_result_b;

	cpool_init(&pool, buff, 8, 4);

	gc_result_b = pool.gc(&pool, &e);
	TEST_ASSERT_FALSE(gc_result_b);

	e.gc_info.pool_id = 2;
	e.gc_info.ref_cnt = 5;

	gc_result_b = pool.gc(&pool, &e);
	TEST_ASSERT_FALSE(gc_result_b);
}

TEST_GROUP_RUNNER(ep)
{
	RUN_TEST_CASE(ep, new_1);
	RUN_TEST_CASE(ep, new_2);
	RUN_TEST_CASE(ep, overallocate);
	RUN_TEST_CASE(ep, reuse_event);
	RUN_TEST_CASE(ep, ignore_foreign_event);
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
