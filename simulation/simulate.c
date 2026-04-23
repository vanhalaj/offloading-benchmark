#include "simulate.h"

#include "scheduler.h"
#include "lyapunov/lyapunov.h"

static void write_csv_header(FILE* fp, SweepConfig* cfg)
{
    switch (cfg->type)
    {
        case SWEEP_CPU_FREQ_LOCAL: fprintf(fp, "f_local"); break;
        case SWEEP_CPU_FREQ_OFFLOADED: fprintf(fp, "f_off"); break;
        case SWEEP_LATENCY: fprintf(fp, "d_network"); break;
        case SWEEP_BANDWIDTH_UP: fprintf(fp, "r_up"); break;
        case SWEEP_BANDWIDTH_DOWN: fprintf(fp, "r_down"); break;
        case SWEEP_POWER_LOAD: fprintf(fp, "p_load"); break;
        case SWEEP_POWER_IDLE: fprintf(fp, "p_idle"); break;
        case SWEEP_POWER_TRANSMITTER: fprintf(fp, "p_tx"); break;
        case SWEEP_POWER_RECEIVER: fprintf(fp, "p_rx"); break;
        case SWEEP_TASK_COMPUTATION: fprintf(fp, "c_t"); break;
    }
    for (DecisionAlgorithm algo = 0; algo < DECISION_ALGORITHM_COUNT; algo++)
    {
        const char* name = decision_algorithm_to_string(algo);
        fprintf(fp, ",%s_e_total", name);
        fprintf(fp, ",%s_d_total", name);
        fprintf(fp, ",%s_offload_count", name);
        fprintf(fp, ",%s_task_total", name);
    }
    fprintf(fp, "\n");
}

static void log_results(FILE* fp, double x, SimResult results[])
{
    fprintf(fp, "%f", x);
    for (DecisionAlgorithm algo = 0; algo < DECISION_ALGORITHM_COUNT; algo++)
    {
        SimResult* result = &results[algo];
        fprintf(fp, ",%f", result->total_energy);
        fprintf(fp, ",%f", result->total_delay);
        fprintf(fp, ",%d", result->offloaded_count);
        fprintf(fp, ",%d", result->task_count);
    }
    fprintf(fp, "\n");
}

void run_sweep(const SweepConfig* cfg, DeviceDescriptions* devices, TaskDescription* task_queue, int task_count, FILE* fp)
{
    write_csv_header(fp, cfg);

    for (double x = cfg->start; x <= cfg->end; x += cfg->step)
    {
        DeviceDescriptions dev = *devices;

        int avg_task_size = (int)x;
        int variance = 25000;
        generate_task_queue(task_queue, 4096, avg_task_size - variance, avg_task_size + variance, 12345U);

        /*switch (cfg->type)
        {
            case SWEEP_CPU_FREQ_LOCAL: dev.cpu_freq_local = x; break;
            case SWEEP_CPU_FREQ_OFFLOADED: dev.cpu_freq_offloaded = x; break;
            case SWEEP_LATENCY: dev.network_latency = x; break;
            case SWEEP_BANDWIDTH_UP: dev.bandwidth_up = x; break;
            case SWEEP_BANDWIDTH_DOWN: dev.bandwidth_down = x; break;
            case SWEEP_POWER_LOAD: dev.power_load = x; break;
            case SWEEP_POWER_IDLE: dev.power_idle = x; break;
            case SWEEP_POWER_TRANSMITTER: dev.power_transmitter = x; break;
            case SWEEP_POWER_RECEIVER: dev.power_receiver = x; break;
        }*/

        SimResult results[DECISION_ALGORITHM_COUNT] = { 0 };

        for (int task_index = 0; task_index < task_count; task_index++)
        {
            TaskDescription* task = &task_queue[task_index];
            DecisionFactors factors = calculate_factors(&dev, task);

            for (DecisionAlgorithm algo = 0; algo < DECISION_ALGORITHM_COUNT; algo++)
            {
                if (algo == LYAPUNOV)
                {
                    set_local_time((double)results[0].task_count / results[0].total_delay);
                }

                int decision = do_offload_decision(factors, algo);
                update_result(&results[algo], decision, &factors);
            }
        }

        log_results(fp, x, results);
    }
}

void update_result(SimResult* result, int decision, DecisionFactors* factors)
{
    result->offloaded_count += decision;
    result->task_count += 1;
    result->total_energy += ((1 - decision) * factors->energy_local + decision * factors->energy_offloaded);
    result->total_delay += ((1 - decision) * factors->delay_local + decision * factors->delay_offloaded);
}
