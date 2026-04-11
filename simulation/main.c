#include <stdio.h>
#include <math.h>

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
        .network_latency = 2.0 ms,
        .bandwidth_up = 10.0 Mbps,
        .bandwidth_down = 100.0 Mbps,
        .power_load = 5.0 Watts,
        .power_idle = 1.0 Watts,
        .power_transmitter = 0.2 Watts,
        .power_receiver = 0.2 Watts
    };
    TaskDescription task = {
        .task_input_size = 1024,
        .task_output_size = 1024,
        .task_computation_size = 350.0 * 1000000
    };

    DecisionFactors factors = calculate_factors(&devices, &task);
    DecisionAlgorithm algo = GREEDY;

    int decision = do_offload_decision(factors, algo);

    printf("Algo type: %s \n", decision_algorithm_to_string(algo));
    printf("Decision was %d \n", decision);

    return 0;
}