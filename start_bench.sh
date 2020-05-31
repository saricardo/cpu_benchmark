#!/bin/bash

if [[ $# -lt 1 || $# -gt 2 ]]; then
	echo "Wrong Usage"
	echo "Usage ./start_bench.sh <MODE> <PROCESS>[OPTIONAL]"
	exit 1

fi

sudo apt-get install gnuplot > /dev/null 2>&1 || sudo apt-get install gnuplot

#create folder with date and name

if [[ $# == 1 ]]; then
mode=$1
fi

if [[ $# == 2 ]]; then
mode=$1
proccess=$2
fi

if [[ -z "$proccess"  ]]; then
    echo "Starting Monitoring"
else
	until pid=$(pidof -s $proccess)
	do  
    		echo "Waiting for $proccess to start"
    		echo ".." 
    		sleep 1
	done
	echo "Starting Monitoring"
fi



cd build/

echo Tracking $pid with mode $mode
./gen_amd64 $mode $pid

cd -

exit 0
