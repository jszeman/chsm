/*
 * canopen_test.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#include <chsm.h>
#include "cevent.h"
#include <stdint.h>
#include <string.h>
#include "unity_fixture.h"
#include "chsm_test.h"

TEST_GROUP(hsm);

CHsm hsm;

char log_str[1024];




TEST_SETUP(hsm)
{
	memset(log_str, 0, sizeof(log_str));
}

TEST_TEAR_DOWN(hsm)
{
}

/*
TEST(hsm, enter_initial_state)
{
	chsm_ctor(&hsm, a_initial);
	chsm_init(&hsm);

	TEST_ASSERT_EQUAL_STRING("a_entry", log_str);
}

TEST(hsm, enter_nested_initial_state)
{
	chsm_ctor(&hsm, b_initial);
	chsm_init(&hsm);

	TEST_ASSERT_EQUAL_STRING("b_entry, b1_entry", log_str);
}

TEST(hsm, execute_init_after_entry)
{
	chsm_ctor(&hsm, c_initial);
	chsm_init(&hsm);

	TEST_ASSERT_EQUAL_STRING("c_entry, c_init, c1_entry", log_str);
}

TEST(hsm, process_event_after_init)
{
	CEvent chsm_test_event = {.sig=HSM_TEST_INTERNAL, .gc_info=0};

	chsm_ctor(&hsm, d_initial);
	chsm_init(&hsm);
	chsm_dispatch(&hsm, &chsm_test_event);

	TEST_ASSERT_EQUAL_STRING("d_entry, d_init, d1_entry, d1_test", log_str);
}
*/


TEST_GROUP_RUNNER(hsm)
{
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
	//RUN_TEST_CASE(hsm, test0);
}
