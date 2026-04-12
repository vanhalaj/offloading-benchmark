#include "simulate.h"

#include "profiled_estimate.h"
#include "decision.h"

void run_sweep(const SweepConfig* cfg, DeviceDescriptions* devices, int task_input_size)
{
    for (double x = cfg->start; x <= cfg->end; x += cfg->step)
    {
        int input_size = task_input_size;
        DeviceDescriptions dev = *devices;

        switch (cfg->type)
        {
        case SWEEP_INPUT_SIZE:
            input_size = (int)x;
            break;

        case SWEEP_CPU_FREQ_LOCAL:
            dev.cpu_freq_local = x;
            break;

        case SWEEP_CPU_FREQ_OFFLOADED:
            dev.cpu_freq_offloaded = x;
            break;

        case SWEEP_BANDWIDTH_UP:
            dev.bandwidth_up = x;
            break;

        case SWEEP_LATENCY:
            dev.network_latency = x;
            break;

            // TODO rest of sweep cases
        }

        double computation_size = estimate_e1_complexity(input_size);

        TaskDescription task = {
            .task_input_size = input_size,
            .task_output_size = input_size,
            .task_computation_size = computation_size
        };

        DecisionFactors factors = calculate_factors(&dev, &task);

        for (int algo = ALWAYS_LOCAL; algo <= REINFORCEMENT_LEARNING; algo++)
        {
            int decision = do_offload_decision(factors, algo);

            printf("========== Iteration ============\n");
            printf("Algo type: %s \n", decision_algorithm_to_string(algo));
            printf("Task in %d out %d computation %f", task.task_input_size, task.task_output_size, task.task_computation_size);
            printf("Local energy: %f offloaded %f \n", factors.energy_local, factors.energy_offloaded);
            printf("Local delay: %f offloaded %f \n", factors.delay_local, factors.delay_offloaded);
            printf("Decision was %d \n", decision);
        }
    }
}
