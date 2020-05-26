#include <stdio.h>
#include <stdlib.h>
#include "../inc/general_stats.h"

static float _peak;
static float _global_average;
static float _cum_global_counter;

void init_stats(){
	_peak = 0.0;
	_global_average = 0.0;
	_cum_global_counter = 0.0;
}

void detect_peak(float newsample){

	if(newsample > _peak){
		_peak = newsample;
	}
#ifdef DEBUG
	printf("Peak CPU load: %lf\n", _peak);
#endif
}


void global_average(float newsample, int infinite_counter){

	_cum_global_counter = _cum_global_counter + newsample;
	_global_average = _cum_global_counter / ((float)infinite_counter);

#ifdef DEBUG
	printf("Global CPU load: %lf\n", _global_average);
#endif

}


void report_dump(int sig){
	FILE *fptr = fopen("cpu_report.txt", "w"); 
  	if (fptr == NULL){ 
	        printf("Could not open file"); 
	        return; 
	}
	fputs("CPU load average:", fptr);
	fprintf(fptr, "%lf%%\n",_global_average);
	//fputs("CPU last minute load average:", fptr);
	//fprintf(fptr, "%lf%%\n",_average);
	fputs("CPU load peak:", fptr);
	fprintf(fptr, "%lf%%\n",_peak);
	exit(0);
}
