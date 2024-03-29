#include "queue.h"
#include "bench.h"

/**
 * void fill_queue(int n)
 * 
 * Add n random key/value pairs to queue.
 * 
 * Parameters:
 * 	@n - number of keys
 * 
 * **/
void fill_queue(int n)
{
	int *keys, *vals;

	keys = (int*)malloc(n * sizeof(int));
	vals = (int*)malloc(n * sizeof(int));

	srand(time(NULL));

	// Prepare key/value pairs for insertion
	for(int i = 0; i < n; i++)
	{
		keys[i] = rand() % RAND_RANGE + 1;
		vals[i] = rand() % RAND_RANGE + 1;
	}

	omp_set_num_threads(16);

	#pragma omp parallel for
	for(int i = 0; i < n; i++)
	{
		insert(keys[i], vals[i]);
	}

	free(keys);
	free(vals);
}

/**
 * void run_bench(void)
 * 
 * Run all benchmarks.
 * 
 * **/
void small_bench(void)
{
	double tic, toc;

	tic = omp_get_wtime();

	// Mixed benchmark
	set_mixed_bench_50_del();
	set_mixed_bench_80_del();
	set_mixed_bench_20_del();

	//Delete-only benchmark
	set_delete_only_bench();

	//Insert-only benchmark
	set_insert_only_bench();

	toc = omp_get_wtime();

	printf("Absolute benchmark time: %f sec.\n", toc - tic);
}
