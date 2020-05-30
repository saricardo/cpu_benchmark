# cpu_benchmark

This app is intended to benchmark the cpu usage of any linux system (Desktop or embedded). Compilation for the corresponding platform must be performed accordingly.

100% non-obtrusive, no need to change your code or recompile. 

It supports 3 operating modes:
	- Mode 0: Standard general benchmarking -- Benchmarks the linux system as a whole and outputs reports for the usage of every single core temporally. Also outputs an overall cpu usage consisting on the cummulative capacity of all the system cores. Generic statistics such as cpu peaks and cpu load average also provided.
	- Mode 1: Proccess benchmarking by PID -- Benchmarks the cpu load of a single proccess. Multiple instances of this application can be run in parallel to benchmark more than one process. Outputs individual core load and load peaks. Generates a comparison chart between the load distribution on all cores. Also generates a report and chart on the cummulative usage regarding total cpu capacity.
	- Mode 2: Monitoring mode -- Benchmarks CPU usage during the past minute. If a given threshold is hit (from below or from above - configurable), the application can perform an action (currently system reboot).


Usage should be as below:
	./gen_amd64 <mode> <pid>[optional]

Or, called with the auxiliary script:

	./start_bench.sh <mode> <nameofproccess>[optional]

More parameters can be configured on file parameters.txt:

	- [all modes] The frequency of cpu usage samples can be adjusted with the parameter: 'nsamplesperminute'
More than two samples per second may be to heavy, depending on your system.
	
	- [mode 2 only] The 'comparison' parameter (1 or 0) is used to configure if you want to trigger an action in case the average CPU usage over the last minute goes above or below a given threashold. usagelimit is that threshold. If comparison is 0, then the action is triggered if the average drops below the threshold, if the threshold is 1 then the action is triggered above the threshold.

To stop data collection just stop the application with ctrl-c (SIGINT) at any time.

Chart Generation:
	To generate charts on the collected data, run:
	./generate_chart.bash
	Charts and reports will be placed on folder: reports/


Note: When the charts refer to general cummulative CPU usage, the CPU values are printed in percentage and refer to the relative CPU capacity disregarding the number of cpu cores. Ex: In a 8 cores system 100% refers to 8 cores fully used, whereas in a 4 core system 100% refers to 4 cores fully used. In a 2 cores system 50% referes to 1 core on average. So the 100% always refers to the totallity of CPU cores available.

