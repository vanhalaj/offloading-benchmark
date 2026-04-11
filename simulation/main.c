#include <stdio.h>

#include "decision.h"

int main(void)
{
    printf("Simulation started\n");

    DecisionFactors factors = {
        .task_input_size = 64,
        .task_output_size = 64,
        .task_computation_size = 64,
        .delay_computation_local = 5.0,
        .delay_computation_offloaded = 5.0,
        .delay_transmission = 5.0,
        .delay_receive = 5.0,
        .energy_computation_local = 5.0,
        .energy_transmission = 5.0,
        .energy_receive = 5.0,
        .energy_idle = 5.0
    };
    DecisionAlgorithm algo = GREEDY;

    int decision = do_offload_decision(factors, algo);

    printf("Algo type: %s \n", decision_algorithm_to_string(algo));
    printf("Decision was %d \n", decision);

    return 0;
}