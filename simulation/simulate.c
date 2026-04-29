#include "simulate.h"

#include <stdlib.h>
#include "lyapunov/lyapunov.h"
#include "optimal/optimal.h"
#include "testcase.h"

#define SKIP_OPTIMAL 1

static void write_csv_header(FILE* fp, const SweepConfig* outer, const SweepConfig* inner, int nested)
{
    // first write outer and then inner (if nested=1) sweep param name
    for (int i = 0; i <= nested; i++)
    {
        const SweepConfig* cfg = i == 0 ? outer : inner;
        switch (cfg->type)
        {
        case SWEEP_CPU_FREQ_LOCAL: fprintf(fp, "f_local"); break;
        case SWEEP_CPU_FREQ_OFFLOADED: fprintf(fp, "f_off"); break;
        case SWEEP_LATENCY_AVG: fprintf(fp, "d_latency"); break;
        case SWEEP_LATENCY_VAR: fprintf(fp, "d_latency (var)"); break;
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

static void log_results(FILE* fp, double x, double y, int nested, SimResult results[])
{
    fprintf(fp, "%f", x);
    if (nested) fprintf(fp, "%f", y);

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

static void apply_sweep(const SweepConfig* cfg, DeviceDescriptions* dev, NetworkDescription* net, SchedulerConfig* scheduler, double x)
{
    switch (cfg->type)
    {
    case SWEEP_CPU_FREQ_LOCAL: dev->cpu_freq_local = x; break;
    case SWEEP_CPU_FREQ_OFFLOADED: dev->cpu_freq_offloaded = x; break;
    case SWEEP_LATENCY_AVG: net->latency_avg = x; break;
    case SWEEP_LATENCY_VAR: net->latency_var = x; break;
    case SWEEP_BANDWIDTH_UP: net->bandwidth_up = x; break;
    case SWEEP_BANDWIDTH_DOWN: net->bandwidth_down = x; break;
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

static TaskDescription* allocate_task_queue(const SchedulerConfig* scheduler)
{
    TaskDescription* task_queue = calloc(scheduler->task_count, sizeof(TaskDescription));
    if (!task_queue)
    {
        printf("Memory allocation for task queue failed\n");
        exit(EXIT_FAILURE);
    }
    return task_queue;
}

static void update_result(SimResult* result, int decision, DecisionFactors* factors)
{
    result->offloaded_count += decision;
    result->task_count += 1;
    result->total_energy += ((1 - decision) * factors->energy_local + decision * factors->energy_offloaded);
    result->total_delay += ((1 - decision) * factors->delay_local + decision * factors->delay_offloaded);
}

static void run_sweep(const TestCase* test, FILE* fp)
{
    const int nested = test->nested;
    write_csv_header(fp, &test->outer_sweep, &test->inner_sweep, nested);
    TaskDescription* task_queue = allocate_task_queue(&test->scheduler);

    for (double x = test->outer_sweep.start; x <= test->outer_sweep.end; x += test->outer_sweep.step)
    {
        DeviceDescriptions dev = test->devices;
        NetworkDescription net = test->network;
        SchedulerConfig scheduler = test->scheduler;
        apply_sweep(&test->outer_sweep, &dev, &net, &scheduler, x);

        // if nested=0, only run once
        for (double y = test->inner_sweep.start; y <= test->inner_sweep.end || (!nested); y += test->inner_sweep.step)
        {
            if (nested)
            {
                apply_sweep(&test->inner_sweep, &dev, &net, &scheduler, y);
            }
            generate_task_queue(task_queue, &scheduler, &net);
            #if !(SKIP_OPTIMAL)
            optimal_prepare(&dev, task_queue, scheduler.task_count);
            #endif

            SimResult results[DECISION_ALGORITHM_COUNT] = { 0 };
            int previous_decision[DECISION_ALGORITHM_COUNT] = { 0 };

            // Loop over tasks and find decision for each algorithm
            for (int task_index = 0; task_index < scheduler.task_count; task_index++)
            {
                TaskDescription* task = &task_queue[task_index];
                for (DecisionAlgorithm algo = 0; algo < DECISION_ALGORITHM_COUNT; algo++)
                {
                    if (algo == LYAPUNOV)
                    {
                        set_arrival_rate((double)results[ALWAYS_LOCAL].task_count / results[ALWAYS_LOCAL].total_delay);
                    }
                    #if SKIP_OPTIMAL
                    else if (algo == OPTIMAL)
                    {
                        continue;
                    }
                    #endif

                    DecisionFactors factors = calculate_factors(&dev, task, previous_decision[algo]);
                    int decision = do_offload_decision(&factors, algo);
                    
                    previous_decision[algo] = decision;
                    update_result(&results[algo], decision, &factors);
                }
            }

            log_results(fp, x, y, nested, results);
            optimal_free();

            if (!nested)
            {
                break;
            }
        }
    }

    free(task_queue);
}

void run_testcase(const TestCase* specs)
{
    FILE* fp = fopen(specs->file_name, "w");
    if (!fp)
    {
        printf("Failed to open file %s!\n", specs->file_name);
        exit(EXIT_FAILURE);
    }

    run_sweep(specs, fp);

    fflush(fp);
    fclose(fp);
}