/*
 * canopen_test.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#include <cqueue.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "unity_fixture.h"

TEST_GROUP(cq);

cqueue_tst 	eq;
cevent_tst  *events[8];

TEST_SETUP(cq)
{
	for (int i=0; i<8; i++)
	{
		events[i] = (cevent_tst *)0xcafebabe;
	}
}

TEST_TEAR_DOWN(cq)
{
	// Make sure the no out-of-bounds access happens
	for (int i=4; i<8; i++)
	{
		TEST_ASSERT(events[i] == (cevent_tst *)0xcafebabe);
	}
}

/**
 * Test that we can get back one event from the queue
 */
TEST(cq, put_1_get_1)
{
	cevent_tst  e;
	const cevent_tst  *ep;
	int32_t r;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	r = eq.put(&eq, &e);
	ep = eq.get(&eq);

	TEST_ASSERT_EQUAL(0, r);
	TEST_ASSERT(ep == &e);
}

/**
 * Test that we can get back the first event from the queue,
 * when 2 was put in
 */
TEST(cq, put_2_get_1)
{
	cevent_tst  e1, e2;
	const cevent_tst  *ep;
	int32_t r1, r2;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	r1 = eq.put(&eq, &e1);
	r2 = eq.put(&eq, &e2);
	ep = eq.get(&eq);

	TEST_ASSERT_EQUAL(0, r1);
	TEST_ASSERT_EQUAL(0, r2);
	TEST_ASSERT(ep == &e1);
}

/**
 * Put 2 events in the queue, get 2, then check addresses
 * and order.
 */
TEST(cq, put_2_get_2)
{
	cevent_tst  e1, e2;
	const cevent_tst  *ep1, *ep2;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	eq.put(&eq, &e1);
	eq.put(&eq, &e2);
	ep1 = eq.get(&eq);
	ep2 = eq.get(&eq);

	TEST_ASSERT(ep1 == &e1);
	TEST_ASSERT(ep2 == &e2);
}


/**
 * Check that we get NULL when trying to get from empty queue.
 */
TEST(cq, get_from_empty)
{
	const cevent_tst  *ep1;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	ep1 = eq.get(&eq);

	TEST_ASSERT(NULL == ep1);
}

/**
 * Check that cqueue_put returns -1 when there are no more slots.
 */
TEST(cq, put_more_than_capacity)
{
	cevent_tst  e1;
	int32_t r;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	for (uint16_t i=0; i<5; i++)
	{
		r = eq.put(&eq, &e1);
	}

	TEST_ASSERT(-1 == r);
	TEST_ASSERT_EQUAL(1, eq.fault_cnt);
}

/**
 * Check that we can get an element even if the queue is full
 */
TEST(cq, get_all)
{
	cevent_tst  e[4];
	const cevent_tst  *ep[4];

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	for (uint16_t i=0; i<4; i++)
	{
		eq.put(&eq, &e[i]);
	}

	for (uint16_t i=0; i<4; i++)
	{
		ep[i] = eq.get(&eq);

		TEST_ASSERT(ep[i] == &e[i]);
	}

	
}

/**
 * Check that put will roll over, when more events were put/get from
 * the queue.
 */
TEST(cq, test_put_roll)
{
	cevent_tst  e1;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);

	for (uint16_t i=0; i<5; i++)
	{
		eq.put(&eq, &e1);
		eq.get(&eq);
	}

	TEST_ASSERT((cevent_tst *)0xcafebabe == events[4]);
}

/**
 * Check that put/get will work with many operations. (The implementation
 * uses uint16_t indices so it may fail on rollover.)
 */
TEST(cq, test_put_a_lot)
{
	cevent_tst  e1;
	cevent_tst  e2;
	cevent_tst  e3;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);

	for (uint32_t i=0; i<500000; i++)
	{
		TEST_ASSERT(0 == eq.put(&eq, &e1));
		TEST_ASSERT(0 == eq.put(&eq, &e2));
		TEST_ASSERT(0 == eq.put(&eq, &e3));
		TEST_ASSERT(&e1 == eq.get(&eq));
		TEST_ASSERT(&e2 == eq.get(&eq));
		TEST_ASSERT(&e3 == eq.get(&eq));
	}
}

/**
 * Check that get will roll over, when more events were put/get from
 * the queue.
 */
TEST(cq, test_get_roll)
{
	cevent_tst  e1;
	const cevent_tst  *ep1;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);

	for (uint16_t i=0; i<5; i++)
	{
		eq.put(&eq, &e1);
		ep1 = eq.get(&eq);
	}

	TEST_ASSERT(&e1 == ep1);
}

/**
 * Check that put_left will allow getting the pointer back.
 */
TEST(cq, put_left_1_get_1)
{
	cevent_tst  e;
	const cevent_tst  *ep;
	int32_t r;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	r = eq.put_left(&eq, &e);
	ep = eq.get(&eq);

	TEST_ASSERT_EQUAL(0, r);
	TEST_ASSERT(ep == &e);
}

/**
 * Check that put_left will result in a stack like operation,
 * when the items are read back in reverse order.
 */
TEST(cq, put_left_2_get_2)
{
	cevent_tst  e1, e2;
	const cevent_tst  *ep1, *ep2;
	int32_t r1, r2;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	r1 = eq.put_left(&eq, &e1);
	r2 = eq.put_left(&eq, &e2);
	ep1 = eq.get(&eq);
	ep2 = eq.get(&eq);

	TEST_ASSERT_EQUAL(0, r1);
	TEST_ASSERT_EQUAL(0, r2);
	TEST_ASSERT(ep1 == &e2);
	TEST_ASSERT(ep2 == &e1);
}

/**
 * Check that put_left is able to insert an element in
 * front of the previously put in items.
 */
TEST(cq, put_left_after_put)
{
	cevent_tst  e1, e2, e3;
	const cevent_tst  *ep1;
	int32_t r1, r2, r3;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	r1 = eq.put(&eq, &e1);
	r2 = eq.put(&eq, &e2);
	r3 = eq.put_left(&eq, &e3);
	ep1 = eq.get(&eq);

	TEST_ASSERT_EQUAL(0, r1);
	TEST_ASSERT_EQUAL(0, r2);
	TEST_ASSERT_EQUAL(0, r3);
	TEST_ASSERT(ep1 == &e3);
}

/**
 * Check that put_left returns -1 when there are no more slots.
 */
TEST(cq, put_left_more_than_capacity)
{
	cevent_tst  e1;
	int32_t r;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	for (uint16_t i=0; i<5; i++)
	{
		r = eq.put_left(&eq, &e1);
	}

	TEST_ASSERT(-1 == r);
	TEST_ASSERT_EQUAL(1, eq.fault_cnt);
}

/**
 * Test that we can get back the last event from the queue,
 * when 2 was put in
 */
TEST(cq, put_2_get_right_1)
{
	cevent_tst  e1, e2;
	const cevent_tst  *ep;
	int32_t r1, r2;

	cqueue_init(&eq, (const cevent_tst  **)events, 4);
	r1 = eq.put(&eq, &e1);
	r2 = eq.put(&eq, &e2);
	ep = eq.get_right(&eq);

	TEST_ASSERT_EQUAL(0, r1);
	TEST_ASSERT_EQUAL(0, r2);
	TEST_ASSERT(ep == &e2);
}


TEST_GROUP_RUNNER(cq)
{
	RUN_TEST_CASE(cq, put_1_get_1);
	RUN_TEST_CASE(cq, put_2_get_1);
	RUN_TEST_CASE(cq, put_2_get_2);
	RUN_TEST_CASE(cq, get_from_empty);
	RUN_TEST_CASE(cq, put_more_than_capacity);
	RUN_TEST_CASE(cq, test_put_roll);
	RUN_TEST_CASE(cq, test_get_roll);
	RUN_TEST_CASE(cq, put_left_1_get_1);
	RUN_TEST_CASE(cq, put_left_2_get_2);
	RUN_TEST_CASE(cq, put_left_after_put);
	RUN_TEST_CASE(cq, put_left_more_than_capacity);
	RUN_TEST_CASE(cq, get_all);
	RUN_TEST_CASE(cq, put_2_get_right_1);
	RUN_TEST_CASE(cq, test_put_a_lot);
//	RUN_TEST_CASE(cq, new_1);
//	RUN_TEST_CASE(cq, new_1);
//	RUN_TEST_CASE(cq, new_1);
}
