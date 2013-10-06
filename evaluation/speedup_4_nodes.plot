set encoding utf8
set terminal postscript color solid
set output '| ps2pdf - speedup_4_nodes.pdf'

set style data histogram
set style histogram cluster gap 1

set ylabel "Time (s)"

set y2tics nomirror
set y2label "Mop/s"

set style fill solid 1.0
set boxwidth 1 relative


set style line 1 lt 1 linecolor rgb "orange"
set style line 2 lt 1 linecolor rgb "greenyellow"

plot "speedup_4_nodes.csv" using 2:xtic(1) title "Time (s)" axes x1y2 ls 1, \
	'' using 3:xtic(1) title "Mops/s" axes x1y2 ls 2
