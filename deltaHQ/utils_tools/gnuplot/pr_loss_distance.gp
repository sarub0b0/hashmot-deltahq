set terminal postscript eps color 24
set output "pr_loss_distance.eps"
set ytics nomirror
set y2tics
set xlabel "distance[m]"
set ylabel "received power[dBm]"
set y2label "loss rate[\%]"
set xrange [1:21]
set key top left

plot 'loss_distance.log' using 1:2 with lines lw 3 title "received power", \
'loss_distance.log' using 1:($3*100) with lines lw 3 title "loss rate" axes x1y2

