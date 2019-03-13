set terminal pngcairo size 1920,1080
set output "time_CNF_SMT.png"

set title "Temps d'exécution moyen et médian en fonction du nombre de mots dans (S_+, S_-) pour les méthodes CNF et SMT"

set auto x
set logscale y
set y2tics
unset logscale y2

set xlabel "Taille de S\n"
set ylabel "temps en s"

set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.9 absolute

set key vertical top left Left reverse

plot "full.time" using 2:xtic(1) title "Méthode de Biermann et Feldman - moyenne" linecolor rgb "#FF0000",\
    "" using 3:xtic(1) title "Méthode de Biermann et Feldman - médianne" linecolor rgb "#FF0000" fillstyle pattern 5,\
    "" using 5:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#0000FF",\
    "" using 6:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#0000FF" fillstyle pattern 5,\
    "" using 8:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#00FF00",\
    "" using 9:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#00FF00" fillstyle pattern 5,\
    "" using 11:xtic(1) title "Méthode de Heule et Verwer - moyenne" linecolor rgb "#FF00FF",\
    "" using 12:xtic(1) title "Méthode de Heule et Verwer - médianne" linecolor rgb "#FF00FF" fillstyle pattern 5,\
    "" using 14:xtic(1) title "Méthode SMT de Neider et Jansen - moyenne" linecolor rgb "#FFFF00",\
    "" using 15:xtic(1) title "Méthode SMT de Neider et Jansen - médianne" linecolor rgb "#FFFF00" fillstyle pattern 5
    # "" using 5:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman (non CNF) - moyenne" linecolor rgb "#00C8D7",\
    # "" using 6:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman (non CNF) - médianne" linecolor rgb "#00C8D7" fillstyle pattern 5,\

set output "time_CNF.png"
set title "Temps d'exécution moyen et médian en fonction du nombre de mots dans (S_+, S_-) pour les méthodes CNF"
plot "full.time" using 5:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#0000FF",\
    "" using 6:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#0000FF" fillstyle pattern 5,\
    "" using 7:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman - nombre de timeouts" axes x1y2 with points pointsize 2,\
    "" using 8:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - moyenne" linecolor rgb "#00FF00",\
    "" using 9:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - médianne" linecolor rgb "#00FF00" fillstyle pattern 5,\
    "" using 10:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman - nombre de timeouts" axes x1y2 with points pointsize 2,\
    "" using 11:xtic(1) title "Méthode de Heule et Verwer - moyenne" linecolor rgb "#FF00FF",\
    "" using 12:xtic(1) title "Méthode de Heule et Verwer - médianne" linecolor rgb "#FF00FF" fillstyle pattern 5,\
    "" using 13:xtic(1) title "Méthode de Heule et Verwer - nombre de timeouts" axes x1y2 with points pointsize 2

set output "time_SMT.png"
set title "Temps d'exécution moyen et médian en fonction du nombre de mots dans (S_+, S_-) pour les méthodes SMT"
plot "full.time" using 2:xtic(1) title "Méthode de Biermann et Feldman - moyenne" linecolor rgb "#FF0000",\
    "" using 3:xtic(1) title "Méthode de Biermann et Feldman - médianne" linecolor rgb "#FF0000" fillstyle pattern 5,\
    "" using 4:xtic(1) title "Méthode de Biermann et Feldamn - nombre de timeouts" axes x1y2 with points pointsize 2,\
    "" using 14:xtic(1) title "Méthode SMT de Neider et Jansen - moyenne" linecolor rgb "#FFFF00",\
    "" using 15:xtic(1) title "Méthode SMT de Neider et Jansen - médianne" linecolor rgb "#FFFF00" fillstyle pattern 5,\
    "" using 16:xtic(1) title "Méthode SMT de Neider et Jansen - nombre de timeouts" axes x1y2 with points pointsize 2

set output "time_non_CNF.png"
set title "Temps d'exécution moyen et médian en fonction du nombre de mots dans (S_+, S_-) pour les méthodes non-CNF"
plot "full.time" using 17:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman (non CNF) - moyenne" linecolor rgb "#5555FF",\
    "" using 18:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman (non CNF) - médianne" linecolor rgb "#5555FF" fillstyle pattern 5,\
    "" using 19:xtic(1) title "Méthode unaire de Grinchtein, Leucker et Piterman (non CNF) - nombre de timeouts" axes x1y2 with points pointsize 2,\
    "" using 20:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman (non CNF) - moyenne" linecolor rgb "#55FF55",\
    "" using 21:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman (non CNF) - médianne" linecolor rgb "#55FF55" fillstyle pattern 5,\
    "" using 22:xtic(1) title "Méthode binaire de Grinchtein, Leucker et Piterman (non CNF) - nombre de timeouts" axes x1y2 with points pointsize 2,\
    "" using 23:xtic(1) title "Méthode de Heule et Verwer (non CNF) - moyenne" linecolor rgb "#FF55FF",\
    "" using 24:xtic(1) title "Méthode de Heule et Verwer (non CNF) - médianne" linecolor rgb "#FF55FF" fillstyle pattern 5,\
    "" using 25:xtic(1) title "Méthode de Heule et Verwer (non CNF) - nombre de timeouts" axes x1y2 with points pointsize 2