#include <stdio.h>
#include "unity_fixture.h"
#include "unity.h"

void disableInterrupts(void)
{

}

void enableInterrupts(void)
{

}

void run_fram_tests(void)
{
	RUN_TEST_GROUP(fram);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_fram_tests);
}