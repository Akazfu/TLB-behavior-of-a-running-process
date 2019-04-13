set term push
set term png
set output "mergesort with 10000 instance sizes.png"
set title "mergesort with 10000 instance sizes"
set ylabel "misses rataes"
set auto x
set yrange [0:0.005]
set style data histogram
set key under nobox
set style histogram clustered gap 1 title offset 2,0.25
set style fill solid noborder
set boxwidth 0.95
unset xtics
set xtics nomirror font ",8"
set xlabel offset 0,-2
set ytics font ",8"
set grid y
set auto y
plot newhistogram "FIFO",'mergesort with 10000 instance sizes.dat' using "F512":xtic(1) t col, '' u "F4096" t col, '' u "F32768" t col,newhistogram "LRU",'mergesort with 10000 instance sizes.dat' using "L512":xtic(1) t col, '' u "L4096" t col, '' u "L32768" t col
set output
set term pop
