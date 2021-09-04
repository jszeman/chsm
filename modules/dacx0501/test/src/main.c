#include <stdio.h>
#include "unity_fixture.h"
#include "unity.h"

void disableInterrupts(void)
{

}

void enableInterrupts(void)
{

}

void run_dacx0501_tests(void)
{
	RUN_TEST_GROUP(dacx0501);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_dacx0501_tests);
}