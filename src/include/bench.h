#ifndef BENCH_H
#define BENCH_H

#include <time.h>

#define WORKLOAD 1000000

/* Mixed benchmark implementation */

double run_mixed_bench(int t, int p);

void set_mixed_bench_half(void);
void set_mixed_bench_80_del(void);
void set_mixed_bench_20_del(void);

/* Delete-only benchmark implementation */

double run_delete_only_bench(int t);
void set_delete_only_bench(void);

/* Insert-only benchmark implementation */

double run_insert_only_bench(int t);
void set_insert_only_bench(void);

/* Common functions */

void fill_queue(int n);
void run_bench(void);

#endif
