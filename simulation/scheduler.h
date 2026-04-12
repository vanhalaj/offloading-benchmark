#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

void generate_task_queue(TaskDescription* task_queue, int task_count, int min_size, int max_size, unsigned int seed);

#endif