#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "testcase.h"
#include "simulate.h"
#include "decision.h"

#include "testcases/test_latency.h"

const SweepConfig sweeps[] = {
    //{.type = SWEEP_TASK_COMPLEXITY_AVG, .start = 300000, .end = 20000000, .step = 300000 },
    {.type = SWEEP_LATENCY_AVG, .start = 4 ms, .end = 40 ms, .step = 1 ms },
    {.type = SWEEP_LATENCY_VAR, .start = 1 ms, .end = 20 ms, .step = 1 ms },
    {.type = SWEEP_CPU_FREQ_LOCAL, .start = 500 MHz, .end = 3000 MHz, .step = 100 MHz },
    {.type = SWEEP_LATENCY_AVG, .start = 1 ms, .end = 50 ms, .step = 1 ms }
};

const TestCase* tests[] = {
    &test_latency
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