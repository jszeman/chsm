/*
 * main.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#include <stdio.h>
#include "unity_fixture.h"
#include "unity.h"

void disableInterrupts(void)
{

}

void enableInterrupts(void)
{

}

void run_hsm_tests(void)
{
	RUN_TEST_GROUP(ep);
	RUN_TEST_GROUP(cq);
	RUN_TEST_GROUP(hsm);
	RUN_TEST_GROUP(crf);
	RUN_TEST_GROUP(cb);
	RUN_TEST_GROUP(cv);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_hsm_tests);
}
