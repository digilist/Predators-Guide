set encoding utf8
set terminal postscript color solid
set output '| ps2pdf - speedup_time_1_node.pdf'

# margins
set tmargin 0
set bmargin 0
set lmargin 2
set rmargin 0

set grid
set xlabel "Number of Processes"
set ylabel "Time (s)"

set style line 1 lt 1 lc rgb "orange"
set style fill solid 1.0
set boxwidth 0.5

plot "speedup_time_1_node.csv" using 2:xticlabels(1) with boxes ls 1 notitle
