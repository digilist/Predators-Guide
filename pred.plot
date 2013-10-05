set encoding utf8
set terminal postscript color solid
set output '| ps2pdf - plot.pdf'

# margins
set tmargin 0
set bmargin 0
set lmargin 0
set rmargin 0

set grid
set xlabel "Time"
set ylabel "Population"
# set title "Predators-Guide"

plot "/tmp/pred/steps.csv" using 1:2 with lines smooth csplines t "Predator" linecolor rgb "red", '' using 1:3 with lines smooth csplines t "Prey" linecolor rgb "blue", '' using 1:4 with lines smooth csplines t "Plants" linecolor rgb "green"