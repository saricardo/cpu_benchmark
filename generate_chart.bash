#!/bin/bash

mkdir -p reports

mv log* reports
mv fulldump.txt reports

cd reports
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

#soft alternative for comparison: using 1:2 w linespoints title 'cpu'.i.''
cd -

exit 0
