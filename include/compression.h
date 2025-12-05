#ifndef COMPRESSION_H
    #define COMPRESSION_H

    #include "bitstream.h"
    #include "quadtree.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>

    /**
     * @brief Lit un fichier PGM et extrait ses informations (dimensions et niveau de couleur).
     *
     * Cette fonction lit un fichier PGM et récupère les dimensions de l'image ainsi que le niveau de couleur. Elle ignore les lignes de commentaires.
     *
     * @param fichier Le fichier PGM à lire.
     * @param largeur Pointeur vers la largeur de l'image.
     * @param longueur Pointeur vers la longueur de l'image.
     * @param niveau_couleur Pointeur vers le niveau de couleur de l'image.
     * @param ligne Une chaîne de caractères utilisée pour stocker chaque ligne lue.
     * @return 0 si la lecture a réussi, sinon 1 en cas d'échec.
     */
    int lire_fichier_PGM(FILE *fichier, int *largeur, int *longueur, int *niveau_couleur, char *ligne);
    /**
     * @brief Initialise un quadtree à partir d'une pixmap.
     *
     * Cette fonction initialise un quadtree pour une image donnée. Elle alloue la mémoire nécessaire pour la structure et remplit le quadtree en divisant l'image en zones de plus en plus petites.
     *
     * @param pixmap Le tableau des pixels de l'image.
     * @param largeur La largeur de l'image.
     * @param longueur La longueur de l'image.
     * @return Un pointeur vers la structure Quadtree initialisée, ou NULL en cas d'échec.
     */
    Quadtree* initialisation_quadtree(unsigned char* pixmap, int largeur, int longueur);
    /**
     * @brief Écrit un fichier QTC à partir du quadtree.
     *
     * Cette fonction génère un fichier au format QTC contenant les informations du quadtree ainsi que le taux de compression.
     *
     * @param quadtree Pointeur vers la structure Quadtree à écrire.
     * @param nom_fichier Le nom du fichier de sortie.
     * @param longueur La longueur de l'image.
     * @param largeur La largeur de l'image.
     * @return 0 si l'écriture réussit, sinon 1 en cas d'échec.
     */
    int ecrire_qtc(Quadtree* quadtree, char* nom_fichier, int longueur, int largeur);

#endif