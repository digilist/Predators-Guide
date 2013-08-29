set encoding utf8
set terminal postscript color solid
set output '| ps2pdf - output.pdf'

set grid
set xlabel "Zeit"
set ylabel "Population"
set title "Populationsverlauf"

plot "/tmp/pred/100x100-10-10-90.csv" using 1:2 with lines smooth csplines t "Räuber", '' using 1:3 with lines smooth csplines t "Beute"
plot "/tmp/pred/100x100-10-20-80.csv" using 1:2 with lines smooth csplines t "Räuber", '' using 1:3 with lines smooth csplines t "Beute"
plot "/tmp/pred/100x100-10-30-70.csv" using 1:2 with lines smooth csplines t "Räuber", '' using 1:3 with lines smooth csplines t "Beute"
plot "/tmp/pred/100x100-10-40-60.csv" using 1:2 with lines smooth csplines t "Räuber", '' using 1:3 with lines smooth csplines t "Beute"
plot "/tmp/pred/100x100-10-50-50.csv" using 1:2 with lines smooth csplines t "Räuber", '' using 1:3 with lines smooth csplines t "Beute"
plot "/tmp/pred/100x100-10-60-40.csv" using 1:2 with lines smooth csplines t "Räuber", '' using 1:3 with lines smooth csplines t "Beute"
plot "/tmp/pred/100x100-10-70-30.csv" using 1:2 with lines smooth csplines t "Räuber", '' using 1:3 with lines smooth csplines t "Beute"
plot "/tmp/pred/100x100-10-80-20.csv" using 1:2 with lines smooth csplines t "Räuber", '' using 1:3 with lines smooth csplines t "Beute"
