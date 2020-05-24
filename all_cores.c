#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <stdbool.h>

#define PROCSTATFILE "/proc/stat"

static FILE * all_cores_fileptr[8];
static int _cpus;

int cpucount(void);

bool init_all_cores(){
	char name[20], filename[20];

	_cpus=cpucount();

	for(int n=0; n< _cpus; n++){
		sprintf(name, "logcpu%d", n);
		all_cores_fileptr[n] = fopen(name, "w");
	    	if (all_cores_fileptr[n] == NULL){
		        printf("Could not open file"); 
		        return false; 
		}
	}
	return true;

}

bool all_cpus_log(int timestamp, double * cpu_log){

	for (int n = 0; n < _cpus; n++){
	    	//printf("cpu%d:%.2f%%\n", n, percent[n]);
		fprintf(all_cores_fileptr[n], "%d ",timestamp);
		fprintf(all_cores_fileptr[n], "%lf\n", cpu_log[n]);
        }

}

void eprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

void *emalloc(size_t size) {
    void *p;

    p = malloc(size);
    if (!p)
        eprintf("out of memory\n");
    return p;
}

int cpucount(void) {
    int i;
    FILE *fp;
    char buf[BUFSIZ];

    fp = fopen(PROCSTATFILE, "r");
    if (!fp)
        eprintf("can't open %s\n", PROCSTATFILE);
    fgets(buf, BUFSIZ, fp);
    for (i = 0; fgets(buf, BUFSIZ, fp); i++)
        if (!!strncmp("cpu", buf, 3))
            break;

    fclose(fp);

    return i;
}

double *cpuusage(void) {
    int i;
    char buf[BUFSIZ];
    int cpuid;
    int r;
    static unsigned long long *lastuser, *lastnice, *lastsystem, *lastidle;
    unsigned long long *user, *nice, *system, *idle;
    unsigned long long total;
    static double *percent;
    FILE *fp;

    if (!lastuser)
        lastuser = emalloc(_cpus * sizeof(long long));
    if (!lastnice)
        lastnice = emalloc(_cpus * sizeof(long long));
    if (!lastsystem)
        lastsystem = emalloc(_cpus * sizeof(long long));
    if (!lastidle)
        lastidle = emalloc(_cpus * sizeof(long long));

    user = emalloc(_cpus * sizeof(long long));
    nice = emalloc(_cpus * sizeof(long long));
    system = emalloc(_cpus * sizeof(long long));
    idle = emalloc(_cpus * sizeof(long long));

    if (!percent)
        percent = calloc((_cpus + 1), sizeof(double));

    fp = fopen(PROCSTATFILE, "r");
    if (!fp)
        eprintf("can't open %s\n", PROCSTATFILE);
    fgets(buf, BUFSIZ, fp);
    for (i = 0; i < _cpus; i++) {
        //if (lastuser[i] && lastnice[i] && lastsystem[i] && lastidle[i]) {
            fgets(buf, BUFSIZ, fp);
            r = sscanf(buf, "cpu%d %llu %llu %llu %llu",
                    &cpuid, &user[i], &nice[i], &system[i], &idle[i]);
            //if (r < 5)
              //  break;
	    
	    if (user[i] < lastuser[i] || nice[i] < lastnice[i] ||
		system[i] < lastsystem[i] || idle[i] < lastidle[i]){
		//Overflow detection. Just skip this value.
		percent[i] = -1.0;
	    }
	    else{
		total = (user[i] - lastuser[i]) + (nice[i] - lastnice[i]) +
		    (system[i] - lastsystem[i]);
		percent[i] = total;
		total += (idle[i] - lastidle[i]);
		percent[i] /= total;
		percent[i] *= 100;
	    }
    }
    free(user);
    free(nice);
    free(system);
    free(idle);
    fclose(fp);

    fp = fopen(PROCSTATFILE, "r");
    if (!fp)
        eprintf("can't open %s\n", PROCSTATFILE);
    fgets(buf, BUFSIZ, fp);
    for (i = 0; i < _cpus; i++) {
        fgets(buf, BUFSIZ, fp);
        r = sscanf(buf, "cpu%d %llu %llu %llu %llu",
                &cpuid, &lastuser[i], &lastnice[i], &lastsystem[i], &lastidle[i]);
        if (r < 5)
            break;
    }
    fclose(fp);

    return percent;
}
