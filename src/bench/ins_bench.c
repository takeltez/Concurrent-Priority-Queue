#include "queue.h"
#include "bench.h"

/**
 * double run_insert_only_bench(int t)
 * 
 * Run Insert-only benchmark.
 * 
 * Parameters:
 * 	@t - number of threads.
 * 
 * Returned value:
 * @tic-toc - elapsed time for running benchmark.
 * 
 * **/
double run_insert_only_bench(int t)
{
	double tic, toc;
	int *keys, *vals;

	keys = (int*)malloc(WORKLOAD * sizeof(int));
	vals = (int*)malloc(WORKLOAD * sizeof(int));

	srand(time(NULL));

	// Prepare key/value pairs for insertion
	for(int i = 0; i < WORKLOAD; i++)
	{
		keys[i] = rand() % RAND_RANGE + 1;
		vals[i] = rand() % RAND_RANGE + 1;
	}

	omp_set_num_threads(t);

	tic = omp_get_wtime();

	#pragma omp parallel for
	for(int i = 0; i < WORKLOAD; i++)
	{
		insert(keys[i], vals[i]);
	}

	toc = omp_get_wtime();

	free(keys);
	free(vals);

	return toc - tic;
}

/**
 * void set_insert_only_bench(void)
 * 
 * Prepare Insert-only benchmark for run.
 * 
 * **/
void set_insert_only_bench(void)
{
	int i = 1;
	double res;

	printf("\n#5 Insertion-only workload\n\n");

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
		res = run_insert_only_bench(i);

		printf("\tElapsed time: %f sec.\n", res);
		printf("\t---end test---\n\n");

		// Destroy queue
		destroy_queue();
	}
}
