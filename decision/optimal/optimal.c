#include "optimal.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <omp.h>

char* decisions = NULL;
int decision_count = 0;
int decision_index = 0;

#define DISABLE_OPTIMAL 1
#define BLOCK_SIZE 10

static uint32_t solve_block(const DeviceDescriptions* dev, const TaskDescription* task_window, 
	uint32_t window_size, int last_prev, double* total_max_delay, double* cumulative_delay)
{
	// Add full local execution delay to total_max_delay
	double local_sum = 0.0;
	for (uint32_t i = 0; i < window_size; i++)
	{
		int prev = i == 0 ? last_prev : 0;
		DecisionFactors f = calculate_factors(dev, &task_window[i], prev);
		local_sum += f.delay_local;
	}
	*total_max_delay += local_sum;

	uint32_t best_mask = 0U;
	double best_energy = DBL_MAX;
	double best_delay = 0.0;

	#pragma omp parallel
	{
		uint32_t smallest_found = 0U;
		double min_block_energy = DBL_MAX;
		double min_block_delay = 0.0;

		int mask;
		#pragma omp for nowait
		for (mask = 0; mask < (1 << window_size); mask++)
		{
			uint32_t i = (uint32_t)mask;
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

			if (sum_energy < min_block_energy && (*cumulative_delay) + sum_delay <= (*total_max_delay))
			{
				smallest_found = i;
				min_block_energy = sum_energy;
				min_block_delay = sum_delay;
			}
		}

		#pragma omp critical
		{
			if (min_block_energy < best_energy)
			{
				best_energy = min_block_energy;
				best_delay = min_block_delay;
				best_mask = smallest_found;
			}
		}
	}
	*cumulative_delay += best_delay;
	return best_mask;
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

	#if DISABLE_OPTIMAL
	return;
	#endif

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
