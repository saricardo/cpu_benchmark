#!/bin/bash

mkdir -p reports

if [[ -f log* && -f fulldump.txt ]]; then
	mv log* reports
	mv fulldump.txt reports
fi

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
for f in log*; do
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


gnuplot -p << EOF
set datafile separator " "
set xlabel "time s"
set ylabel "CPU load %"
set yrange [0:100]
set title "CPU load analysis"
set terminal pngcairo size 1000,800 enhanced font 'Verdana,10'
set output "all.png"
plot for [i=0:6]  sprintf('logcpu%i', i) using 1:2 smooth bezier title 'cpu'.i.''
EOF
cd -

exit 0
