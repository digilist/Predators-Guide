set encoding utf8
set terminal postscript color solid
set output '| ps2pdf - plot.pdf'

set grid
set xlabel "Time"
set ylabel "Population"
set title "Predators-Guide"

plot "/tmp/pred/steps.csv" using 1:2 with lines smooth csplines t "Räuber" linecolor rgb "red", '' using 1:3 with lines smooth csplines t "Prey" linecolor rgb "blue", '' using 1:4 with lines smooth csplines t "Pflanzen" linecolor rgb "green"