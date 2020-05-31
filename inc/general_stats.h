#include "../inc/definitions.h"

void global_average(float newsample, int infinite_counter);
void detect_peak(float newsample);
void detect_core_peak(double* newsample);
void detect_pidcore_peak(UINT8 cpuid, float newsample);
void report_dump(int sig);
void init_stats();
