#!/usr/bin/env gnuplot

set terminal pngcairo size 1920,1080

set yrange [0:100]

set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.8

set ylabel 'Pourcentage de réussite'
set xlabel 'Taille'

# SMT methods
set output "loop-free_biermann.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode de Biermann et Feldman"
plot 'loop_free_success.data' using 2:xticlabel(1) notitle linecolor rgb "#FF0000"

set output "loop-free_neider.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode de Neider et Jansen"
plot 'loop_free_success.data' using 3:xticlabel(1) notitle linecolor rgb "#FFFF00"

set output "loop-free_SMT.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour les méthodes SMT"
plot 'loop_free_success.data' using 2:xticlabel(1) title "Méthode de Biermann et Feldman" linecolor rgb "#FF0000",\
    '' using 3:xticlabel(1) title "Méthode de Neider et Jansen" linecolor rgb "#FFFFF00"

# CNF methods
set output "loop-free_unaryCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode unaire de Grinchtein, Leucker et Piterman (CNF)"
plot 'loop_free_success.data' using 4:xticlabel(1) notitle linecolor rgb "#0000FF"

set output "loop-free_binaryCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode binaire de Grinchtein, Leucker et Piterman (CNF)"
plot 'loop_free_success.data' using 5:xticlabel(1) notitle linecolor rgb "#00FF00"

set output "loop-free_heuleCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode de Heule et Verwer (CNF)"
plot 'loop_free_success.data' using 6:xticlabel(1) notitle linecolor rgb "#FF00FF"

set output "loop-free_CNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour les méthodes CNF"
plot 'loop_free_success.data' using 4:xticlabel(1) title "Méthode unaire de Grinchtein, Leucker et Piterman" linecolor rgb "#0000FF",\
    'loop_free_success.data' using 5:xticlabel(1) title "Méthode binaire de Grinchtein, Leucker et Piterman" linecolor rgb "#00FF00",\
    'loop_free_success.data' using 6:xticlabel(1) title "Méthode de Heule et Verwer" linecolor rgb "#FF00FF"

# Non-CNF methods
set output "loop-free_unaryNonCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode unaire de Grinchtein, Leucker et Piterman (non-CNF)"
plot 'loop_free_success.data' using 7:xticlabel(1) notitle linecolor rgb "#5555FF"

set output "loop-free_binaryNonCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode binaire de Grinchtein, Leucker et Piterman (non-CNF)"
plot 'loop_free_success.data' using 8:xticlabel(1) notitle linecolor rgb "#AAFFAA"

set output "loop-free_heuleNonCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour la méthode de Heule et Verwer (non-CNF)"
plot 'loop_free_success.data' using 9:xticlabel(1) notitle linecolor rgb "#FF55FF"

set output "loop-free_nonCNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour les méthodes non CNF"
plot 'loop_free_success.data' using 7:xticlabel(1) title "Méthode unaire de Grinchtein, Leucker et Piterman" linecolor rgb "#5555FF",\
    'loop_free_success.data' using 8:xticlabel(1) title "Méthode binaire de Grinchtein, Leucker et Piterman" linecolor rgb "#AAFFAA",\
    'loop_free_success.data' using 9:xticlabel(1) title "Méthode de Heule et Verwer" linecolor rgb "#FF55FF"

# All SAT methods
set output "loop-free_SAT.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour les méthodes SAT"
plot 'loop_free_success.data' using 4:xticlabel(1) title "Méthode unaire de Grinchtein, Leucker et Piterman (CNF)" linecolor rgb "#0000FF",\
    'loop_free_success.data' using 7:xticlabel(1) title "Méthode unaire de Grinchtein, Leucker et Piterman (non CNF)" linecolor rgb "#5555FF",\
    'loop_free_success.data' using 5:xticlabel(1) title "Méthode binaire de Grinchtein, Leucker et Piterman (CNF)" linecolor rgb "#00FF00",\
    'loop_free_success.data' using 8:xticlabel(1) title "Méthode binaire de Grinchtein, Leucker et Piterman (non CNF)" linecolor rgb "#AAFFAA",\
    'loop_free_success.data' using 6:xticlabel(1) title "Méthode de Heule et Verwer (CNF)" linecolor rgb "#FF00FF",\
    'loop_free_success.data' using 9:xticlabel(1) title "Méthode de Heule et Verwer (non CNF)" linecolor rgb "#FFAAFF"

# SMT+CNF methods
set output "loop-free_SMT_CNF.png"
set title "Pourcentage de réussite en fonction de la taille du DFA original pour les méthodes SMT et CNF"
plot 'loop_free_success.data' using 2:xticlabel(1) notitle linecolor rgb "#FF0000",\
    'loop_free_success.data' using 4:xticlabel(1) title "Méthode unaire de Grinchtein, Leucker et Piterman" linecolor rgb "#0000FF",\
    'loop_free_success.data' using 5:xticlabel(1) title "Méthode binaire de Grinchtein, Leucker et Piterman" linecolor rgb "#00FF00",\
    'loop_free_success.data' using 6:xticlabel(1) title "Méthode de Heule et Verwer" linecolor rgb "#FF00FF",\
    '' using 3:xticlabel(1) title "Méthode de Neider et Jansen" linecolor rgb "#FFFFF00"

set palette gray
set output "loop-free_total.png"
set xlabel 'Méthode'
set title "Pourcentage de réussite de chaque méthode"
plot 'loop_free_success_total.data' using 2:xticlabel(1) notitle linecolor rgb "gray30"