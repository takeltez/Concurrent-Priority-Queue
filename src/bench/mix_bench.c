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
		if (i < (WORKLOAD - p))
		{
			insert(keys[i], vals[i]);
		}
		else
		{
			deleteMin();
		}
	}

	toc = omp_get_wtime();

	free(keys);
	free(vals);

	return toc - tic;
}

/**
 * void set_mixed_bench_50_del(void)
 * 
 * Prepare Mixed benchmark with 50% deleteMin() operation persentage for run.
 * 
 * **/
return_values set_mixed_bench_50_del(void)
{
	int i, j, p;
	double res;

	return_values result;

	// Set deleteMin() operation percentage
	p = WORKLOAD * 0.5;

	printf("\n#1 Mixed workload with 50%% of DeleteMin() operations\n\n");

	for(i = 1, j = 0; i <= 64; i += 8, j++)
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

		// Run mixed benchmark with 50/50 operations persentage.
		res = run_mixed_bench(i, p);

		printf("\tElapsed time: %f sec.\n", res);
		printf("\t---end test---\n\n");

		result.time[j] = res;

		// Destroy queue
		destroy_queue();
	}

	return result;
}

/**
 * void set_mixed_bench_80_del(void)
 * 
 * Prepare Mixed benchmark with 80% deleteMin() operation persentage for run.
 * 
 * **/
return_values set_mixed_bench_80_del(void)
{
	int i, j, p;
	double res;

	return_values result;

	// Set deleteMin() operation percentage
	p = WORKLOAD * 0.8;

	printf("\n#2 Mixed workload with 80%% of DeteleMin() operations\n\n");

	for(i = 1, j = 0; i <= 64; i += 8, j++)
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

		// Run mixed benchmark with 80% deleteMin() operation persentage.
		res = run_mixed_bench(i, p);

		printf("\tElapsed time: %f sec.\n", res);
		printf("\t---end test---\n\n");

		result.time[j] = res;

		// Destroy queue
		destroy_queue();
	}

	return result;
}

/**
 * void set_mixed_bench_20_del(void)
 * 
 * Prepare Mixed benchmark with 20% deleteMin() operation persentage for run.
 * 
 * **/
return_values set_mixed_bench_20_del(void)
{
	int i, j, p;
	double res;

	// Set deleteMin() operation percentage
	p = WORKLOAD * 0.2;

	return_values result;

	printf("\n#3 Mixed workload with 20%% of DeteleMin() operations\n\n");

	for(i = 1, j = 0; i <= 64; i += 8, j++)
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
		res = run_mixed_bench(i, p);

		printf("\tElapsed time: %f sec.\n", res);
		printf("\t---end test---\n\n");

		result.time[j] = res;

		// Destroy queue
		destroy_queue();
	}

	return result;
}
