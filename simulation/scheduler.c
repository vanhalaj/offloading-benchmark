#include "scheduler.h"

#include <stdlib.h>

#include "profiled_estimate.h"

double (*task_estimates[])(int) = {
	estimate_e1_complexity
};
const int TASK_FUNCTIONS_COUNT = sizeof(task_estimates) / sizeof(task_estimates[0]);

void generate_task_queue(TaskDescription* task_queue, int task_count, int min_size, int max_size, unsigned int seed)
{
	srand(seed);

	for (int i = 0; i < task_count; i++)
	{
		int size = min_size + (rand() % (max_size - min_size));
		int type = rand() % TASK_FUNCTIONS_COUNT;

		double complexity = task_estimates[type](size);

		TaskDescription task = {
			.task_input_size = size,
			.task_output_size = size,
			.task_computation_size = complexity * 8.0
		};
		task_queue[i] = task;
	}
}
