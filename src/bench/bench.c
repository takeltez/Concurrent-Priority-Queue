#include "queue.h"
#include "bench.h"

/**
 * void fill_queue(int n)
 * 
 * Add n random keys to queue.
 * 
 * Parameters:
 * 	@n - number of keys
 * 
 * **/
void fill_queue(int n)
{
	int keys[n];

	srand(time(NULL));

	for(int i = 0; i < n; i++)
	{
		keys[i] = rand() % RAND_RANGE + 1;
	}

	omp_set_num_threads(1);

	#pragma omp parallel for
	for(int i = 0; i < n; i++)
	{
		insert(keys[i]);
	}
}

/**
 * void run_bench(void)
 * 
 * Run all benchmarks.
 * 
 * **/
void small_bench(void)
{
	// Mixed benchmark
	set_mixed_bench_half();
	set_mixed_bench_80_del();
	set_mixed_bench_20_del();

	//Delete-only benchmark
	set_delete_only_bench();

	//Insert-only benchmark
	set_insert_only_bench();
}
