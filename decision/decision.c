#include "decision.h"

#include "greedy/greedy.h"

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

int do_offload_decision(DecisionFactors factors, DecisionAlgorithm algorithm)
{
	switch (algorithm)
	{
		case ALWAYS_LOCAL:
			return 0;
		case ALWAYS_OFFLOAD:
			return 1;
		case GREEDY:
			return greedy_decision(factors);
		case LYAPUNOV:
			return -2;
		case REINFORCEMENT_LEARNING:
			return -2;
	}

	return -1;
}

DecisionFactors calculate_factors(DeviceDescriptions* devices, TaskDescription* task)
{
	double d_comp_local = task->task_computation_size / devices->cpu_freq_local;
	double d_comp_off = task->task_computation_size / devices->cpu_freq_offloaded;
	double d_trans = task->task_input_size / devices->bandwidth_up;
	double d_tx = d_trans + devices->network_latency;
	double d_recv = task->task_output_size / devices->bandwidth_down;
	double d_rx = d_recv + devices->network_latency;

	double d_local = d_comp_local;
	double d_off = d_tx + d_comp_off + d_rx;

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
