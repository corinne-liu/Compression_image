README Projet compression/décompression
LIU Corinne TD3
BOUKADA Féryel TD1


Présentation du projet :
------------------------

	Il nous a été demandé dans ce projet de programmer une décompression sans perte et avec perte ainsi qu'une compression.


Options pour utiliser notre programme :
--------------------------------------

-c : active l'encodeur
-u : active le décodeur
-i : à mettre avant le nom du fichier
-o : permet de nommer le fichier de sortie
-g : active la grille de segmentation pour le décodeur et l'encodeur
-h : permet d'afficher l'aide
-v : permet de commenter les étapes suivies
Le nom de notre exécutable est codec. Il faut dans un premier temps écrire dans le terminal "make -f makelib" puis "make" afin de créer la librairie puis l'éxécutable et ensuite "./codec [OPTIONS] [NOM_FICHIER]", pour éxécuter. Il est également possible de faire un "make -f makelib clean" et "make clean" ce qui permettra de supprimer tous les fichiers objets étant dans le dossier "obj" ainsi que son dossier, le dossier "lib" pour la librairie sera aussi supprimé. Les fichiers d'en-tête se trouvent dans le dossier "include" et les fichiers sources dans le dossier "src", la documentations de doxygen se situe dans doc.

exemple :

        - ./codec -u -i QTC.lossless/boat.512.qtc -a 1.4 -g -v -o PGM/mon_boat.pgm
        Cette ligne de commande de permet de décoder le fichier boat.512.qtc se trouvant dans QTC.lossless et le résultat se trouvera dans PGM/mon_boat.pgm, de filtrer avec un alpha de 1.4 et créera sa grille de segmentation dans PGM/mon_boat_g.pgm.
        - ./codec -i PGM/toto.pgm -c
        Cette ligne de commande permet de d'encoder toto.pgm et comme il n'y a pas de -o alors il créera un nom de fichier par défaut qui est out.qtc qui se trouvera dans le dossier QTC


Contenu du dossier :
--------------------
        Fichiers :
        - README.txt
        - makefile
        - makelib
        Dossiers :
        - doc
        - PGM
        - QTC
        - include
        - src

Fonctions utilisées pour l'encodage :
------------------------------------

	Nous avons décidé de faire deux fonctions pour chaque passage. Nous avons ainsi une fonction pour passer de la pixmap au quadtree, à savoir "lire_fichier_PGM", ainsi que "remplir_tas". Nous en avons une pour le passage de quadtree à qtc, à savoir "parcours_tas" et "écrire_qtc".

- lire_fichier_PGM() : Cette fonction permet de lire la dimension du fichier ainsi que son niveau de gris tout en sautant les commentaires. Nous parcourons le fichier à l'aide de la fonction fgets. Ensuite, nous regardons si le premier élément de la ligne est un "#". Si non, nous continuons la fonction. Pour éviter que le fichier lise trop de lignes, nous avons décidé de retourner un zéro pour indiquer que la fonction est terminée.

- remplir_tas() : Cette fonction est une fonction récursive permettant de remplir le quadtree à partir d'une pixmap. Elle va diviser récursivement la pixmap en deux jusqu'à ce que la taille soit égale à 1. Lorsque celle-ci atteint cette taille, nous allons attribuer au nœud le premier pixel ainsi que son uniformité et son epsilon. Nous n'oublions pas de vérifier si le nœud est uniforme. Sinon, nous continuons à diviser la taille par deux.

- remplir_epsilon() : Cette fonction permet de remplir correctement les epsilon des nœuds du quadtree. Celle-ci est appelée après avoir fait le quadtree. Cette fonction parcourt ainsi le quadtree et fait la somme des moyennes de ces fils modulo 4 pour avoir la valeur epsilon du nœud.

- parcours_tas() : Cette fonction permet de parcourir le tas et de remplir le buffer de bitstream à l'aide de la fonction pushbits() utilisée lors d'un TP. Pour remplir correctement ce buffer, nous avons pris tous les cas possibles et les avons mis dans des if. De plus, cette fonction nous renvoie le nombre de caractères que nous devons écrire dans le fichier.

- ecrire_qtc() : Cette fonction permet d'écrire les valeurs des nœuds du quadtree dans un fichier .qtc donné en ligne de commande. Nous allons dans un premier temps écrire à l'aide de la fonction fprintf() "Q1". Ensuite, pour écrire la date, nous utilisons la structure temps_info. De plus, nous allouons un buffer pour pouvoir le remplir à l'aide de la fonction parcours_tas. Enfin, nous calculons le taux de compression à partir de la formule donnée dans le sujet et écrivons le buffer dans le fichier .qtc créé.


Fonctions utilisées pour le décodage :
-------------------------------------

	Le décodage repose sur le même principe que le codage mais dans le sens inverse. Pour le réaliser, nous avons fait les fonctions suivantes :

- lire_fichier_qtc() : Cette fonction permet de lire le fichier .qtc pour pouvoir récupérer le format ainsi que la profondeur.

- remplir_tas() : Cette fonction permet de lire un fichier et de remplir le buffer du bitstream en utilisant la fonction pullbits(). Toutes les conditions de cette fonction sont celles demandées dans le sujet.

- creation_pixmap() : Cette fonction permet de créer la pixmap à partir du quadtree. Pour réaliser cette fonction, nous avons dans un premier temps seulement inversé la fonction remplir_tas(). Cependant, cela n'a pas fonctionné. Nous avons ainsi repris la même récursion et logique en changeant l'implémentation.

- ecrire_pgm() : Cette fonction permet d'écrire dans un fichier .pgm donné dans la console. Nous allons dans un premier temps écrire "P1", la hauteur, la largeur et le niveau de gris à l'aide de la fonction fprintf(). Ensuite, nous devons écrire à l'aide de fwrite() la pixmap dans le fichier .pgm.


Fonctions utilisées pour la compression avec perte : 
---------------------------------------------------

	Pour effectuer la compression avec perte, nous avons simplement suivi les méthodes et pseudo-code donnés dans le sujet. Nous avons ainsi fait les fonctions suivantes :

- filtrage() : suivant pas à pas le pseudo-code

- remplir_variance() : Cette fonction parcourt le tas itérativement en calculant la variance de chacun des nœuds.

- calcule_medvar_maxvar() : Cette fonction permet de calculer la moyenne totale des variances. Nous divisons par le nombre de nœuds internes car les variances des fils sont toujours égales à 0.0.


Structures de données utilisées :
---------------------------------

typedef struct {
        unsigned char m; // Moyenne des quatre fils
        unsigned char e; // Vérifie la compression sans perte, gestion d'erreur
        unsigned char u; // Uniforme si 1, sinon 0
        double v;        // Variance vérifie homogénité
} Noeud;
---> pour un nœud du quadtree


typedef struct quadtree {
        Noeud *tas;	// Tas a quatres fils
        int taille;     // Taille actuelle du tas
        int profondeur; // Profondeur
} Quadtree;
--> structure de donnée de l'arbre quadtree


typedef struct {
        unsigned char* buffer;  // Pointeur vers l'octet courant
        unsigned long capacite; // Bits restants dans l'octet courant
} BitStream;
--> structure de donnée utilisé pour la fonction pushbit et pullbit


Programme principal :
---------------------

        Nous avons décidé de faire plusieurs fonctions dans le main étant :

- choix_argument : Cette fonction permet de vérifier les options passées en ligne de commande. Celle-ci stocke dans des variables les options utilisées en mettant les variables globales à 1.

- encodage() : Cette fonction permet de réaliser l'encodage d'un fichier en utilisant les fonctions présentes dans le module compression. Dans un premier temps, nous commençons par lire le fichier PGM. Ensuite, nous allons initialiser la pixmap pour pouvoir par la suite initialiser le quadtree. Enfin, nous allons écrire le fichier qtc à l'aide de la fonction "écrire_qtc()".

- decodage() : Cette fonction permet de réaliser le décodage d'un fichier en utilisant les fonctions présentes dans le module décompression. Dans un premier temps, nous allons lire le fichier qtc. Ensuite, nous allons initialiser le quadtree en le remplissant pour pouvoir enfin créer la pixmap. Enfin, nous allons écrire le fichier pgm.

- generer_nom_fichier_grille() : Cette fonction permet de créer un nom de fichier pour la grille à partir d'un nom de fichier. Tous nos fichiers contenant une grille de segmentation ont dans leur nom un "_g".

- main() : Cette fonction étant le programme principal, elle regarde si chacune des variables (encodage, décodage, etc.) est à 1. En fonction de ces variables, le programme effectuera l'encodage, le décodage, le filtrage et la grille de segmentation.

De plus, nous avons passé plusieurs variables en variables globales pour éviter de prendre trop de paramètres dans certaines de nos fonctions.


Explication de la modularisation :
----------------------------------

Nous avons décidé de faire une modularisation étant :

- filtrage
- décompression
- compression
- BitStream
- quadtree
pour plus de facilité de compréhension du projet. Nous aurions pu mettre les fonctions de BitStream dans décompression et compression, mais il nous semblait plus propre de faire ainsi. De plus, filtrage aurait également pu être dans compression, mais nous nous sommes dit qu'il serait mieux de faire un module car une compression peut se faire avec ou sans filtrage.


Difficultés rencontrées :
------------------------

	Nous avons rencontré des difficultés au niveau de la compréhension du sujet. Nous ne comprenions pas comment réaliser le sujet. Nous avons également rencontré des difficultés lors de la compression sans perte au niveau du passage de quadtree -> qtc.
        

Répartition du travail
----------------------

	Nous avons fait une grande partie du travail ensemble.
