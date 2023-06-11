#include "queue.h"
#include "bench.h"

/**
 * double run_delete_only_bench(int t)
 * 
 * Run Delete-only benchmark.
 * 
 * Parameters:
 * 	@t - number of threads.
 * 
 * Returned value:
 * @tic-toc - elapsed time for running benchmark.
 * 
 * **/
double run_delete_only_bench(int t)
{
	double tic, toc;

	omp_set_num_threads(t);

	tic = omp_get_wtime();

	#pragma omp parallel for
	for(int i = 0; i < WORKLOAD; i++)
	{
		deleteMin();
	}

	toc = omp_get_wtime();

	return toc - tic;
}

/**
 * void set_delete_only_bench(void)
 * 
 * Prepare Delete-only benchmark for run.
 * 
 * **/
void set_delete_only_bench(void)
{
	int i = 1;
	double res;

	printf("Delete-only benchmark:\n");

	for(i = 1; i <= 64; i += 8)
	{
		if(i == 9)
		{
			i--;
		}

		// Create queue
		create_queue();

		// Prepare queue for test
		fill_queue(WORKLOAD);

		// Run mixed benchmark with 20% deleteMin() operation persentage
		res = run_delete_only_bench(i);

		printf("\tNumber of threads: %d\n\tElapsed time: %f\n\n", i, res);

		// Destroy queue
		destroy_queue();
	}
}
