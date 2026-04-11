#include "task_examples.h"

void matrix_transpose_multiplication(float matrix[], int row_count, int col_count, float result[])
{
	for (int i = 0; i < row_count; ++i)
	{
		for (int j = 0; j < row_count; ++j)
		{
			float sum = 0.0f;

			for (int k = 0; k < col_count; ++k)
			{
				float a = matrix[i * col_count + k];
				float b = matrix[j * col_count + k];
				sum += a * b;
			}

			result[i * row_count + j] = sum;
		}
	}
}
