#!/usr/bin/env gnuplot

set terminal pngcairo size 1920,1080

set output "random_success.png"

set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.8

set ylabel 'Pourcentage de réussite'
set xlabel 'Méthode'

set palette gray

plot 'random_success.data' using 2:xticlabel(1) notitle linecolor rgb "gray30"