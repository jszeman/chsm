#include <stdio.h>
#include "unity_fixture.h"
#include "unity.h"
#include "crf.h"

crf_tst					crf;

void disableInterrupts(void)
{

}

void enableInterrupts(void)
{

}

void run_co_tests(void)
{
	RUN_TEST_GROUP(co);
	RUN_TEST_GROUP(od);
	RUN_TEST_GROUP(bk);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_co_tests);
}