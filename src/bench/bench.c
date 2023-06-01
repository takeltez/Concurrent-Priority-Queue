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
	int i;
	int key;

	srand(time(NULL));

	for(i = 0; i < n; i++)
	{
		key = rand() % UINT32_MAX + 1;

		insert(key);
	}
}

/**
 * void run_bench(void)
 * 
 * Run all benchmarks.
 * 
 * **/
void run_bench(void)
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
