#include "optimal.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>

char* decisions = NULL;
int decision_count = 0;
int decision_index = 0;

#define BLOCK_SIZE 12

static uint32_t solve_block(const DeviceDescriptions* dev, const TaskDescription* task_window, 
	uint32_t window_size, int last_prev, double* total_max_delay, double* cumulative_delay)
{
	uint32_t smallest_found = 0U;
	double min_block_energy = DBL_MAX;
	double min_block_delay = 0.0;

	for (uint32_t i = 0; i < (1U << window_size); i++)
	{
		double sum_delay = 0.0;
		double sum_energy = 0.0;
		for (uint32_t j = 0; j < window_size; j++)
		{
			uint32_t previous_decision = j > 0 
				? (i >> (j - 1)) & 1
				: last_prev; // stitching windows
			DecisionFactors f = calculate_factors(dev, &task_window[j], previous_decision);

			uint32_t current_decision = (i >> j) & 1;
			if (current_decision)
			{
				sum_delay += f.delay_offloaded;
				sum_energy += f.energy_offloaded;
			}
			else
			{
				sum_delay += f.delay_local;
				sum_energy += f.energy_local;
			}
		}

		if (i == 0)
		{
			*total_max_delay += sum_delay; // delay can be max as big as in full local execution
			//min_block_energy = sum_energy; // for some reason makes it so that delay is not respected
		}

		if (sum_energy < min_block_energy && (*cumulative_delay) + sum_delay <= (*total_max_delay))
		{
			smallest_found = i;
			min_block_energy = sum_energy;
			min_block_delay = sum_delay;
		}
	}
	*cumulative_delay += min_block_delay;
	return smallest_found;
}

void optimal_prepare(const DeviceDescriptions* dev, const TaskDescription* tasks, int task_count)
{
	optimal_free();
	decisions = calloc(task_count, sizeof(char));
	if (!decisions)
	{
		printf("Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	decision_count = task_count;

	double max_delay = 0.0;
	double cumulative_delay = 0.0;
	for (int block_start = 0, last_prev = 0; block_start < decision_count; block_start += BLOCK_SIZE)
	{
		uint32_t window_size = min(decision_count - block_start, BLOCK_SIZE);
		uint32_t result_block = solve_block(dev, &tasks[block_start], window_size, last_prev, &max_delay, &cumulative_delay);
		for (uint32_t i = 0; i < window_size; i++)
		{
			uint32_t decision = (result_block >> i) & 1;
			decisions[block_start + i] = decision;
			if (i == window_size - 1) 
				last_prev = decision;
		}
	}
}

void optimal_free()
{
	if (decisions)
	{
		free(decisions);
		decisions = NULL;
	}
	decision_count = 0;
	decision_index = 0;
}

int optimal_decision(const DecisionFactors* factors)
{
	if (!decisions)
	{
		printf("Optimal decision not allocated\n");
		exit(EXIT_FAILURE);
	}
	if (decision_index == decision_count)
	{
		printf("Optimal decision index out of bounds\n");
		exit(EXIT_FAILURE);
	}

	int decision = (int)decisions[decision_index];
	decision_index++;
	return decision;
}
