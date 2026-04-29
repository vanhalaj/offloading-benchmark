#ifndef DECISION_H
#define DECISION_H

#include "task.h"
#include "device.h"

struct decision_factors_t
{
	TaskDescription task;
	double delay_local; // total local delay, (same as computation local)
	double delay_offloaded; // total offloaded delay (transmission + computation + receive)
	double delay_computation_local; // seconds
	double delay_computation_offloaded; // seconds
	double delay_transmission; // seconds
	double delay_receive; // seconds
	double energy_local; // total local energy (same as computation local)
	double energy_offloaded; // total offloaded energy (transmission + idle + receive)
	double energy_computation_local; // joules
	double energy_transmission; // joules
	double energy_receive; // joules
	double energy_idle; // joules
} typedef DecisionFactors;

enum decision_algorithm_t
{
	ALWAYS_LOCAL,
	ALWAYS_OFFLOAD,
	OPTIMAL,
	GREEDY,
	LYAPUNOV,
	REINFORCEMENT_LEARNING,
	DECISION_ALGORITHM_COUNT
} typedef DecisionAlgorithm;

const char* decision_algorithm_to_string(DecisionAlgorithm algorithm);

/*!
 * @brief Make offloading decision with the given factors.
 * @return 0 for local execution and 1 for offloading. Other values mean error
 */
int do_offload_decision(const DecisionFactors* factors, DecisionAlgorithm algorithm);

/*!
 * Calculate decision factors from device and task variables
 */
DecisionFactors calculate_factors(const DeviceDescriptions* devices, const TaskDescription* task, int previous_decision);

#endif // DECISION_H