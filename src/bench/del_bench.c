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

	// Create queue
	create_queue();

	// Prepare queue for test inserting 10.000.000 random keys
	fill_queue(10000000);

	// Set operation percentage

	printf("Delete-only benchmark:\n");

	// Run mixed benchmark with 20% deleteMin() operation persentage for 1 thread.
	res = run_delete_only_bench(i);

	printf("\tNumber of threads: %d\t\nElapsed time: %f\n\n", i, res);

	for(i = 8; i <= 64; i += 8)
	{
		// Run mixed benchmark with 20% deleteMin() operation persentage for 8+ threads.
		res = run_delete_only_bench(i);

		printf("\tNumber of threads: %d\t\nElapsed time: %f\n\n", i, res);
	}
	
	// Destroy queue
	destroy_queue();
}
