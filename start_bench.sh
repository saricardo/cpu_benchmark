#!/bin/bash

if [[ $# -lt 1 || $# -gt 2 ]]; then
	echo "Wrong Usage"
	exit 1

fi

#create folder with date and name

if [[ $# == 1 ]]; then
mode=$1
fi

if [[ $# == 2 ]]; then
mode=$1
proccess=$2
fi



pid=$(pidof -s $proccess)
##TODO wait for proccess start

cd build/

echo Tracking $pid with mode $mode
./gen_amd64 $mode $pid

cd -

exit 0
