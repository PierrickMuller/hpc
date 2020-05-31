# Laboratoire 3 HPC

#### Pierrick Muller
---
## Commandes
#### time:
- array :
  - Commande : `time ./sort array 100000`
  - Resultat : `./sort array 100000  6.49s user 0.00s system 99% cpu 6.491 total`
- list :
  - Commande : `time ./sort list 100000`
  - Resultat : `./sort list 100000  75.80s user 0.01s system 99% cpu 1:15.81 total`
---
#### strace -tt :
- array :
  - Commande : `strace -tt ./sort array 100000 `
  - Resultat (1ere ligne) : `11:19:42.067169 execve("./sort", ["./sort", "array", "100000"], 0x7ffeaa66f448 /* 79 vars */) = 0`
  - Resultat (dernière ligne) : `11:19:48.582257 +++ exited with 0 +++`
  - Calcul : ` 48.582257 − 42.067169 = 6.5151 secondes `
- list :
  - Commande : `strace -tt ./sort list 100000 `
  - Resultat (1ere ligne) : `11:20:46.703412 execve("./sort", ["./sort", "list", "100000"], 0x7ffc642dbb98 /* 79 vars */) = 0`
  - Resultat (dernière ligne) : `11:22:02.004428 +++ exited with 0 +++`
  - Calcul : `((22×60) +02.004428 ) − ((20×60) + 46.703412) = 75.301 secondes `
---
#### gprof :

- array :
  - Commandes :
  ```
  ./sort array 100000
  gprof sort gmon.out > analysis_array.txt
  ```
  - Contenu de analysis_array.txt (Sans explications sur les champs) :  
  ```
  Flat profile:
  Each sample counts as 0.01 seconds.
    %   cumulative   self              self     total      
    time   seconds   seconds    calls  Ts/call  Ts/call  name    
   100.32      6.47     6.47                             array_sort
  ```
---
- array :
  - Commandes :
  ```
  ./sort list 100000
  gprof sort gmon.out > analysis_list.txt
  ```
  - Contenu de analysis_list.txt (Sans explications sur les champs) :  
  ```
  Flat profile:
  Each sample counts as 0.01 seconds.
    %   cumulative   self              self     total      
    time   seconds   seconds    calls  Ts/call  Ts/call  name    
    51.95     37.16    37.16                             list_sort
    25.86     55.66    18.50                             list_init
    22.51     71.76    16.10                             list_clear
  ```
---

## Question:  Pourquoi l’une de ces deux fonctions est plus performante que l’autre ?
Car elle optimise la façon dont la cache est utilisée. En effet, dans la seconde version, on calcule toutes les multiplication nécessaires par rapport à une case de la matrice 1 avant de passer à une autre case de la matrice 1. Ainsi, lorsque la fonction doit trouver la valeur de la case matrice en question, elle se trouvera déjà en cache (Sauf lors du premier accès.). Cela permet de gagner en performances car le temps nécessaire pour accéder à la cache est plus court que le temps nécessaire à l'accès dans une mémoire plus éloigné.
