#include "queue.h"

int main(int argc, const char **argv)
{
	(void) argc;
	(void) argv;

	omp_set_num_threads(4);

	#pragma omp parallel
	{
		int i, max;

		for(i = 0, max = 20; i < MAX_CHUNKS; max += 20, i++)
		{
			if (i == MAX_CHUNKS - 1)
			{
				create_chunk(UINT32_MAX);
			}
			else
				create_chunk(max);
		}

		print_queue(head);
	}

	

	return 0;
}
