#ifndef SIMULATE_H
#define SIMULATE_H

#include "device.h"

enum sweep_type_t {
    SWEEP_INPUT_SIZE,
    SWEEP_OUTPUT_SIZE,
    SWEEP_CPU_FREQ_LOCAL,
    SWEEP_CPU_FREQ_OFFLOADED,
    SWEEP_BANDWIDTH_UP,
    SWEEP_BANDIWIDTH_DOWN,
    SWEEP_LATENCY
} typedef SweepType;

struct sweep_config_t {
    SweepType type;
    double start;
    double end;
    double step;
} typedef SweepConfig;


void run_sweep(const SweepConfig* cfg, DeviceDescriptions* devices, int task_input_size);

#endif