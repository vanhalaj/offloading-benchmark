#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _MSC_VER
#include <intrin.h>
#endif

#include "task_examples.h"

#define ITERATIONS_PER_INPUT_SIZE 64

struct profiler_result_t
{
	uint64_t median;
	int input_size_in_bytes;
} typedef ProfilerResult;

static inline uint64_t get_cycle_count(void)
{
#ifdef _MSC_VER
	return __rdtsc();
#else
#ifdef PLATFORM_DESKTOP
	unsigned hi, lo;
	__asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
	return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
#else
	printf("NOT SUPPORTED ON ARM\n");
	exit(1);
#endif
#endif
}

static int cmp_uint64(uint64_t a, uint64_t b) 
{
	if (a < b) return -1;
	if (a > b) return 1;
	return 0;
}

ProfilerResult profile_matrix_example(int input_size)
{
	const int row_count = input_size;
	const int col_count = row_count; // lets keep the matrices square for simplicity

	float* matrix = malloc(row_count * col_count * sizeof(float));
	float* result = malloc(row_count * row_count * sizeof(float));

	if (!matrix || !result)
	{
		printf("Allocation for matrix failed\n");
		exit(1);
	}

	for (int i = 0; i < row_count * col_count; ++i)
	{
		matrix[i] = (float)rand() / (float)RAND_MAX;
	}

	uint64_t results[ITERATIONS_PER_INPUT_SIZE] = { 0 };
	for (int i = 0; i < ITERATIONS_PER_INPUT_SIZE; ++i)
	{
		uint64_t start_cycles = get_cycle_count();

		matrix_transpose_multiplication(matrix, row_count, col_count, result);

		uint64_t end_cycles = get_cycle_count();
		uint64_t cycle_diff = end_cycles - start_cycles;
		results[i] = cycle_diff;
	}
	qsort(results, ITERATIONS_PER_INPUT_SIZE, sizeof(uint64_t), cmp_uint64);
	uint64_t median = results[ITERATIONS_PER_INPUT_SIZE / 2];

	free(matrix);
	free(result);

	ProfilerResult profile = {
		.median = median,
		.input_size_in_bytes = row_count * col_count * sizeof(float)
	};
	return profile;
}

int main(void)
{
	FILE* fp = fopen("results.csv", "w");
	if (!fp) {
		perror("Failed to open file");
		exit(1);
	}

	// CSV header
	fprintf(fp, "n,cycles\n");

	for (int input_size = 16; input_size < 512; input_size += 16)
	{
		ProfilerResult result = profile_matrix_example(input_size);

		printf("n: %d / input size: %d / median cycles: %llu\n", input_size, result.input_size_in_bytes, result.median);
		fprintf(fp, "%d,%llu\n", result.input_size_in_bytes, result.median);
	}

	fflush(fp);
	fclose(fp);

	return 0;
}