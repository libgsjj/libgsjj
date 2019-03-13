\mainpage
[TOC]
TODO: Rendre ça compréhensible
## Passif
Notre programme utilise une version légèrement modifiée de MapleCOMSPS_LRB. La modification apportée consiste à retirer les affichages dans la console afin de rendre notre programme plus agréable à utiliser.

### Installation de Maple
  * Télécharger MapleCOMSPS_LRB depuis le [site officiel](https://sites.google.com/a/gsd.uwaterloo.ca/maplesat/maplesat).
  * `unzip MapleCOMSPS_LRB.zip && cd MapleCOMSPS_LRB/simp && export MROOT=.. && make`

On peut installer dans les dossiers standards en faisant `make install` (ou en utilisant `checkinstall` qui permet de créer un .deb et donc d'avoir une installation plus propre) mais on en n'a pas besoin.

#### Pourquoi Maple ?
Parce qu'il a gagné la SAT Competition en 2016 et a été 2ème en 2017.

## Dépendances
Il faut installer :
  - CMake
  - Un compilateur de C++ qui supporte le standard C++14. Le compilateur de Visual Studio n'est pas supporté.
  - Boost (program_options et la bibliothèque String Algorithms)
  - cvc4 (présent dans les dépôts officiels Debian/Ubuntu/ArchLinux); il s'agit d'un SMT solveur
  - gmp (idem); nécessaire pour le SMT solveur
  - Doxygen; nécessaire pour générer la documentation
  - graphviz; idem

Un SAT solveur basé sur CNF (MapleCOMSPS_LRB) est fourni dans le code source.

Il y a également 2 autres SAT solveurs fournis dans le code source. L'un est limboole qui permet de trouver les variables satisfaisant une formule booléenne qui n'est pas en CNF et l'autre est picosat qui est une dépendance de limboole. Les deux sont distribués sous une license "as-is". La ligne 51 de `limboole.c` a été modifée afin de pouvoir plus aisément ajouter la bibliothèque à notre code. Un fichier `limboole.h` a également été ajouté pour la même raison.

bcsat est également fourni dans le code source. Il permet de rajouter une interface à Minisat (donc Maple) pour manipuler des formules booléennes quelconques. Plus précisément, cet outil traduit les formules booléennes en CNF. Pour télécharger cette dépendance, il faut exécuter `git submodule init && git submodule update`

## Compilation des benchmarks
Dans le dossier `benchmarks` :
~~~
  mkdir build && cd build && cmake .. && make
~~~

### Documentation
Il suffit d'éxécuter `make documentation`. La documentation est générée dans le sous-dossier `documentation` de `build`.

Pour la documentation html, le fichier `index.html` est l'endroit idéal pour commencer à lire la documentation.

### Ajouter une méthode
Il suffit de créer la classe et, dans le fichier `main.cpp`, de rajouter une valeur dans l'énumération `Choices` ainsi que l'appel au constructeur dans la fonction `createMethod`.

### Changement de SAT solver
Il est possible de changer le SAT solver utilisé en changeant le paramètre `SAT_SOLVER` dans CMake. Le programme `cmake-gui` peut être utile pour modifier cette valeur facilement.

### Changements effectués sur bcsat
https://github.com/scrippie/bcsat

  * Le 6 Mars 2019 :
    * Modification du fichier bcminisat220_solve.cc pour retirer les informations de Maple qui s'affichaient dans la console
    * Suppression des sous-dossiers `minisat-2.2.0` et `zchaff.2008.10.12`