# Laboratoire 7 : Optimisations de compilation

### Pierrick Muller

### 1ere optimisation : Sibling calls

J'ai voulu pour cette optimisation me concentrer sur l'option `-foptimize-sibling-calls` qui permet d'optimiser les "sibling calls" et les "tail recursive calls". Voici le lien pour le code : 

https://godbolt.org/z/Fa8bVx

J'ai pris pour exemple une fonction qui prend un entier en entrée et qui retourne sa factorielle. Dans le cadre de la version non optimisée, un appel récursif est effectué afin de calculer le résultat. Le problème est que si le programme peut fonctionner aisément avec un petit entier, l'application de la fonction sur un gros entier risque fortement de créer un stack overflow au bout d'un moment. Si ce n'est pas vraiment une "optimisation" que d'arranger ce problème, réduire l'overhead dû aux nombreux appels de fonctions qui sont effectué représente bien une optimisation. 

Pour optimiser la fonction manuellement, le mieux est de la transformer en utilisant une boucle pour le calcul. Ainsi la partie "Récursion" est abandonnée. C'est donc ce que j'ai fait dans le deuxième code.

Enfin, pour l'optimisation avec le compilateur, j'ai utilisé l'option `-O2` qui permet d'activer l'option `-foptimize-sibling-calls`, option qui permet d'éviter le problème des appels de fonctions successifs. En effet, comme pour les autres fonctions de ce laboratoire, j'ai utilisé les options `-O*` car l'utilisation des optimisations seules ne marchait pas. Lorsque c'était possible , j'ai désactivé les options que je voulais mettre en avant comme options précises liées à mon amélioration.Par exemple j'ai dans le code en commentaire à coté du pragma GCC de la dernière fonction ajouté l'option `-fno-optimize-sibling-calls`. En l'ajoutant au pragma, on désactive l'option foptimize-sibling-calls présente par défaut dans l'option -O2, ce qui permet de voir l'optimisation effectuée.

Parlons maintenant un peu des résultats obtenus en observant le code assembleur. Pour la première fonction, la seule chose qu'il faut impérativement voir est la ligne 11 qui contient l'instruction ` call    factorial` . C'est cette instruction que l'on souhaite voir disparaître, car c'est là que la récursion a lieu et donc le risque de stack overflow ainsi que l'overhead généré dans la fonction.  

La deuxième fonction, de par sa modification en boucle, ne contient pas cette instruction.

La troisième fonction est intéressante. Je ne vais pas ici parler des autres optimisations apportées par l'option `-O2` , mais uniquement sur l'optimisation des "tail recursive calls". On voit que pour optimiser la fonction, l'appel récursif a été transformé en boucle. Cela tombe bien, c'est exactement l'optimisation qui était attendue. Ainsi, la nouvelle fonction suit le schéma suivant : 

```ASM
factorial_opt:
        mov     eax, 1	// Mise de la valeur 1 dans le registre qui contiendra la valeur de retour
        cmp     edi, 1  // Comparaison entre n et 1
        jle     .L11	// si n <= 1, on return directement la fonction, et donc 1
.L10:
        mov     edx, edi // On met N dans edx
        sub     edi, 1   // On soustrait 1 à N
        imul    eax, edx // On multiplie eax par edx et on stocke le resultat dans eax
        cmp     edi, 1	 // comparaison entre n et 1
        jne     .L10     // Si pas égal, on recomence la boucle
        ret				 // Si égal, on retourne la fonction
.L11:
        ret
```

On peut donc bien voir le schéma de boucle qui s'est mis en place. L'optimisation a bien eu lieu car comme dit plus haut, en ajoutant l'option désactivant l'optimisation, on retrouve une structure assembleur faisant appel à l'instruction call et donc un appel récursif normal.

### 2eme optimisation : if-conversion et hoist-coding

Le but de cette optimisation était à la base de présenter l'optimisation `-fcrossjumping`. Si l'objectif final est atteint, ce n'est pas au travers de l'optimisation souhaitée qu'elle a eu lieu. Nous allons voir ici les démarches effectuées pour réussir à obtenir ce résultat et quelles options d'optimisations ont permis de l'atteindre. Voici le lien pour le code : 

https://godbolt.org/z/hFVcJv

L'idée de base est la suivante. Le code fourni par défaut est assez simple. Il prend deux entiers en paramètre, puis en fonction de la valeur de l'entier a, il va additionner une valeur différente à cet entier. De plus, la fonction ajoute quoi qu'il arrive 5000 à l'entier b dans les deux cas, puis additionne l'entier a et b dans une variable c qu'elle retourne. Ici, l'exemple est un peu bête, mais l'idée est de démontrer par la suite le fonctionnement de l'optimisation `-fcrossjumping`. Le lien suivant présente le principe de crossjumping :

https://katecpp.wordpress.com/2015/09/19/cross-jumping/

Selon la doc des options d'optimisation de gcc, l'option `-fcrossjumping` permet d'unifier les parties de codes équivalentes et donc de diminuer la taille du code.  Ici aussi, l'optimisation peut sembler ne pas correspondre à une "réel" optimisation, mais lors de mes recherches je souhaitais tout de même tester cette option. J'ai obtenu une autre amélioration en découlant comme nous le verrons par la suite.

La version améliorée du code extrait simplement le traitement de l'entier b et l'addition des entiers a et b dans c hors des deux cas présentés par la condition if.

Pour la version optimisée du code par le compilateur, j'ai dû utiliser l'option `-O2` qui permet d'activer l'option `-fcrossjumping`.  On peut voir plusieurs choses dans ces fonctions. premièrement, sur la ligne du pragma GCC non commenté, j'ai laissé en commentaire à côté les deux options que j'ai découvert comme ayant un impact intéressant sur mon code de base. Nous en parlerons plus tard. La ligne en dessous, celle contenant une option permettant de désactiver l'option `-fmerge-constants`, est présente afin de démontrer un problème que j'ai rencontré. En effet, il semblerait qu'il ne soit pas possible de désactiver cette option via le pragma GCC, un warning apparaît au passage de la souris sur le mot GCC. J'ai testé cette option en local toute en la laissant dans le pragma et j'ai eu la même erreur, je n'ai pas réussi à déterminer la cause de cette erreur malgré mes recherches. Enfin la dernière ligne présente l'optimisation avec les options activées par `-O2` désactivée. Je la laisse car cela m'a aidé à mieux comprendre le fonctionnement des options, mais l'intérêt est moindre dans le cadre de notre analyse.

Par la suite, j'ai pu m'intéresser aux codes assembleur.

Dans le premier code, on peut voir la duplication des instructions suivantes : 

```ASM
        add     DWORD PTR [rbp-24], 5000 // b += 5000
        mov     edx, DWORD PTR [rbp-20]  // stockage a dans edx
        mov     eax, DWORD PTR [rbp-24]  // stockage b dans eax
        add     eax, edx				 // addition a et b dans eax
        mov     DWORD PTR [rbp-4], eax   // stockage resultat dans c
```

Dans le deuxième code, on retrouve ces instructions uniquement une fois, après l'addition de la valeur voulue dans a. On a donc un code plus petit.

C'est surtout la troisième fonction, la fonction optimisée, qui est intéressante. Comme dis à la base, je pensais démontrer l'optimisation apportée par `-fcrossjumping` en désactivant cette option à l'aide de l'option `-fno-crossjumping`. Malheureusement, cela ne fonctionnait pas. Voyons voir le code obtenu à l'aide de l'option `-O2`. Je n'ai pas précisement compris le fonctionnement au niveau des registres , mais voici la structure qui est appliquée selon moi : 

```ASM
func_codeHoisting_opt:
        add     esi, 5000				// Ajout 5000 à b
        cmp     edi, 1001				// comparaison entre a et 1001
        lea     edx, [rsi+100+rdi]		// Calcul cas 1 (c = b + 100 + a) dans edx
        lea     eax, [rsi+1000+rdi]		// Calcul cas 2 (c = b + 1000 + a) dans eax
        cmovge  eax, edx				// Si le résultat de la comparaison deux ligne 											// est plus grand ou = , alors on est dans le 											// premier cas et il faut stocker edx dans eax
        ret								// retour de la fonction
```

Plusieurs choses sont intéressante dans cette version optimisée. 

- Le branchement du if a disparu, au lieu de cela on effectue les deux calculs on choisit le bon résultat à la fin. 
- La partie où 5000 est ajouté à b n'est plus dupliqué, mais faite uniquement au début. 

Je me suis d'abord attelé à comprendre d'où provenait la disparition du branchement du if. J'ai vite trouvé l'option `-fif-conversion` qui permet de transformer les branchements conditionnels en version sans. cette option est activée par défaut dès le premier niveau de l'option `-O` et l'un des commentaires permet de la désactiver dans le code. On peut voir ainsi la différence avec la réapparition d'un branchement.

Par la suite je me suis demandé d'où provenait l'autre optimisation si ce n'était pas grace au crossjumping. J'ai trouvé l'option `-fcode-hoisting` qui effectue l'optimisation. Selon la documentation des options GCC, cette option recherche les instructions qui s'effectue dans tous les chemins du programme (comme notre b += 5000) et tente de les évaluer le plus tôt possible dans le programme. Cela explique pourquoi cet ajout de 5000 à b est effectué au début du code assembleur, car c'est une instruction qui se fait de toutes façons dans le code et qui peut être évalué dès le début du programme. La documentation précise aussi que cette option réduit la taille du code mais augmente aussi souvent la vitesse de notre code. Le deuxième commentaire de la ligne du pragma GCC permet de désactiver cette fonction. On voi que dans notre cas, l'optimisation n'est pas forcément flagrante, étant donnée que sans cette optimisation on supprime la ligne add du début et on ajoute juste 5000 au 100 et 1000 présents aux lignes comprenant les instruction lea. On peut en conclure que dans certains cas, les optimisations n'apportent pas forcément la réponse la plus optimale possible.

### 3eme optimisation : Loop Invariant

Mon but pour cette dernière partie était de m'intéresser à l'optimisation de code de type loop-invariant, soit de code présent dans une boucle et pouvant en être sortit. J'ai récupéré le code fournit sur le lien wikipedia présentant l'optimisation pour la première et la deuxième fonction : 

https://en.wikipedia.org/wiki/Loop-invariant_code_motion

Voici le lien pour le code : 

https://godbolt.org/z/2cNsPR

Ici, j'ai voulu regarder si l'option `-fmove-loop-invariants` avait un effet sur le code optimisé par le compilateur. Comme nous le verrons, la désactivation de cette option n'a pas marché mais l'optimisation a tout de même eu lieu avec l'option `-O1`. Par souci de lisibilité, j'ai utilisé deux options supplémentaires  afin de désactiver deux options de `-O1` qui optimisait plus en profondeur le code, mais qui ne le rendait pas plus clair, soit `-fno-tree-ch` et `-fno-reorder-blocks`. 

Le code de base prend 4 arguments , un tableau d'entier a qui contiendra les résultats, un entier n qui correspond au nombre d'éléments dans a, et deux entiers y et z qui sont utilisés pour les calculs. On remplit le tableau en suivant le schéma suivant : a[i] = 6 \* i + x \* x  où i correspond à l'itération dans laquelle on est en partant de 0 et x = y + z. Le problème est que la ligne x = y + z est recalculé à chaque itération de boucle. La version optimisée manuellement y remédie en sortant cette ligne de la boucle et en créant une variable supplémentaire pour stocker une seule fois le résultat de x \* x. Enfin, comme dit plus haut, la version optimisée par le compilateur de la fonction utilise comme option de compilation les options `-01`, `-fno-tree-ch` et `-fno-reorder-blocks` afin d'obtenir un résultat pouvant être expliqué et démontrant le résultat souhaité.

En regardant les résultats obtenus dans le langage assembleur, on peut voir qu'entre les fonctions 1 et 2 , soit la fonction de base et la fonction optimisée manuellement, ce que nous souhaitions voir arriver est arrivé. En effet, les lignes présentes dans la boucle concernant le calcul de x dans la fonction 1 se retrouvent en dehors de la boucle dans la fonction optimisée manuellement (ligne 12 à 15 et ligne 53 à 56). 

La partie la plus intéressante concerne la fonction optimisée par le compilateur. Jettons-y un coup d'oeil : 

```ASM
func_loopInvariant_opt:
        add     edx, ecx	// x = y + z  en dehors de la boucle
        imul    edx, edx	// x * x en dehors de la boucle , stockage dans edx
        mov     eax, 0		// i = 0, d'une certaine manière
        jmp     .L9			
.L10:
        mov     DWORD PTR [rdi+rax*4], edx  // On stocke edx dans a[i]
        add     rax, 1					// i++
        add     edx, 6					// Comme normalement i * 6, on ajoute 6 à edx à 										// chaque itération
.L9:
        cmp     esi, eax    // comparaison i et n
        jg      .L10		// si n > i, on saute à L10
        ret					// retour de la fonction
```



La partie que je voulais démontrer est ici bien claire. la partie du code concernant le calcul de x et de x \* x qui ne changeait pas d'itération en itération est sortie de la boucle.  Une chose que je ne m'explique toujours pas est l'impossibilité de désactiver réellement cette optimisation. Même en désactivant toutes les options de `-O1` comme j'ai pu le faire en commentaire dans le code (voir les différents pragma), impossible de supprimer cette optimisation. Heureusement, le but de base qui était de montrer comment l'optimisation est effectuée par le compilateur est atteind.

### Conclusion

J'ai trouvé ce laboratoire particulièrement frustrant parfois. En effet, le travail sur la désactivation des options qui au final ne se désactivaient pas prend beaucoup de temps. Cependant, quand après plusieurs recherches concernant ces options une désactivation d'option prouve enfin le bon fonctionnement de l'optimisation, le sentiment ressenti est très satisfaisant. D'un point de vue plus technique, le laboratoire permet de se rendre compte de la richesse que gcc fournit en terme d'option et des implications entre ces options, ce qui permet de mieux se rendre compte de la complexité de l'outil compilateur.