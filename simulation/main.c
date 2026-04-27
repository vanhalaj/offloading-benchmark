#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "simulate.h"
#include "decision.h"
#include "device.h"
#include "task.h"
#include "scheduler.h"
#include "network.h"

#define MHz * 1000000.0
#define Mbps / 8.0 * 1000000.0
#define ms * 0.001
#define Watts * 1.0

const DeviceDescriptions devices = {
    .cpu_freq_local = 700.0 MHz,
    .cpu_freq_offloaded = 3600.0 MHz,
    .power_load = 2.5 Watts,
    .power_idle = 1.2 Watts,
    .power_transmitter = 0.2 Watts,
    .power_receiver = 0.2 Watts
};

const NetworkDescription network = {
    .latency_avg = 10.0 ms,
    .latency_var = 3.0 ms,
    .bandwidth_up = 100.0 Mbps,
    .bandwidth_down = 100.0 Mbps
};

const SchedulerConfig scheduler = {
    .task_count = 4096,
    .avg_size = 640000,
    .var_size = 630000,
    .avg_complexity = 10000000.0,
    .var_complexity = 2500000,
    .complexity_multiplier = 8.0,
    .complexity_mode = COMPLEXITY_ESTIMATE,
    .dependency_probability = 0.35f,
    .generator_seed = 12345U
};

const SweepConfig sweeps[] = {
    //{.type = SWEEP_TASK_COMPLEXITY_AVG, .start = 300000, .end = 20000000, .step = 300000 },
    {.type = SWEEP_LATENCY_AVG, .start = 4 ms, .end = 40 ms, .step = 1 ms },
    {.type = SWEEP_LATENCY_VAR, .start = 1 ms, .end = 20 ms, .step = 1 ms },
    {.type = SWEEP_CPU_FREQ_LOCAL, .start = 500 MHz, .end = 3000 MHz, .step = 100 MHz },
    {.type = SWEEP_LATENCY_AVG, .start = 1 ms, .end = 50 ms, .step = 1 ms }
};

int main(void)
{
    printf("Simulation started\n");

    FILE* fp = fopen("results.csv", "w");
    if (!fp)
    {
        printf("Failed to open file!\n");
        exit(EXIT_FAILURE);
    }

    run_sweep(&sweeps[0], &devices, &network, &scheduler, fp);

    fflush(fp);
    fclose(fp);

    return 0;
}