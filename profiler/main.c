#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <intrin.h>
#endif

#include "task_examples.h"

static inline unsigned long long get_cycle_count(void)
{
#ifdef _MSC_VER
	return __rdtsc();
#else
#ifdef PLATFORM_DESKTOP
	unsigned hi, lo;
	__asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
	return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
#endif
#endif
}

int main(void)
{
	const int row_count = 16;
	const int col_count = 16;
	float* matrix = malloc(row_count * col_count * sizeof(float));
	float* result = malloc(row_count * row_count * sizeof(float));

	if (!matrix || !result) 
	{
		exit(1);
	}

	for (int i = 0; i < row_count * col_count; ++i)
	{
		matrix[i] = (float)rand() / (float)RAND_MAX;
	}

	for (int i = 0; i < 16; ++i)
	{
		unsigned long long start_cycles = get_cycle_count();

		matrix_transpose_multiplication(matrix, row_count, col_count, result);

		unsigned long long end_cycles = get_cycle_count();
		unsigned long long cycle_diff = end_cycles - start_cycles;

		printf("Cycles: %llu\n", cycle_diff);
	}

	free(matrix);
	free(result);
	return 0;
}