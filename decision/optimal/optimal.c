#include "optimal.h"

#include <stdlib.h>
#include <stdio.h>

char* decisions = NULL;
int decision_count = 0;
int decision_index = 0;

void optimal_prepare(TaskDescription* tasks, int task_count)
{
	optimal_free();
	decisions = calloc(task_count, sizeof(char));
	if (!decisions)
	{
		printf("Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	decision_count = task_count;

	// TODO: find optimal solution and populate decisions with ones and zeroes
	for (int task = 0; task < task_count; task++)
	{
		decisions[task] = task & 1;
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
