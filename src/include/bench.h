#ifndef BENCH_H
#define BENCH_H

#include <time.h>

/* Number of benchmark iterations */

#define WORKLOAD 10000

/* Range of random keys */

#define RAND_RANGE 100

typedef struct return_values
{
	double time[9];
}return_values;

/* Mixed benchmark implementation */

double run_mixed_bench(int t, int p);

return_values set_mixed_bench_50_del(void);
return_values set_mixed_bench_80_del(void);
return_values set_mixed_bench_20_del(void);

/* Delete-only benchmark implementation */

double run_delete_only_bench(int t);
return_values set_delete_only_bench(void);

/* Insert-only benchmark implementation */

double run_insert_only_bench(int t);
return_values set_insert_only_bench(void);

/* Common functions */

void fill_queue(int n);
void small_bench(void);

#endif
