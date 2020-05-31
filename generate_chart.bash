#!/bin/bash

mkdir -p reports

if ls build/log* 1> /dev/null 2>&1; then
    mv build/log* reports
	mv build/fulldump.txt reports
	mv build/cpu_report_stats.txt reports
fi

cd reports


if ls fulldump.txt 1> /dev/null 2>&1; then
	#CPU generic load
	gnuplot -p << EOF
	set datafile separator " "
	set xlabel "time s"
	set ylabel "CPU load %"
	set yrange [0:100]
	set title "CPU load analysis"
	set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
	set output 'full_plot.png'
	plot 'fulldump.txt' using 1:2 w linespoints title 'cpu global usage', 'fulldump.txt' with points notitle
EOF
fi

if ls logcpu* 1> /dev/null 2>&1; then
	#CPU load by core
	i=0
	for f in logcpu*; do
		echo $f
		i=$(($i+1))
	    	gnuplot <<- EOF
		set datafile separator " "
		set xlabel "time s"
		set ylabel "CPU load %"
		set yrange [0:100]
		set title "CPU load analysis"
		set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
		set output "chart_cpu_$i.png"
		plot '${f}' using 1:2 w linespoints title 'cpu$i', '${f}' with points notitle
EOF
	done
fi
#Proccess load by CPU by core
if ls logpid* 1> /dev/null 2>&1; then
	i=0
	for f in logpid*; do
		echo $f
		i=$(($i+1))
	    	gnuplot <<- EOF
		set datafile separator " "
		set xlabel "time s"
		set ylabel "CPU load %"
		set yrange [0:100]
		set title "CPU load analysis"
		set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
		set output "chart_$f.png"
		plot '${f}' using 1:2 w linespoints title 'cpu$i', '${f}' with points notitle
EOF
	done
fi

if ls logpid_cpu* 1> /dev/null 2>&1; then
	#All proccess load by CPU by core - same graph
	gnuplot -p << EOF
	set datafile separator " "
	set xlabel "time s"
	set ylabel "CPU load %"
	set yrange [0:100]
	set title "CPU load analysis"
	set terminal pngcairo size 1000,800 enhanced font 'Verdana,10'
	set output "all_pid.png"
	plot for [i=0:6]  sprintf('logpid_cpu%i', i) using 1:2 w linespoints title 'cpu'.i.''
EOF
fi

if ls logcpu* 1> /dev/null 2>&1; then
	#All cpus load - same graph
	gnuplot -p << EOF
	set datafile separator " "
	set xlabel "time s"
	set ylabel "CPU load %"
	set yrange [0:100]
	set title "CPU load analysis"
	set terminal pngcairo size 1000,800 enhanced font 'Verdana,10'
	set output "all.png"
	plot for [i=0:6]  sprintf('logcpu%i', i) using 1:2 w linespoints title 'cpu'.i.''
EOF
fi

#soft alternative for comparison: using 1:2 w linespoints title 'cpu'.i.''
cd -
exit 0

