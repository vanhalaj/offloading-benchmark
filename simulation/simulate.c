#include "simulate.h"

#include <stdlib.h>
#include "lyapunov/lyapunov.h"

static void write_csv_header(FILE* fp, const SweepConfig* cfg)
{
    switch (cfg->type)
    {
        case SWEEP_CPU_FREQ_LOCAL: fprintf(fp, "f_local"); break;
        case SWEEP_CPU_FREQ_OFFLOADED: fprintf(fp, "f_off"); break;
        case SWEEP_LATENCY: fprintf(fp, "d_latency"); break;
        case SWEEP_BANDWIDTH_UP: fprintf(fp, "R_up"); break;
        case SWEEP_BANDWIDTH_DOWN: fprintf(fp, "R_down"); break;
        case SWEEP_POWER_LOAD: fprintf(fp, "P_load"); break;
        case SWEEP_POWER_IDLE: fprintf(fp, "P_idle"); break;
        case SWEEP_POWER_TRANSMITTER: fprintf(fp, "P_tx"); break;
        case SWEEP_POWER_RECEIVER: fprintf(fp, "P_rx"); break;
        case SWEEP_TASK_COMPLEXITY_AVG: fprintf(fp, "c_T"); break;
        case SWEEP_TASK_COMPLEXITY_VAR: fprintf(fp, "c_T (var)"); break;
        case SWEEP_TASK_SIZE_AVG: fprintf(fp, "L_T"); break;
        case SWEEP_TASK_SIZE_VAR: fprintf(fp, "L_T (var)"); break;
        case SWEEP_TASK_DEPENDENCY_PROB: fprintf(fp, "dependency_rate"); break;
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

static void apply_sweep(const SweepConfig* cfg, DeviceDescriptions* dev, SchedulerConfig* scheduler, double x)
{
    switch (cfg->type)
    {
    case SWEEP_CPU_FREQ_LOCAL: dev->cpu_freq_local = x; break;
    case SWEEP_CPU_FREQ_OFFLOADED: dev->cpu_freq_offloaded = x; break;
    case SWEEP_LATENCY: dev->network_latency = x; break;
    case SWEEP_BANDWIDTH_UP: dev->bandwidth_up = x; break;
    case SWEEP_BANDWIDTH_DOWN: dev->bandwidth_down = x; break;
    case SWEEP_POWER_LOAD: dev->power_load = x; break;
    case SWEEP_POWER_IDLE: dev->power_idle = x; break;
    case SWEEP_POWER_TRANSMITTER: dev->power_transmitter = x; break;
    case SWEEP_POWER_RECEIVER: dev->power_receiver = x; break;
    case SWEEP_TASK_COMPLEXITY_AVG:
        scheduler->complexity_mode = COMPLEXITY_RANDOM;
        scheduler->avg_complexity = x; break;
    case SWEEP_TASK_COMPLEXITY_VAR:
        scheduler->complexity_mode = COMPLEXITY_RANDOM;
        scheduler->var_complexity = x; break;
    case SWEEP_TASK_SIZE_AVG: scheduler->avg_size = (int)x; break;
    case SWEEP_TASK_SIZE_VAR: scheduler->var_size = (int)x; break;
    case SWEEP_TASK_DEPENDENCY_PROB: scheduler->dependency_probability = (float)x; break;
    }
}

static TaskDescription* generate_tasks(const SchedulerConfig* scheduler)
{
    TaskDescription* task_queue = calloc(scheduler->task_count, sizeof(TaskDescription));
    if (!task_queue)
    {
        printf("Memory allocation for task queue failed\n");
        exit(EXIT_FAILURE);
    }
    generate_task_queue(task_queue, scheduler);
    return task_queue;
}

void run_sweep(const SweepConfig* cfg, const DeviceDescriptions* devices, const SchedulerConfig* scheduler_cfg, FILE* fp)
{
    write_csv_header(fp, cfg);

    for (double x = cfg->start; x <= cfg->end; x += cfg->step)
    {
        DeviceDescriptions dev = *devices;
        SchedulerConfig scheduler = *scheduler_cfg;

        apply_sweep(cfg, &dev, &scheduler, x);
        TaskDescription* task_queue = generate_tasks(&scheduler);
        SimResult results[DECISION_ALGORITHM_COUNT] = { 0 };

        for (int task_index = 0; task_index < scheduler.task_count; task_index++)
        {
            TaskDescription* task = &task_queue[task_index];
            DecisionFactors factors = calculate_factors(&dev, task);

            for (DecisionAlgorithm algo = 0; algo < DECISION_ALGORITHM_COUNT; algo++)
            {
                // TODO should these two inner loops other way around so one algorithm is went over completely
                // before moving onto the next one?
                if (algo == LYAPUNOV)
                {
                    set_arrival_rate((double)results[ALWAYS_LOCAL].task_count / results[ALWAYS_LOCAL].total_delay);
                }

                int decision = do_offload_decision(factors, algo);
                update_result(&results[algo], decision, &factors);
            }
        }

        log_results(fp, x, results);
        free(task_queue);
    }
}

void update_result(SimResult* result, int decision, DecisionFactors* factors)
{
    result->offloaded_count += decision;
    result->task_count += 1;
    result->total_energy += ((1 - decision) * factors->energy_local + decision * factors->energy_offloaded);
    result->total_delay += ((1 - decision) * factors->delay_local + decision * factors->delay_offloaded);
}
