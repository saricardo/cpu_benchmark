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
#include "../inc/all_cores.h"
#include "../inc/general_cores.h"
#include "../inc/general_stats.h"
#include "../inc/pid_usage.h"
#include "../inc/definitions.h"


static int NUMBER_OF_SAMPLES_PER_MINUTE = 60;
static int CPU_LIMIT = 5;
static int DECISION = 0;


static double _average = 0;


int calculate_average(double* CPU_Measurements, int index);
bool read_parameters(UINT8 mode);
void restart();


int main(int argc, char *argv[]){

	printf("CPU BENCHMARK - ©By RSA© - All rights reserved\n");

	time_t  tack;
	time_t  teck;
	time_t  tick;
	time_t  tock;
	
	UINT8 mode = MODE_GENERIC; //default mode 
	UINT32 pidmonitor = 0;     //empty pid to follow
	
	//check and load program args	
	for(UINT8 i=0; i < argc; i++){
		if(argv[i] == NULL){
			printf("Argument %d is invalid\n", i);
			continue;	
		}
		if(i==1){
			mode = atoi(argv[i]);
		}else if(i==2){
			pidmonitor = atoi(argv[i]);
		}
	}
	//check usage
	if(mode == MODE_PROCCESS && pidmonitor == 0){
		printf("Wrong usage. Please specify a pid as arg 2\n");
		return 1;
	}	

	if(!read_parameters(mode)){
		printf("ERROR - PROGRAM HAS INVALID OR UNKNOWN PARAMETERS\n");		
		return 1;	
	}
        //termination signals registration
	signal(SIGINT, report_dump);
	signal(SIGTERM, report_dump);
	signal(SIGKILL, report_dump);
	
	//inits
	switch (mode){

    		case MODE_GENERIC:
      			// statements
			if(!general_cores_init()){
				printf("Error initializing general cores\n");		
				return 1;
			}
			if(!init_all_cores()){
				printf("Error initializing all cores\n");		
				return 1;
			} 
      		break;
		
		case MODE_PROCCESS:
      			// statements
			if(!pid_usage_init(pidmonitor)){
				printf("Error initializing pid monitor\n");		
				return 1;
			}
      		break;

		case MODE_MONITOR:
      		// statements
      		break;
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
	u_int8_t cpuid = 0;
	float pid_usage = 0.0;
	
	printf("Starting CPU load Monitoring\n Frequency set to: %d \n", NUMBER_OF_SAMPLES_PER_MINUTE);
	time(&tick);	
	
	//fputs("CPU Samples: ", fptr);
	
	
		
	switch (mode){

    	case MODE_GENERIC:
		for(;;){
			for(UINT8 i=0; i<NUMBER_OF_SAMPLES_PER_MINUTE; i++){
				usleep(60000000/(NUMBER_OF_SAMPLES_PER_MINUTE));
				infinite_counter++;
				//read CPU sample
/*CPU overall usage*/
				newsample = get_global_cpu_usage();
				//reading timestamp
				time(&tock);
				//log the sample
				general_cores_log((int)(tock - tick), &newsample);
#ifdef DEBUG
				printf("New CPU Sample: %lf\n",newsample);
#endif
/*CPU overall peak detection usage*/
				//CPU Sample operations - cpu peak calculation
				detect_peak(newsample);	
/*CPU overall average*/
				//CPU Sample operations - global average
				global_average(newsample, infinite_counter);
/*CPU individual core usage*/
				//read individual samples		
				all_newsample = cpuusage();
				//reading the timestamp
				time(&tack);
				//log the samples from all cpus
				all_cpus_log((int)(tack - tick), all_newsample);
/*CPU individual core peaks*/				
				//detect core peaks
				detect_core_peak((float *)all_newsample);
						
			}	
		}
      	break;
	case MODE_PROCCESS:
		for(;;){
      			for(UINT8 i=0; i<NUMBER_OF_SAMPLES_PER_MINUTE; i++){
				usleep(60000000/(NUMBER_OF_SAMPLES_PER_MINUTE));
				infinite_counter++;
				/*PID monitor stuff*/
				pid_usage = get_pid_usage(&cpuid);
				time(&teck);
				dump_pid_report((int)(teck - tick), pid_usage, cpuid);
				/*CPU overall average*/

				//CPU average proccess usage
				global_average(pid_usage, infinite_counter);
							
			}
		}	
	break;

	case MODE_MONITOR:
		for(;;){
      			for(UINT8 i=0; i<NUMBER_OF_SAMPLES_PER_MINUTE; i++){
					usleep(60000000/(NUMBER_OF_SAMPLES_PER_MINUTE));
					//read CPU sample
					newsample = get_global_cpu_usage();
					//CPU Sample operations - last minute average
					CPU_Measurements[i] = newsample;

					if(halfsample){
						average = calculate_average(CPU_Measurements, i);
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
								
					}
			
				}
				halfsample = false;
		}
	break;
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



bool read_parameters(UINT8 mode){
	
	volatile char tmpstr1[50];
	volatile char tmpstr2[50];
	
	FILE * fr = fopen("../parameters.txt", "rt");
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
			   if(NUMBER_OF_SAMPLES_PER_MINUTE > 120){
			   	printf("Your sample rate per minute is too high. This may have an impact in the performance. Please lower this parameter\n");
			   }
			}
			if(mode == MODE_MONITOR){
				if (strcmp((const char*)tmpstr1, "usagelimit") == 0){
				    CPU_LIMIT = atoi((const char*)tmpstr2);
				}

				else if (strcmp((const char*)tmpstr1, "comparison") == 0){
				    DECISION = atoi((const char*)tmpstr2);
				}
				
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




