#!/usr/bin/env gnuplot

set terminal pngcairo size 1920,1080

set auto x
set logscale y
set ytics nomirror
# set yrange [0:400]
set ytics add (300 300) # We manually add a label on y = 300

# y2 is used for the number of timeouts
set y2tics
unset logscale y2
set y2range [0:100]

set xlabel "Taille de S\n"
set ylabel "Temps (en secondes)"
set y2label "Pourcentage d'échecs"

set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.9 absolute

set key vertical top left Left reverse

# First, SMT methods
set output "random_biermann.png"
set title "Temps d'exécution moyen et médian et pourcentage d'échecs en fonction du nombre de mots dans (S_+, S_-) pour la méthode de Biermann et Feldman"
plot "random.data" using 2:xtic(1) title "Méthode de Biermann et Feldman - moyenne" linecolor rgb "#FF0000",\
    "" using 3:xtic(1) title "Méthode de Biermann et Feldman - médianne" linecolor rgb "#FF0000" fillstyle pattern 5,\
    "" using ($1)-0.84:4 title "Méthode de Biermann et Feldamn - nombre de timeouts" axes x1y2 with points linecolor rgb "#FF0000" pointsize 2 pointtype 9

set output "random_neider.png"
set title "Temps d'exécution moyen et médian et pourcentage d'échecs en fonction du nombre de mots dans (S_+, S_-) pour la méthode de Neider et Jansen"
plot "random.data" using 5:xtic(1) title "Méthode SMT de Neider et Jansen - moyenne" linecolor rgb "#8D0000",\
    "" using 6:xtic(1) title "Méthode SMT de Neider et Jansen - médianne" linecolor rgb "#8D0000" fillstyle pattern 5,\
    "" using ($1)-0.84:7 title "Méthode SMT de Neider et Jansen - nombre de timeouts" axes x1y2 with points linecolor rgb "#8D0000" pointsize 2 pointtype 7

set output "random_SMT.png"
set title "Temps d'exécution moyen et médian et pourcentage d'échecs en fonction du nombre de mots dans (S_+, S_-) pour les méthodes SMT"
plot "random.data" using 2:xtic(1) title "Méthode de Biermann et Feldman - moyenne" linecolor rgb "#FF0000",\
    "" using 3:xtic(1) title "Méthode de Biermann et Feldman - médianne" linecolor rgb "#FF0000" fillstyle pattern 5,\
    "" using ($1)-1.1:4 title "Méthode de Biermann et Feldamn - nombre de timeouts" axes x1y2 with points linecolor rgb "#FF0000" pointsize 2 pointtype 9,\
    "" using 5:xtic(1) title "Méthode SMT de Neider et Jansen - moyenne" linecolor rgb "#8D0000",\
    "" using 6:xtic(1) title "Méthode SMT de Neider et Jansen - médianne" linecolor rgb "#8D0000" fillstyle pattern 5,\
    "" using ($1)-0.7:7 title "Méthode SMT de Neider et Jansen - nombre de timeouts" axes x1y2 with points linecolor rgb "#8D0000" pointsize 2 pointtype 7

# Now, CNF methods
set output "random_unary_cnf.png"
set title "Temps d'exécution moyen et médian et pourcentage d'échecs en fonction du nombre de mots dans (S_+, S_-) pour la méthode unaire de Grinchtein, Leucker et Piterman (en CNF)"
plot "random.data" using 8:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#0000FF",\
    "" using 9:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#0000FF" fillstyle pattern 5,\
    "" using ($1)-0.84:10 title "Méthode unaire de Grinchtein, Leucker et Piterman - nombre de timeouts" axes x1y2 with points pointsize 2 pointtype 9 linecolor rgb "#0000FF"

set output "random_binary_cnf.png"
set title "Temps d'exécution moyen et médian et pourcentage d'échecs en fonction du nombre de mots dans (S_+, S_-) pour la méthode binaire de Grinchtein, Leucker et Piterman (en CNF)"
plot "random.data" using 11:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#00FF00",\
    "" using 12:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#00FF00" fillstyle pattern 5,\
    "" using ($1)-0.84:13 title "Méthode unaire de Grinchtein, Leucker et Piterman - nombre de timeouts" axes x1y2 with points pointsize 2 pointtype 5 linecolor rgb "#00FF00"

set output "random_heule_cnf.png"
set title "Temps d'exécution moyen et médian et pourcentage d'échecs en fonction du nombre de mots dans (S_+, S_-) pour la méthode de Heule et Verwer"
plot "random.data" using 14:xtic(1) title "Méthode de Heule et Verwer - moyenne" linecolor rgb "#FF00FF",\
    "" using 15:xtic(1) title "Méthode de Heule et Verwer - médianne" linecolor rgb "#FF00FF" fillstyle pattern 5,\
    "" using ($1)-0.84:16 title "Méthode de Heule et Verwer - nombre de timeouts" axes x1y2 with points pointsize 2 pointtype 7 linecolor rgb "#FF00FF"

set output "random_cnf.png"
set title "Temps d'exécution moyen et médian et pourcentage d'échecs en fonction du nombre de mots dans (S_+, S_-) pour les méthodes CNF"
plot "random.data" using 8:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#0000FF",\
    "" using 9:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#0000FF" fillstyle pattern 5,\
    "" using ($1)-1.21:10 title "Méthode unaire de Grinchtein, Leucker et Piterman - nombre de timeouts" axes x1y2 with points pointsize 2 pointtype 9 linecolor rgb "#0000FF",\
    "" using 11:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#00FF00",\
    "" using 12:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#00FF00" fillstyle pattern 5,\
    "" using ($1)-0.92:13 title "Méthode binaire de Grinchtein, Leucker et Piterman - nombre de timeouts" axes x1y2 with points pointsize 2 pointtype 5 linecolor rgb "#00FF00",\
    "" using 14:xtic(1) title "Méthode de Heule et Verwer - moyenne" linecolor rgb "#FF00FF",\
    "" using 15:xtic(1) title "Méthode de Heule et Verwer - médianne" linecolor rgb "#FF00FF" fillstyle pattern 5,\
    "" using ($1)-0.637:16 title "Méthode de Heule et Verwer - nombre de timeouts" axes x1y2 with points pointsize 2 pointtype 7 linecolor rgb "#FF00FF"

# Now, non-CNF methods
set output "random_unary_non_cnf.png"
set title "Temps d'exécution moyen et médian et pourcentage d'échecs en fonction du nombre de mots dans (S_+, S_-) pour la méthode unaire de Grinchtein, Leucker et Piterman (en non-CNF)"
plot "random.data" using 17:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#5555FF",\
    "" using 18:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#5555FF" fillstyle pattern 5,\
    "" using ($1)-0.84:19 title "Méthode unaire de Grinchtein, Leucker et Piterman - nombre de timeouts" axes x1y2 with points pointsize 2 pointtype 5 linecolor rgb "#5555FF"

set output "random_binary_non_cnf.png"
set title "Temps d'exécution moyen et médian et pourcentage d'échecs en fonction du nombre de mots dans (S_+, S_-) pour la méthode binaire de Grinchtein, Leucker et Piterman (en non-CNF)"
plot "random.data" using 20:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#55FF55",\
    "" using 21:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#55FF55" fillstyle pattern 5,\
    "" using ($1)-0.84:22 title "Méthode binaire de Grinchtein, Leucker et Piterman - nombre de timeouts" axes x1y2 with points pointsize 2 pointtype 7 linecolor rgb "#55FF55"

set output "random_heule_non_cnf.png"
set title "Temps d'exécution moyen et médian et pourcentage d'échecs en fonction du nombre de mots dans (S_+, S_-) pour la méthode de Heule et Verwer (en non-CNF)"
plot "random.data" using 23:xtic(1) title "Méthode de Heule et Verwer - moyenne" linecolor rgb "#FF55FF",\
    "" using 24:xtic(1) title "Méthode de Heule et Verwer - médianne" linecolor rgb "#FF55FF" fillstyle pattern 5,\
    "" using ($1)-0.84:16 title "Méthode de Heule et Verwer - nombre de timeouts" axes x1y2 with points pointsize 2 pointtype 9 linecolor rgb "#FF55FF"

set output "random_non_cnf.png"
set title "Temps d'exécution moyen et médian et pourcentage d'échecs en fonction du nombre de mots dans (S_+, S_-) pour les méthodes non-CNF"
plot "random.data" using 17:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#5555FF",\
    "" using 18:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#5555FF" fillstyle pattern 5,\
    "" using ($1)-1.21:19 title "Méthode unaire de Grinchtein, Leucker et Piterman - nombre de timeouts" axes x1y2 with points pointsize 2 pointtype 5 linecolor rgb "#5555FF",\
    "" using 20:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#55FF55",\
    "" using 21:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#55FF55" fillstyle pattern 5,\
    "" using ($1)-0.92:22 title "Méthode binaire de Grinchtein, Leucker et Piterman - nombre de timeouts" axes x1y2 with points pointsize 2 pointtype 7 linecolor rgb "#55FF55",\
    "" using 23:xtic(1) title "Méthode de Heule et Verwer - moyenne" linecolor rgb "#FF55FF",\
    "" using 24:xtic(1) title "Méthode de Heule et Verwer - médianne" linecolor rgb "#FF55FF" fillstyle pattern 5,\
    "" using ($1)-0.637:16 title "Méthode de Heule et Verwer - nombre de timeouts" axes x1y2 with points pointsize 2 pointtype 9 linecolor rgb "#FF55FF"

# Finally, a plot with SMT and CNF methods
set output "random_cnf_smt.png"
set title "Temps d'exécution moyen et médian et pourcentage d'échecs en fonction du nombre de mots dans (S_+, S_-) pour les méthodes CNF et SMT"
plot "random.data" using 2:xtic(1) title "Méthode de Biermann et Feldman - moyenne" linecolor rgb "#FF0000",\
    "" using 3:xtic(1) title "Méthode de Biermann et Feldman - médianne" linecolor rgb "#FF0000" fillstyle pattern 5,\
    "" using ($1)-1.32:4 title "Méthode de Biermann et Feldamn - nombre de timeouts" axes x1y2 with points linecolor rgb "#FF0000" pointsize 2 pointtype 9,\
    "" using 8:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#0000FF",\
    "" using 9:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#0000FF" fillstyle pattern 5,\
    "" using ($1)-1.135:10 title "Méthode unaire de Grinchtein, Leucker et Piterman - nombre de timeouts" axes x1y2 with points pointsize 2 pt 9 linecolor rgb "#0000FF",\
    "" using 11:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#00FF00",\
    "" using 12:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#00FF00" fillstyle pattern 5,\
    "" using ($1)-0.95:13 title "Méthode binaire de Grinchtein, Leucker et Piterman - nombre de timeouts" axes x1y2 with points pointsize 2 pt 5 linecolor rgb "#00FF00",\
    "" using 14:xtic(1) title "Méthode de Heule et Verwer - moyenne" linecolor rgb "#FF00FF",\
    "" using 15:xtic(1) title "Méthode de Heule et Verwer - médianne" linecolor rgb "#FF00FF" fillstyle pattern 5,\
    "" using ($1)-0.775:16 title "Méthode de Heule et Verwer - nombre de timeouts" axes x1y2 with points pointsize 2 pt 7 linecolor rgb "#FF00FF",\
    "" using 5:xtic(1) title "Méthode SMT de Neider et Jansen - moyenne" linecolor rgb "#FFFF00",\
    "" using 6:xtic(1) title "Méthode SMT de Neider et Jansen - médianne" linecolor rgb "#FFFF00" fillstyle pattern 5,\
    "" using ($1)-0.59:7 title "Méthode SMT de Neider et Jansen - nombre de timeouts" axes x1y2 with points linecolor rgb "#8D0000" pointsize 2 pt 7