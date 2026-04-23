#ifndef SIMULATE_H
#define SIMULATE_H

#include <stdio.h>
#include "device.h"
#include "task.h"
#include "decision.h"

enum sweep_type_t 
{
    SWEEP_CPU_FREQ_LOCAL,
    SWEEP_CPU_FREQ_OFFLOADED,
    SWEEP_LATENCY,
    SWEEP_BANDWIDTH_UP,
    SWEEP_BANDWIDTH_DOWN,
    SWEEP_POWER_LOAD,
    SWEEP_POWER_IDLE,
    SWEEP_POWER_TRANSMITTER,
    SWEEP_POWER_RECEIVER,
    SWEEP_TASK_COMPUTATION
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


void run_sweep(const SweepConfig* cfg, DeviceDescriptions* devices, TaskDescription* task_queue, int task_count, FILE* fp);

void update_result(SimResult* result, int decision, DecisionFactors* factors);

#endif