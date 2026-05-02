#ifndef TEST_BANDWIDTH_LATENCY_H
#define TEST_BANDWIDTH_LATENCY_H

#include "testcase.h"

const TestCase test_bandwidth_latency = {
    .test_name = "bandwidth_latency",
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
        .latency_var = 1 ms,
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
    .nested = 1,
    .outer_sweep = {
        .type = SWEEP_BANDWIDTH_UP,
        .start = 2 Mbps,
        .end = 100 Mbps,
        .step = 2 Mbps
    },
    .inner_sweep = {
        .type = SWEEP_LATENCY_AVG,
        .start = 2 ms,
        .end = 50 ms,
        .step = 2 ms
    }
};

#endif