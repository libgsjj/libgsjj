#!/usr/bin/env gnuplot

set terminal pngcairo size 1920,1080

set yrange [0:100]

set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.8

set ylabel 'Pourcentage de réussite'
set xlabel 'Taille'

set palette gray

set output "loop-free_biermann.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode de Biermann et Feldman"
plot 'loop_free_success.data' using 2:xticlabel(1) notitle linecolor rgb "gray30"

set output "loop-free_neider.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode de Neider et Jansen"
plot 'loop_free_success.data' using 3:xticlabel(1) notitle linecolor rgb "gray30"

set output "loop-free_unaryCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode unaire de Grinchtein, Leucker et Piterman (CNF)"
plot 'loop_free_success.data' using 4:xticlabel(1) notitle linecolor rgb "gray30"

set output "loop-free_binaryCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode binaire de Grinchtein, Leucker et Piterman (CNF)"
plot 'loop_free_success.data' using 5:xticlabel(1) notitle linecolor rgb "gray30"

set output "loop-free_heuleCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode de Heule et Verwer (CNF)"
plot 'loop_free_success.data' using 6:xticlabel(1) notitle linecolor rgb "gray30"

set output "loop-free_unaryNonCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode unaire de Grinchtein, Leucker et Piterman (non-CNF)"
plot 'loop_free_success.data' using 7:xticlabel(1) notitle linecolor rgb "gray30"

set output "loop-free_binaryNonCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode binaire de Grinchtein, Leucker et Piterman (non-CNF)"
plot 'loop_free_success.data' using 8:xticlabel(1) notitle linecolor rgb "gray30"

set output "loop-free_heuleNonCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode de Heule et Verwer (non-CNF)"
plot 'loop_free_success.data' using 9:xticlabel(1) notitle linecolor rgb "gray30"

set output "loop-free_total.png"
set title "Pourcentage de réussite de chaque méthode"
plot 'loop_free_success_total.data' using 2:xticlabel(1) notitle linecolor rgb "gray30"