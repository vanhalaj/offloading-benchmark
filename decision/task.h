#ifndef TASK_H
#define TASK_H

#include <stdint.h>

struct task_description_t
{
	int32_t task_input_size; // bytes
	int32_t task_output_size; // bytes
	double task_computation_size; // cycles
};
typedef struct task_description_t TaskDescription;

#endif