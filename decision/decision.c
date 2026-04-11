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
	DecisionFactors factors = {
		0
	};
	
	return factors;
}
