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

	printf("\n#4 Deletion-only workload\n\n");

	for(i = 1; i <= 64; i += 8)
	{
		if(i == 9)
		{
			i--;
		}

		// Create queue
		create_queue();

		printf("Filling queue for benchmark test...\n\n");

		// Prepare queue for test
		fill_queue(WORKLOAD);

		printf("\t---start test---\n");
		printf("\tNumber of threads: %d\n", i);

		// Run mixed benchmark with 20% deleteMin() operation persentage
		res = run_delete_only_bench(i);

		printf("\tElapsed time: %f sec.\n", res);
		printf("\t---end test---\n\n");

		// Destroy queue
		destroy_queue();
	}
}
