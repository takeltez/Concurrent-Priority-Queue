#ifndef BENCH_H
#define BENCH_H

#include <time.h>

/* Number of benchmark iterations */

#define WORKLOAD 10000

/* Range of random keys */

#define RAND_RANGE 100

typedef struct threads_times
{
	double time[9];
}threads_times;

/* Mixed benchmark implementation */

double run_mixed_bench(int t, int p);

threads_times set_mixed_bench_50_del(void);
threads_times set_mixed_bench_80_del(void);
threads_times set_mixed_bench_20_del(void);

/* Delete-only benchmark implementation */

double run_delete_only_bench(int t);
threads_times set_delete_only_bench(void);

/* Insert-only benchmark implementation */

double run_insert_only_bench(int t);
threads_times set_insert_only_bench(void);

/* Common functions */

void fill_queue(int n);
void small_bench(void);

#endif
