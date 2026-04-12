#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "simulate.h"
#include "decision.h"
#include "device.h"
#include "task.h"
#include "scheduler.h"

#define TASK_COUNT 4096

#define MHz * 1000000.0
#define Mbps / 8.0 * 1000000.0
#define ms * 0.001
#define Watts * 1.0

DeviceDescriptions devices = {
        .cpu_freq_local = 700.0 MHz,
        .cpu_freq_offloaded = 3600.0 MHz,
        .network_latency = 2.0 ms,
        .bandwidth_up = 100.0 Mbps,
        .bandwidth_down = 100.0 Mbps,
        .power_load = 2.5 Watts,
        .power_idle = 1.2 Watts,
        .power_transmitter = 0.2 Watts,
        .power_receiver = 0.2 Watts
};

SweepConfig sweeps[] = {
    {.type = SWEEP_LATENCY, .start = 1 ms, .end = 40 ms, .step = 1 ms },
    {.type = SWEEP_CPU_FREQ_LOCAL, .start = 500 MHz, .end = 3000 MHz, .step = 100 MHz },
    {.type = SWEEP_LATENCY, .start = 1 ms, .end = 50 ms, .step = 1 ms }
};

int main(void)
{
    printf("Simulation started\n");

    TaskDescription task_queue[TASK_COUNT] = { 0 };
    int min_task_size = 10000;
    generate_task_queue(task_queue, TASK_COUNT, min_task_size, 128 * min_task_size, 12345U);

    FILE* fp = fopen("results.csv", "w");
    if (!fp)
    {
        perror("Failed to open file");
        exit(1);
    }

    run_sweep(&sweeps[0], &devices, task_queue, TASK_COUNT, fp);

    fflush(fp);
    fclose(fp);

    return 0;
}