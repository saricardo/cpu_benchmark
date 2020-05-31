#include <stdio.h>
#include <stdlib.h>
#include "../inc/general_stats.h"
#include "../inc/definitions.h"
#include "../inc/all_cores.h"

static float _peak;
static float _peaks[10];
static float _global_average;
static float _cum_global_counter;
static UINT8 _num_cores;

void init_stats(){
	_peak = 0.0;
	_global_average = 0.0;
	_cum_global_counter = 0.0;
	_num_cores=cpucount();
	for(UINT8 i=0; i<10; i++){
		_peaks[i]=0.0;	
	}
}

void detect_peak(float newsample){

	if(newsample > _peak){
		_peak = newsample;
	}
#ifdef DEBUG
	printf("Peak CPU load: %lf\n", _peak);
#endif
}

/*Individual core peak*/
void detect_core_peak(double* newsample){
	
	for(UINT8 i=0; i<_num_cores; i++){
		//printf("currpeak: %lf and currsample: %lf\n", _peaks[i], newsample[i]);
		if(newsample[i] > _peaks[i]){
			_peaks[i] = (float)newsample[i];
		}
	}
		}
/*Individual core peak per proccess*/
void detect_pidcore_peak(UINT8 cpuid, float newsample){
	
	if(newsample > _peaks[cpuid]){
		_peaks[cpuid] = newsample;
	}
	
}


void global_average(float newsample, int infinite_counter){

	_cum_global_counter = _cum_global_counter + newsample;
	_global_average = _cum_global_counter / ((float)infinite_counter);

#ifdef DEBUG
	printf("Global CPU load: %lf\n", _global_average);
#endif

}


void report_dump(int sig){
	FILE *fptr = fopen("cpu_report_stats.txt", "w"); 
  	if (fptr == NULL){ 
	        printf("Could not open file. Stats dump not generated\n"); 
	        return; 
	}
	fputs("CPU load average:", fptr);
	fprintf(fptr, "%lf%%\n",_global_average);
	//fputs("CPU last minute load average:", fptr);
	//fprintf(fptr, "%lf%%\n",_average);
	fputs("CPU load peak:", fptr);
	fprintf(fptr, "%lf%%\n",_peak);
	for(UINT8 i=0; i<_num_cores; i++){
		fprintf(fptr, "CPU %d load peak:", i);
		fprintf(fptr, "%lf%%\n",_peaks[i]);
	}	

	exit(0);
}
