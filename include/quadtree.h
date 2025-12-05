#ifndef QUADTREE_H
    #define QUADTREE_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>

    /**
     * @struct Noeud
     * @brief Représente un noeud du quadtree pour la compression d'image.
     *
     * Cette structure contient des informations sur un bloc d'image (moyenne, erreur, uniformité, variance).
     */
    typedef struct {
        unsigned char m; /**< Moyenne des quatre fils. */
        unsigned char e; /**< Erreur pour la vérification sans perte. */
        unsigned char u; /**< Indique si le bloc est uniforme 1 ou non 0. */
        double v;        /**< Variance du bloc pour vérifier l'homogénéité. */
    } Noeud;


    /**
     * @struct Quadtree
     * @brief Représente un arbre quaternaire pour la compression d'image.
     *
     * Cette structure gère les noeuds du quadtree, contenant des informations sur chaque bloc de l'image.
     */
    typedef struct quadtree {
        Noeud *tas;      /**< Tableau des noeuds du quadtree. */
        int taille;      /**< Taille actuelle du quadtree (nombre de noeuds). */
        int profondeur;  /**< Profondeur maximale du quadtree. */
    } Quadtree;

    /**
     * @brief Calcule le nombre de noeuds dans un arbre quadtree de profondeur n.
     * 
     * @param n Profondeur de l'arbre.
     * @return Le nombre total de noeuds.
     */
    double nombre_noeud(int n);

    /**
     * @brief Calcule de la profondeur.
     * 
     * @param longueur Taille de l'image.
     * @return La profondeur nécessaire pour représenter l'image.
     */
    int calcule_profondeur(int longueur);

    /**
     * @brief Calcule le nombre de noeuds internes à un quadtree de profondeur donnée.
     * 
     * @param profondeur Profondeur de l'arbre.
     * @return Le nombre de noeuds internes.
     */
    int nombre_noeud_interne(int profondeur);

    /**
     * @brief Initialise un tableau pixmap en lisant les données depuis un fichier.
     * 
     * @param fichier Fichier à lire.
     * @param ligne Ligne à traiter.
     * @param largeur Largeur de l'image.
     * @param longueur Longueur de l'image.
     * @param pixmap Tableau pixmap à remplir.
     * @return 0 si réussi, 1 si une erreur se produit.
     */
    int initialise_pixmap(FILE *fichier, char *ligne, int largeur, int longueur, unsigned char *pixmap);

    /**
     * @brief Crée une grille à partir d'un quadtree et la sauvegarde dans un fichier.
     * 
     * @param nom_fichier Nom du fichier de sortie.
     * @param quadtree Arbre quadtree à utiliser pour générer la grille.
     * @param largeur Largeur de l'image.
     * @param hauteur Hauteur de l'image.
     * @return 0 si réussi, 1 si une erreur se produit.
     */
    int creation_grille(char *nom_fichier, Quadtree *quadtree, int largeur, int hauteur);

#endif
