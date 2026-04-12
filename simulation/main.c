#include <stdio.h>
#include <math.h>

#include "decision.h"
#include "device.h"
#include "task.h"

#include "profiled_estimate.h"

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

    DecisionAlgorithm algo = GREEDY;

    for (int i = 1; i < 128; i++)
    {
        int input_size = i * 10000;
        double computation_size = estimate_e1_complexity(input_size);

        TaskDescription task = {
            .task_input_size = input_size,
            .task_output_size = input_size,
            .task_computation_size = computation_size
        };

        DecisionFactors factors = calculate_factors(&devices, &task);

        int decision = do_offload_decision(factors, algo);

        printf("========== Iteration %d ============\n", i);
        printf("Algo type: %s \n", decision_algorithm_to_string(algo));
        printf("Local energy: %f offloaded %f \n", factors.energy_local, factors.energy_offloaded);
        printf("Local delay: %f offloaded %f \n", factors.delay_local, factors.delay_offloaded);
        printf("Decision was %d \n", decision);
    }

    return 0;
}