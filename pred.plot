set encoding utf8
set terminal postscript color solid
set output '| ps2pdf - plot.pdf'

set grid
set xlabel "Zeit"
set ylabel "Population"
set title "Populationsverlauf"

plot "/tmp/pred/steps.csv" using 1:2 with lines smooth csplines t "Räuber", '' using 1:3 with lines smooth csplines t "Beute", '' using 1:4 with lines smooth csplines t "Pflanzen"