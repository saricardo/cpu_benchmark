# cpu_benchmark

This app is intended to benchmark the cpu usage of any linux system.
The CPU values are printed in Percentage and refers to the relative CPU capacity disregarding the number of cpu cores. Ex: In a 8 cores system 100% refers to 8 cores fully used, whereas in a 4 core system 100% refers to 4 cores fully used. In a 2 cores system 50% referes to 1 core on average. So the 100% always refers to the totallity of CPU cores available.

Features:
	-The global average cpu usage in % is monitored during all the runtime of the app.
	-The average CPU during the last minute is also calculated.
	-The peak CPU usage is calculated during the whole execution.


The frequency of cpu usage samples can be adjusted with the parameter: 'nsamplesperminute'
More than one sample per second may be to heavy, depending on your system.

The 'comparison' parameter (1 or 0) is used to configure if you want to trigger an action in case the average CPU usage over the last minute goes above or below a given threashold. usagelimit is that threashold. If comparison is 0, then the action is triggered if the average drops below the threshold, if the threshold is 1 then the action is triggered above the threshold.

By default the only action possible to be triggered is system reboot.
More actions can be implemented later.
