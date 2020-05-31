#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../inc/all_cores.h"
#include "../inc/pid_usage.h"
#include "../inc/general_stats.h"

static unsigned long int pid_usage, pid_usage_old;
static unsigned long int cpu_usage, cpu_usage_old;
static u_int8_t num_cpus;
static int pid_to_monitor;
static FILE * all_cores_pid_fileptr[8];

bool pid_usage_init(int pidmonitor){
	pid_usage = 0;
	pid_usage_old = 0;
	cpu_usage = 0;
	cpu_usage_old = 0;
	num_cpus = cpucount();
	pid_to_monitor = pidmonitor;
	char name[50];

	for(int n=0; n< num_cpus; n++){
		sprintf(name, "logpid_cpu%d", n);
		all_cores_pid_fileptr[n] = fopen(name, "w");
	    	if (all_cores_pid_fileptr[n] == NULL){
		        printf("Could not open file"); 
		        return false; 
		}
	}
	return true;

}

unsigned long int get_process_usage(int pid, u_int8_t* cpu_id){
	FILE* file;
	char filepath[20];
	char dummystr[10];
	int dummyint;
	unsigned long int usrtime, systime;

	sprintf(filepath, "/proc/%d/stat", pid);

	file = fopen(filepath, "r");
	if(file == NULL){
		printf("Proccess is not running\n Exiting\n");
		report_dump(0);
		exit(0);		
	}
	fscanf(file, "%d %s %c %d %d %d %d %d %d %d %d %d %d %lu %lu %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %hhd",
		 &dummyint, dummystr, dummystr, &dummyint, &dummyint, &dummyint,
		 &dummyint, &dummyint, &dummyint, &dummyint, &dummyint, &dummyint,
		 &dummyint, &usrtime, &systime, &dummyint, &dummyint, &dummyint, 
		&dummyint, &dummyint, &dummyint, &dummyint, &dummyint, &dummyint, 
		&dummyint, &dummyint, &dummyint, &dummyint, &dummyint, &dummyint,
		 &dummyint, &dummyint, &dummyint, &dummyint, &dummyint, &dummyint, 
		&dummyint, &dummyint, cpu_id);

	//printf("cpu used: %d", cpu_id);
	//printf("usertime: %lu\n systime: %lu", usrtime, systime);
	return usrtime+systime;

}

unsigned long long get_globalcpu_time_usage(){
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
        &totalSys, &totalIdle);
    fclose(file);

    
    return totalUser+totalUserLow+totalSys+totalIdle;
}

float get_pid_usage(u_int8_t *cpu_id){
	float usage = 0.0;

	pid_usage_old = pid_usage;
	cpu_usage_old = cpu_usage;
	
	pid_usage=get_process_usage(pid_to_monitor, cpu_id);
	cpu_usage=get_globalcpu_time_usage();
			
	int cpu_delta=cpu_usage-cpu_usage_old;
	int pid_delta=pid_usage-pid_usage_old;

	//printf("cpudelta: %d\n", cpu_delta);
	//printf("piddelta: %d\n", pid_delta);

	if(!cpu_delta){
		return 0.0;
	}

	usage=((pid_delta)*100*num_cpus)/(float)(cpu_delta);
	//printf("process usage is: %lf on cpu: %hhd", usage, *cpu_id);
	return usage;

}

float get_pid_global_usage(float pid_usage){
	float usage=pid_usage/num_cpus;
	return usage;

}

void dump_pid_report(int timestamp, float usage, u_int8_t cpuid){
	
	for (int n = 0; n < num_cpus; n++){
		if(n == cpuid){
			fprintf(all_cores_pid_fileptr[n], "%d ",timestamp);
			fprintf(all_cores_pid_fileptr[n], "%lf\n", usage);					
		}else{
			fprintf(all_cores_pid_fileptr[n], "%d ",timestamp);
			fprintf(all_cores_pid_fileptr[n], "0\n");		
		}
		
        }	
}



