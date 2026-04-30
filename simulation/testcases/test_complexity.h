#ifndef TEST_COMPLEXITY_H
#define TEST_COMPLEXITY_H

#include "testcase.h"

const TestCase test_complexity = {
    .test_name = "complexity",
    .devices = {
        .cpu_freq_local = 700.0 MHz,
        .cpu_freq_offloaded = 3600.0 MHz,
        .power_load = 2.5 Watts,
        .power_idle = 1.2 Watts,
        .power_transmitter = 0.2 Watts,
        .power_receiver = 0.2 Watts
    },
    .network = {
        .latency_avg = 10.0 ms,
        .latency_var = 3.0 ms,
        .bandwidth_up = 100.0 Mbps,
        .bandwidth_down = 100.0 Mbps
    },
    .scheduler = {
        .task_count = 4096,
        .avg_size = 640000,
        .var_size = 630000,
        .avg_complexity = 10000000.0,
        .var_complexity = 250000,
        .complexity_mode = COMPLEXITY_RANDOM,
        .dependency_probability = 0.35f,
        .generator_seed = 12345U
    },
    .nested = 0,
    .outer_sweep = {
        .type = SWEEP_TASK_COMPLEXITY_AVG,
        .start = 8000000,
        .end = 20000000,
        .step = 300000
    }
};

#endif