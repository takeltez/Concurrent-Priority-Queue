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
	omp_set_num_threads(1);

	#pragma omp parallel
	{
		uint32_t tid;

		tid = omp_get_thread_num();

		srand(tid);

		#pragma omp for
		for(int i = 0; i < n; i++)
		{
			int key = rand_r(&tid) % RAND_RANGE + 1;

			insert(key);
		}
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
