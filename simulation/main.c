#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "testcase.h"
#include "simulate.h"
#include "decision.h"

#include "testcases/test_latency.h"
#include "testcases/test_latency_var.h"
#include "testcases/test_complexity.h"
#include "testcases/test_cpu_freq_local.h"
#include "testcases/test_bandwidth_up.h"
#include "testcases/test_dependency.h"

const TestCase* tests[] = {
    &test_latency,
    &test_latency_var,
    &test_complexity,
    &test_cpu_freq_local,
    &test_bandwidth_up,
    &test_dependency
};
const int test_count = sizeof(tests) / sizeof(TestCase*);

int main(void)
{
    printf("Simulation started\n");

    for (int i = 0; i < test_count; i++)
    {
        const TestCase* test = tests[i];
        printf("Running test %s ... ", test->test_name);
        
        run_testcase(test);
        printf("DONE\n");
    }

    printf("Simulation done, %d cases run\n", test_count);
    return 0;
}