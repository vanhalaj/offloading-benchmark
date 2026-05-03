#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "testcase.h"
#include "simulate.h"
#include "decision.h"
#include "rl/rl.h"

#include "testcases/test_latency.h"
#include "testcases/test_latency_var.h"
#include "testcases/test_complexity.h"
#include "testcases/test_cpu_freq_local.h"
#include "testcases/test_bandwidth_up.h"
#include "testcases/test_dependency.h"
#include "testcases/test_complexity_size.h"
#include "testcases/test_bandwidth_latency.h"
#include "testcases/test_latency_avg_var.h"

const TestCase* tests[] = {
    &test_latency,
    &test_latency_var,
    &test_complexity,
    &test_cpu_freq_local,
    &test_bandwidth_up,
    &test_dependency,
    &test_complexity_size,
    &test_bandwidth_latency,
    &test_latency_avg_var
};
const int test_count = sizeof(tests) / sizeof(TestCase*);

int main(void)
{
    rl_load_q_table("q_table.bin");

    printf("Simulation started\n");

    for (int i = 0; i < test_count; i++)
    {
        const TestCase* test = tests[i];
        printf("Running test %s ... ", test->test_name);
        
        run_testcase(test);
        printf("DONE\n");
    }

    printf("Simulation done, %d cases run\n", test_count);

    rl_save_q_table("q_table.bin");

    return 0;
}