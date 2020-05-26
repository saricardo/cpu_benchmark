#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "../inc/general_cores.h"

#define PROCSTATFILE "/proc/stat"

static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
static FILE *general_dump_fileptr;

bool general_cores_init(){
    /*init /proc/stat related */
    FILE* proc_stat_fileptr = fopen(PROCSTATFILE, "r");
    if (proc_stat_fileptr == NULL){ 
	printf("Could not open /proc/stat"); 
        return false;
    }
    fscanf(proc_stat_fileptr, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
    fclose(proc_stat_fileptr);

    /*init dump log related*/
    general_dump_fileptr = fopen("fulldump.txt", "w"); 
    if (general_dump_fileptr == NULL){ 
	printf("Could not open fulldump.txt"); 
        return false;
    }

    return true;
}

bool general_cores_log(int timestamp, double* newsample){

	fprintf(general_dump_fileptr, "%d ",timestamp);
	fprintf(general_dump_fileptr, "%lf\n", *newsample);
	return true;

}

double get_global_cpu_usage(){
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
        &totalSys, &totalIdle);
    fclose(file);

    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
        totalSys < lastTotalSys || totalIdle < lastTotalIdle){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else{
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
            (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return percent;
}
