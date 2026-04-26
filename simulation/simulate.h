#ifndef SIMULATE_H
#define SIMULATE_H

#include <stdio.h>
#include "device.h"
#include "task.h"
#include "decision.h"
#include "scheduler.h"
#include "network.h"

enum sweep_type_t
{
    SWEEP_CPU_FREQ_LOCAL,
    SWEEP_CPU_FREQ_OFFLOADED,
    SWEEP_LATENCY_AVG,
    SWEEP_LATENCY_VAR,
    SWEEP_BANDWIDTH_UP,
    SWEEP_BANDWIDTH_DOWN,
    SWEEP_POWER_LOAD,
    SWEEP_POWER_IDLE,
    SWEEP_POWER_TRANSMITTER,
    SWEEP_POWER_RECEIVER,
    SWEEP_TASK_COMPLEXITY_AVG,
    SWEEP_TASK_COMPLEXITY_VAR,
    SWEEP_TASK_SIZE_AVG,
    SWEEP_TASK_SIZE_VAR,
    SWEEP_TASK_DEPENDENCY_PROB
} typedef SweepType;

struct sweep_config_t 
{
    SweepType type;
    double start;
    double end;
    double step;
} typedef SweepConfig;

struct sim_result_t
{
    double total_energy;
    double total_delay;
    int offloaded_count;
    int task_count;
} typedef SimResult;


void run_sweep(const SweepConfig* cfg, const DeviceDescriptions* devices, const NetworkDescription* network, const SchedulerConfig* scheduler_cfg, FILE* fp);

void update_result(SimResult* result, int decision, DecisionFactors* factors);

#endif