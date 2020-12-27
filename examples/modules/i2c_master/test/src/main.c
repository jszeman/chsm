#include <stdio.h>
#include "unity_fixture.h"
#include "unity.h"

void disableInterrupts(void)
{

}

void enableInterrupts(void)
{

}

void run_i2c_master_tests(void)
{
	RUN_TEST_GROUP(i2c_master);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_i2c_master_tests);
}