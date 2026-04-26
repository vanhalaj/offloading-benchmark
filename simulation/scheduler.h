#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"
#include "network.h"

enum complexity_mode_t
{
	COMPLEXITY_RANDOM,
	COMPLEXITY_ESTIMATE
} typedef ComplexityMode;

/*!
 * @brief Config used to generate tasks in to the queue.
 */
struct scheduler_config_t
{
	int task_count; // How many tasks to generate; size of task queue
	int avg_size, var_size; // Size of task (in bytes). Uniform random is taken between (avg-var)...(avg+var)
	double avg_complexity, var_complexity; // Complexity of the task (in cycles), only used with COMPLEXITY_RANDOM. Uniform between (avg-var)...(avg+var)
	double complexity_multiplier; // When using COMPLEXITY_ESTIMATE, multiply estimated task complexity with this value
	ComplexityMode complexity_mode; // Use either random or estimated complexity
	float dependency_probability; // Value between 0...1, where 1 means that all tasks are dependent from their previous task.
	//float offloadable_probability;
	unsigned int generator_seed;
} typedef SchedulerConfig;

void generate_task_queue(TaskDescription* task_queue, const SchedulerConfig* config, const NetworkDescription* network);

#endif