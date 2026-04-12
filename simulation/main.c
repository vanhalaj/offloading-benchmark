#include <stdio.h>
#include <math.h>

#include "simulate.h"
#include "decision.h"
#include "device.h"
#include "task.h"

#define MHz * 1000000.0
#define Mbps * 1000000.0
#define ms * 0.001
#define Watts * 1.0

int main(void)
{
    printf("Simulation started\n");

    DeviceDescriptions devices = { 
        .cpu_freq_local = 700.0 MHz,
        .cpu_freq_offloaded = 3600.0 MHz,
        .network_latency = 5.0 ms,
        .bandwidth_up = 10.0 Mbps,
        .bandwidth_down = 100.0 Mbps,
        .power_load = 2.5 Watts,
        .power_idle = 1.2 Watts,
        .power_transmitter = 0.2 Watts,
        .power_receiver = 0.2 Watts
    };

    SweepConfig sweeps[] = {
        { .type = SWEEP_INPUT_SIZE, .start = 10000, .end = 128*10000, .step = 10000 },
        { .type = SWEEP_CPU_FREQ_LOCAL, .start = 500 MHz, .end = 3000 MHz, .step = 100 MHz },
        { .type = SWEEP_LATENCY, .start = 1 ms, .end = 50 ms, .step = 1 ms }
    };

    // TODO sweep over all options
    // TODO scheduler
    run_sweep(&sweeps[0], &devices, 100000);

    return 0;
}