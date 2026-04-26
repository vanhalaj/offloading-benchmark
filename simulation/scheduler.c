#include "scheduler.h"

#include <stdlib.h>

#include "profiled_estimate.h"

double (*task_estimates[])(int) = {
	estimate_e1_complexity
};
const int TASK_FUNCTIONS_COUNT = sizeof(task_estimates) / sizeof(task_estimates[0]);

static inline int random_range(int min, int max)
{
	return min + (rand() % (max - min + 1));
}

static inline double random_range_d(double min, double max)
{
	return min + ((double)rand() / RAND_MAX) * (max - min);
}

void generate_task_queue(TaskDescription* task_queue, const SchedulerConfig* config, const NetworkDescription* network)
{
	srand(config->generator_seed);

	for (int i = 0; i < config->task_count; i++)
	{
		const int size = random_range(config->avg_size - config->var_size, config->avg_size + config->var_size);
		const int type = rand() % TASK_FUNCTIONS_COUNT;
		const int dependency = rand() > (int)(config->dependency_probability * RAND_MAX);
		const int offloadability = 1;// rand() > (int)(OFFLOADABLE_PROBABILITY * RAND_MAX);

		const double complexity = config->complexity_mode == COMPLEXITY_ESTIMATE
			? task_estimates[type](size) * config->complexity_multiplier
			: random_range_d(config->avg_complexity - config->var_complexity, config->avg_complexity + config->var_complexity);

		const double latency = random_range_d(network->latency_avg - network->latency_var, network->latency_avg + network->latency_var);

		task_queue[i] = (TaskDescription){
			.task_input_size = size,
			.task_output_size = size,
			.task_computation_size = complexity,
			.dependent = dependency,
			.offloadable = offloadability,
			.network_state = (NetworkState){
				.latency = latency, .r_down = network->bandwidth_down, .r_up = network->bandwidth_up
			}
		};
	}
}
