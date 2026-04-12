#ifndef TASK_H
#define TASK_H

#include <stdint.h>

struct task_description_t
{
	int32_t task_input_size; // bytes
	int32_t task_output_size; // bytes
	double task_computation_size; // cycles
	int dependent; // 0 for independent, 1 for dependent on previous task
	int offloadable; // 0 if can only be executed locally, 1 otherwise
};
typedef struct task_description_t TaskDescription;

#endif