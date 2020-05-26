unsigned long long get_globalcpu_time_usage();
bool pid_usage_init(int pidmonitor);
void dump_pid_report(int timestamp, float usage, u_int8_t cpuid);
float get_pid_usage(u_int8_t *cpu_id);
unsigned long int get_process_usage(int pid, u_int8_t* cpu_id);
