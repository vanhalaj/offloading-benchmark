#include "decision.h"

#include <stdlib.h>
#include <stdio.h>
#include "greedy/greedy.h"
#include "lyapunov/lyapunov.h"
#include "rl/rl.h"

static int previous_decision = 0;

const char* decision_algorithm_to_string(DecisionAlgorithm algorithm)
{
	switch (algorithm)
	{
	case ALWAYS_LOCAL:
		return "ALWAYS_LOCAL";
	case ALWAYS_OFFLOAD:
		return "ALWAYS_OFFLOAD";
	case GREEDY:
		return "GREEDY";
	case LYAPUNOV:
		return "LYAPUNOV";
	case REINFORCEMENT_LEARNING:
		return "REINFORCEMENT_LEARNING";
	default:
		return "UNKNOWN";
	}
}

int do_offload_decision(const DecisionFactors* factors, DecisionAlgorithm algorithm)
{
	int decision = 0;
	switch (algorithm)
	{
		case ALWAYS_LOCAL:
			decision = 0;
			break;
		case ALWAYS_OFFLOAD:
			decision = 1;
			break;
		case GREEDY:
			decision = greedy_decision(factors);
			break;
		case LYAPUNOV:
			decision = lyapunov_decision(factors);
			break;
		case REINFORCEMENT_LEARNING:
			decision = reinforcement_learning_decision(factors);
			break;
	}

	previous_decision = decision;
	return decision;
}

static void validate_data(const DeviceDescriptions* d, const TaskDescription* t)
{
	int invalid =
		(d->bandwidth_down <= 0.0) || (d->bandwidth_up <= 0.0) ||
		(d->cpu_freq_local <= 0.0) || (d->cpu_freq_offloaded <= 0.0) ||
		(d->network_latency <= 0.0) ||
		(d->power_idle <= 0.0) || (d->power_load <= 0.0) ||
		(d->power_receiver <= 0.0) || (d->power_transmitter <= 0.0) ||
		(t->task_input_size <= 0) || (t->task_output_size <= 0) ||
		(t->task_computation_size <= 0.0);
	if (invalid)
	{
		printf("Invalid data found:\nTask: {input: %d, output: %d, comp: %f}\n", t->task_input_size, t->task_output_size, t->task_computation_size);
	}
}

DecisionFactors calculate_factors(const DeviceDescriptions* devices, const TaskDescription* task)
{
	validate_data(devices, task);

	double d_comp_local = task->task_computation_size / devices->cpu_freq_local;
	double d_comp_off = task->task_computation_size / devices->cpu_freq_offloaded;
	double d_trans = task->task_input_size / devices->bandwidth_up;
	double d_tx = d_trans + devices->network_latency;
	double d_recv = task->task_output_size / devices->bandwidth_down;
	double d_rx = d_recv + devices->network_latency;

	double d_local = d_comp_local;
	double d_off = (1 - (task->dependent && previous_decision)) * d_tx + d_comp_off + d_rx;

	double e_comp_local = devices->power_load * d_comp_local;
	double e_idle = devices->power_idle * d_comp_off;
	double e_tx = devices->power_transmitter * d_trans;
	double e_rx = devices->power_receiver * d_recv;
	
	double e_local = e_comp_local;
	double e_off = e_tx + e_idle + e_rx;

	DecisionFactors factors = {
		.task = *task,
		.delay_local = d_local,
		.delay_offloaded = d_off,
		.delay_computation_local = d_comp_local,
		.delay_computation_offloaded = d_comp_off,
		.delay_transmission = d_tx,
		.delay_receive = d_rx,
		.energy_local = e_local,
		.energy_offloaded = e_off,
		.energy_computation_local = e_local,
		.energy_transmission = e_tx,
		.energy_receive = e_rx,
		.energy_idle = e_idle
	};
	
	return factors;
}
