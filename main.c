#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "all_cores.h"


int NUMBER_OF_SAMPLES_PER_MINUTE = 60;
int CPU_LIMIT = 5;
int DECISION = 0;

static double _cum_global_counter = 0;
static double _peak = 0;
static double _average = 0;
static double _global_average = 0;

static time_t  tick;
static time_t  tock;
static time_t  tack;

double getCurrentValue();
int calculate_average(double* CPU_Measurements, int index);
bool read_parameters();
void restart();
void global_average(float newsample, int infinite_counter);
void detect_peak(float newsample);
void report_dump(int sig);

static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

void init(){
    FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
    fclose(file);
}

int main(){
	printf("CPU BENCHMARK - ©By RSA© - All rights reserved\n");

	if(!read_parameters()){
		printf("ERROR - PROGRAM HAS INVALID OR UNKNOWN PARAMETERS\n");		
		return 1;	
	}
	signal(SIGINT, report_dump);
	signal(SIGTERM, report_dump);
	signal(SIGKILL, report_dump);

	init();
	
	double CPU_Measurements[NUMBER_OF_SAMPLES_PER_MINUTE];
	double peak = 0;
	u_int64_t infinite_counter = 0; 
	double newsample = 0;
    	double *percent;

	int average = 0;
	bool halfsample = true;
	memset(CPU_Measurements, 0, sizeof(double)*NUMBER_OF_SAMPLES_PER_MINUTE);
	
	printf("Starting CPU load Monitoring\n Frequency set to: %d \n", NUMBER_OF_SAMPLES_PER_MINUTE);
	//printf("CPU Load is: %lf\n", getCurrentValue()); //Debug message
	time(&tick);	

	FILE *fptr = fopen("fulldump.txt", "w"); 
    	if (fptr == NULL){
 
	        printf("Could not open file"); 
	        return 0; 
	}
	//gets number of cpus
	int cpus = cpucount();
	char name[20], filename[20];
	FILE * cpuptr[10];

	for(int n=0; n< cpus; n++){
		sprintf(name, "logcpu%d", n);
		printf("name: %s", name);
		cpuptr[n] = fopen(name, "w");
	    	if (cpuptr[n] == NULL){
		        printf("Could not open file"); 
		        return 0; 
		}
	}
	//fputs("CPU Samples: ", fptr);
	
	for(;;){
		
		for(int index=0; index<NUMBER_OF_SAMPLES_PER_MINUTE; index++){			
			usleep(60000000/(NUMBER_OF_SAMPLES_PER_MINUTE*1));
			//free running counter
			infinite_counter++;
			
			//reads CPU sample
			newsample = getCurrentValue();
			//getting timestamp
			time(&tock);
			fprintf(fptr, "%d ",(int)(tock - tick));
			fprintf(fptr, "%lf\n",newsample);	
#ifdef DEBUG
			printf("New CPU Sample: %lf\n",newsample);
#endif
			//CPU Sample operations - cpu peak calculation
			detect_peak(newsample);	
			
			//CPU Sample operations - last minute average
			CPU_Measurements[index] = newsample;
			//CPU Sample operations - global average
			global_average(newsample, infinite_counter);
			
			//CPU Sample operations - individual usage
			time(&tack);
			percent = cpuusage();
        		for (int n = 0; n < cpus; n++){
            			//printf("cpu%d:%.2f%%\n", n, percent[n]);
				fprintf(cpuptr[n], "%d ",(int)(tack - tick));
				fprintf(cpuptr[n], "%lf\n", percent[n]);
        		}		
			
//uncomment the following lines to enable last minute average
			/*if(halfsample){
				average = calculate_average(CPU_Measurements, index);
			} else {
				average = calculate_average(CPU_Measurements, NUMBER_OF_SAMPLES_PER_MINUTE);
				if(DECISION == 0){				
					if(average < CPU_LIMIT){
						//restart();
					}
				} else{
					if(average > CPU_LIMIT){
						//restart();
					}
				}
				
			}*/
			
		}
		halfsample = false;
	}

	return 0;

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

int calculate_average(double* CPU_Measurements, int index){
	float average;
	if(CPU_Measurements == NULL){
		printf("Error, sample array is empty\n");
		return 0;	
	}	

	for(int i=0; i<=index; i++){
			average += CPU_Measurements[i];
			
	}	
	
	average = average/((float)(index+1));
	_average = average;
#ifdef DEBUG
	printf("Average CPU load: %lf\n", average);
#endif
	return average;
}



bool read_parameters(){
	
	volatile char tmpstr1[50];
	volatile char tmpstr2[50];
	
	FILE * fr = fopen("parameters.txt", "rt");
	char tempbuff[500];

	if(fr == NULL){
		printf("File not found\n");
		return false;
	}

	 while(!feof(fr)){ 

		if (fgets(tempbuff,100,fr)) {
			
			//printf("DEBUG is: %s \n", tempbuff);

			sscanf(tempbuff, "%50s : %50s", tmpstr1, tmpstr2);
			//printf("value read is: %s and %s\n", tmpstr1, tmpstr2);
           		if (strcmp((const char*)tmpstr1, "nsamplesperminute") == 0){

			   NUMBER_OF_SAMPLES_PER_MINUTE = atoi((const char*)tmpstr2);
			   if(NUMBER_OF_SAMPLES_PER_MINUTE > 60){
			   	printf("Your sample rate per minute is too high. This may have an impact in the performance. Please lower this parameter\n");
			   }
			}
			else if (strcmp((const char*)tmpstr1, "usagelimit") == 0){
			    CPU_LIMIT = atoi((const char*)tmpstr2);
			}

			else if (strcmp((const char*)tmpstr1, "comparison") == 0){
			    DECISION = atoi((const char*)tmpstr2);
			}
			
			else {
			    printf("Parameter not recognized\n");
			    return false;
			}

		}
	}

return true;
}

void restart(){
	printf("WILL REBOOT NOW\n");
	sync(); sync(); sync(); 
	reboot(RB_AUTOBOOT);
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

double getCurrentValue(){
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
