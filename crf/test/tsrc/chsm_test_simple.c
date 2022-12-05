/*
 * canopen_test.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "unity_fixture.h"
#include "chsm_simple_machine.h"

#include <stdio.h>

TEST_GROUP(hsm_smpl);

static simple_sm_tst  shsm;




TEST_SETUP(hsm_smpl)
{
	memset(&shsm, 0, sizeof(shsm));
	shsm.callback = simple_machine_callback;
	smpl_sm_clear_log(&shsm);
}

TEST_TEAR_DOWN(hsm_smpl)
{
}


TEST(hsm_smpl, enter_initial_state)
{
	shsm.callback(&shsm, SMPL_SIG_INIT);

	TEST_ASSERT_EQUAL_STRING("a_entry ", shsm.log_ac);
}

TEST(hsm_smpl, internal_event)
{
	shsm.callback(&shsm, SMPL_SIG_INIT);
	smpl_sm_clear_log(&shsm);

	shsm.callback(&shsm, SMPL_SIG_EVENT_A);

	TEST_ASSERT_EQUAL_STRING("a_func guard_1 ", shsm.log_ac);
}

TEST(hsm_smpl, internal_event_and_guard)
{
	shsm.callback(&shsm, SMPL_SIG_INIT);
	smpl_sm_clear_log(&shsm);

	shsm.guard_1 = true;
	shsm.callback(&shsm, SMPL_SIG_EVENT_A);

	TEST_ASSERT_EQUAL_STRING("a_func guard_1 a_exit c_entry b_entry ", shsm.log_ac);
}

TEST(hsm_smpl, external_trans_and_false_guard)
{
	shsm.callback(&shsm, SMPL_SIG_INIT);
	smpl_sm_clear_log(&shsm);

	shsm.callback(&shsm, SMPL_SIG_EVENT_H);

	TEST_ASSERT_EQUAL_STRING("guard_h(2) guard_1 ", shsm.log_ac);
}

TEST(hsm_smpl, external_trans_and_true_guard)
{
	shsm.callback(&shsm, SMPL_SIG_INIT);
	smpl_sm_clear_log(&shsm);

	shsm.guard_h = true;
	shsm.callback(&shsm, SMPL_SIG_EVENT_H);

	TEST_ASSERT_EQUAL_STRING("guard_h(2) func_h(5) a_exit c_entry b_entry ", shsm.log_ac);
}

TEST_GROUP_RUNNER(hsm_smpl)
{
	RUN_TEST_CASE(hsm_smpl, enter_initial_state);
	RUN_TEST_CASE(hsm_smpl, internal_event);
	RUN_TEST_CASE(hsm_smpl, internal_event_and_guard);
	RUN_TEST_CASE(hsm_smpl, external_trans_and_false_guard);
	RUN_TEST_CASE(hsm_smpl, external_trans_and_true_guard);
	// RUN_TEST_CASE(hsm_smpl, enter_initial_state);
	// RUN_TEST_CASE(hsm_smpl, enter_initial_state);
	// RUN_TEST_CASE(hsm_smpl, enter_initial_state);
	
}
