#ifndef FILTRAGE_H
    #define FILTRAGE_H

    #include "quadtree.h"
    
    /**
     * @brief Remplir la variance des noeuds d'un quadtree.
     * 
     * Cette fonction parcourt les noeuds du quadtree en commençant par les feuilles et
     * calcule la variance pour chaque noeud en fonction de ses enfants. La variance est
     * calculée en utilisant la moyenne des différences entre les valeurs des noeuds fils.
     * 
     * @param tas Le tableau des noeuds représentant l'arbre.
     * @param taille_tas Taille totale du tas.
     * @param indice L'indice du noeud courant pour lequel la variance est calculée.
     */
    void remplir_variance(Noeud *tas, int taille_tas, int indice);
    
    /**
     * @brief Appliquer un filtrage aux noeuds d'un quadtree.
     * 
     * Cette fonction applique un filtrage récursif en fonction d'un seuil et d'un paramètre alpha.
     * Si un noeud a une variance inférieure à un seuil, il est fusionné avec ses enfants.
     * Le filtrage est effectué sur le quadtree à partir du noeud spécifié.
     * 
     * @param tas Le tableau des noeuds représentant l'arbre.
     * @param taille_tas Taille totale du tas.
     * @param seuil Le seuil en dessous duquel un noeud est fusionné avec ses enfants.
     * @param alpha Le facteur de réduction appliqué au seuil à chaque niveau de profondeur.
     * @param indice L'indice du noeud courant.
     * @return Retourne 1 si le filtrage a réussi pour ce noeud, 0 sinon.
     */
    int filtrage(Noeud *tas, int taille_tas, double seuil, double alpha, int indice);
    
    /**
     * @brief Calculer la variance moyenne et maximale à partir du quadtree.
     * 
     * Cette fonction calcule la variance moyenne et maximale en appelant la fonction 
     * "stocke_medvar_maxvar" pour stocker ces valeurs.
     * Elle prend en compte le nombre de noeuds internes du quadtree pour calculer la moyenne.
     * 
     * @param node Le quadtree contenant les noeuds à analyser.
     * @param profondeur La profondeur du quadtree.
     * @param medvar Pointeur où la variance moyenne sera stockée.
     * @param maxvar Pointeur où la variance maximale sera stockée.
     */
    void calculer_medvar_maxvar(Quadtree *node, int profondeur, double *medvar, double *maxvar);

#endif
