#ifndef TESTCASE_H
#define TESTCASE_H

#include "simulate.h"
#include "device.h"
#include "scheduler.h"
#include "network.h"

#define MHz * 1000000.0
#define Mbps / 8.0 * 1000000.0
#define ms * 0.001
#define Watts * 1.0

struct testcase_t
{
    const char* file_name;
    const DeviceDescriptions devices;
    const NetworkDescription network;
    const SchedulerConfig scheduler;
    const int nested;
    const SweepConfig outer_sweep;
    const SweepConfig inner_sweep; // only used if nested=1
} typedef TestCase;

#endif