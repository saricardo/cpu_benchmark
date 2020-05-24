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
#include "general_cores.h"
#include "general_stats.h"


int NUMBER_OF_SAMPLES_PER_MINUTE = 60;
int CPU_LIMIT = 5;
int DECISION = 0;

static double _average = 0;


static time_t  tick;
static time_t  tock;
static time_t  tack;


int calculate_average(double* CPU_Measurements, int index);
bool read_parameters();
void restart();

int main(){
	printf("CPU BENCHMARK - ©By RSA© - All rights reserved\n");

	if(!read_parameters()){
		printf("ERROR - PROGRAM HAS INVALID OR UNKNOWN PARAMETERS\n");		
		return 1;	
	}
        //termination signals registration
	signal(SIGINT, report_dump);
	signal(SIGTERM, report_dump);
	signal(SIGKILL, report_dump);

	if(!general_cores_init()){
		printf("Error initializing");		
		return 1;
	}
	if(!init_all_cores()){
		printf("Error initializing");		
		return 1;
	} 
	init_stats();
	
	
	//local inits
	double CPU_Measurements[NUMBER_OF_SAMPLES_PER_MINUTE];
        memset(CPU_Measurements, 0, sizeof(double)*NUMBER_OF_SAMPLES_PER_MINUTE);

	u_int64_t infinite_counter = 0; 
	double newsample = 0;
    	double *all_newsample;
	int average = 0;
	bool halfsample = true;	
	
	printf("Starting CPU load Monitoring\n Frequency set to: %d \n", NUMBER_OF_SAMPLES_PER_MINUTE);
	time(&tick);	
	
	
	
	//fputs("CPU Samples: ", fptr);
	
	for(;;){
		
		for(int index=0; index<NUMBER_OF_SAMPLES_PER_MINUTE; index++){			
			usleep(60000000/(NUMBER_OF_SAMPLES_PER_MINUTE*1));
			//free running counter
			infinite_counter++;
			
			//read CPU sample
			newsample = get_global_cpu_usage();
			//reading timestamp
			time(&tock);
			//log the sample
			general_cores_log((int)(tock - tick), &newsample);
				
#ifdef DEBUG
			printf("New CPU Sample: %lf\n",newsample);
#endif

			//CPU Sample operations - cpu peak calculation
			detect_peak(newsample);	
			
			//CPU Sample operations - last minute average
			CPU_Measurements[index] = newsample;

			//CPU Sample operations - global average
			global_average(newsample, infinite_counter);
			

			//read individual samples		
			all_newsample = cpuusage();
			//reading the timestamp
			time(&tack);
			//log the samples from all cpus
			all_cpus_log((int)(tack - tick), all_newsample);
        			
			
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




