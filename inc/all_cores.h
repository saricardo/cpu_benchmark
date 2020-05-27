#include <stdbool.h>

double *cpuusage(void);
int cpucount(void);
void *emalloc(size_t size);
void eprintf(const char *fmt, ...);
bool all_cpus_log(int timestamp, double * cpu_log);
bool init_all_cores();

