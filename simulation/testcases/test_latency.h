#ifndef TEST_LATENCY_H
#define TEST_LATENCY_H

#include "testcase.h"

const TestCase test_latency = {
    .file_name = "latency.csv",
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
        .complexity_multiplier = 8.0,
        .complexity_mode = COMPLEXITY_ESTIMATE,
        .dependency_probability = 0.35f,
        .generator_seed = 12345U
    },
    .nested = 0,
    .outer_sweep = {
        .type = SWEEP_LATENCY_AVG,
        .start = 4 ms,
        .end = 40 ms,
        .step = 1 ms
    }
};

#endif