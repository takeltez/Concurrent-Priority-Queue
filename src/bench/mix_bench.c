#include "queue.h"
#include "bench.h"

/**
 * double run_mixed_bench(int t, int p)
 * 
 * Run Mixed benchmark.
 * 
 * Parameters:
 * 	@t - number of threads.
 * 	@p - insert()/deleteMin() operation percentage.
 * 
 * Returned value:
 * @tic-toc - elapsed time for running benchmark.
 * 
 * **/
double run_mixed_bench(int t, int p)
{
	double tic, toc;
	int keys[WORKLOAD];

	// Prepare keys for insertion
	for(int i = 0; i < WORKLOAD; i++)
	{
		keys[i] = rand() % RAND_RANGE + 1;
	}

	omp_set_num_threads(t);

	tic = omp_get_wtime();

	#pragma omp parallel for
	for(int i = 0; i < WORKLOAD; i++)
	{
		if (i < (WORKLOAD - p))
		{
			insert(keys[i]);
		}
		else
		{
			deleteMin();
		}
	}

	toc = omp_get_wtime();

	return toc - tic;
}

/**
 * void set_mixed_bench_half(void)
 * 
 * Prepare Mixed benchmark with 50/50 operations persentage for run.
 * 
 * **/
void set_mixed_bench_half(void)
{
	int i = 1;
	int p;
	double res;

	// Create queue
	create_queue();

	// Prepare queue for test inserting 1.000.000 random keys
	fill_queue(1000000);

	// Set operation percentage
	p = WORKLOAD * 0.5;

	printf("Mixed benchmark with 50/50 operations persentage:\n");

	// Run mixed benchmark with 50/50 operations persentage for 1 thread.
	res = run_mixed_bench(i, p);

	printf("\tNumber of threads: %d\t\nElapsed time: %f\n\n", i, res);

	for(i = 8; i <= 64; i += 8)
	{
		// Run mixed benchmark with 50/50 operations persentage for 8+ threads.
		res = run_mixed_bench(i, p);

		printf("\tNumber of threads: %d\t\nElapsed time: %f\n\n", i, res);
	}

	// Destroy queue
	destroy_queue();
}

/**
 * void set_mixed_bench_80_del(void)
 * 
 * Prepare Mixed benchmark with 80% deleteMin() operation persentage  for run.
 * 
 * **/
void set_mixed_bench_80_del(void)
{
	int i = 1;
	int p;
	double res;

	// Create queue
	create_queue();

	// Prepare queue for test inserting 10.000.000 random keys
	fill_queue(10000000);

	// Set operation percentage
	p = WORKLOAD * 0.8;

	printf("Mixed benchmark with 80%% deleteMin() operation persentage:\n");

	// Run mixed benchmark with 80% deleteMin() operation persentage for 1 thread.
	res = run_mixed_bench(i, p);

	printf("\tNumber of threads: %d\t\nElapsed time: %f\n\n", i, res);

	for(i = 8; i <= 64; i += 8)
	{
		// Run mixed benchmark with 80% deleteMin() operation persentage for 8+ threads.
		res = run_mixed_bench(i, p);

		printf("\tNumber of threads: %d\t\nElapsed time: %f\n\n", i, res);
	}

	// Destroy queue
	destroy_queue();
}

/**
 * void set_mixed_bench_20_del(void)
 * 
 * Prepare Mixed benchmark with 20% deleteMin() operation persentage  for run.
 * 
 * **/
void set_mixed_bench_20_del(void)
{
	int i = 1;
	int p;
	double res;

	// Create queue
	create_queue();

	// Prepare queue for test inserting 10.000.000 random keys
	fill_queue(10000000);

	// Set operation percentage
	p = WORKLOAD * 0.2;

	printf("Mixed benchmark with 20%% deleteMin() operation persentage:\n");

	// Run mixed benchmark with 20% deleteMin() operation persentage for 1 threads.
	res = run_mixed_bench(i, p);

	printf("\tNumber of threads: %d\t\nElapsed time: %f\n\n", i, res);

	for(i = 8; i <= 64; i += 8)
	{
		// Run mixed benchmark with 20% deleteMin() operation persentage for 8+ threads.
		res = run_mixed_bench(i, p);

		printf("\tNumber of threads: %d\t\nElapsed time: %f\n\n", i, res);
	}
	
	// Destroy queue
	destroy_queue();
}
