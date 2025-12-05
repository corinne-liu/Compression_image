#ifndef DECOMPRESSION_H
    #define DECOMPRESSION_H

    #include "bitstream.h"
    #include "quadtree.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>

    /**
     * @brief Initialise un quadtree à partir du fichier et de la profondeur spécifiée.
     * 
     * Cette fonction alloue la mémoire pour un quadtree et remplit les données du quadtree
     * en lisant les informations à partir du fichier.
     * 
     * @param fichier Le fichier à lire.
     * @param profondeur La profondeur du quadtree.
     * @param nombre_feuille Un pointeur vers le nombre de feuilles du quadtree.
     * 
     * @return Le quadtree initialisé.
     */
    Quadtree* initialisation_quadtree2(FILE *fichier, int profondeur, int *nombre_feuille);
    /**
     * @brief Lit un fichier QTC et extrait les informations de format, profondeur et date.
     * 
     * Cette fonction vérifie que le fichier est bien au format QTC et récupère la profondeur de l'arbre
     * ainsi que la date de compression à partir des commentaires dans le fichier.
     * 
     * @param fichier Le fichier QTC à lire.
     * @param format Le format du fichier (doit être "Q1").
     * @param profondeur La profondeur du quadtree.
     * @param ligne Un tampon pour lire les lignes du fichier.
     * @param liste_flux Le flux de bits utilisé pour la lecture.
     * @param date La date de compression du fichier.
     * 
     * @return 0 en cas de succès, 1 en cas d'erreur.
     */
    int lire_fichier_qtc(FILE* fichier, char* format, int* profondeur, char* ligne, unsigned char* liste_flux, char **date);
    /**
     * @brief Crée une pixmap à partir des noeuds du quadtree.
     * 
     * Cette fonction génère une pixmap en parcourant le quadtree
     * et en attribuant des valeurs de couleur aux pixels dans chaque indice en fonction des 
     * informations stockées dans les noeuds.
     * 
     * @param tas Le tableau des noeuds représentant l'arbre.
     * @param pixmap La pixmap à remplir.
     * @param taille_tas Taille totale du tas.
     * @param largeur Largeur de la pixmap.
     * @param hauteur Hauteur de la pixmap.
     * @param index L'index actuel dans le tas.
     * @param x Coordonnée X du coin supérieur gauche du bloc.
     * @param y Coordonnée Y du coin supérieur gauche du bloc.
     * @param taille_bloc Taille du bloc à traiter.
     */
    void creation_pixmap(Noeud *tas, unsigned char *pixmap, int taille_tas, int largeur, int hauteur, int index, int x, int y, int taille_bloc);
    /**
     * @brief Écrit la pixmap dans un fichier PGM.
     * 
     * Cette fonction génère un fichier PGM à partir de la pixmap.
     * Le fichier PGM contient les données d'image en niveaux de gris.
     * 
     * @param pixmap La pixmap à écrire dans le fichier.
     * @param nom_fichier Le nom du fichier PGM.
     * @param taille_pixmap La taille de l'image en pixels.
     * @param hauteur Hauteur de l'image.
     * @param largeur Largeur de l'image.
     * @param date La date de compression à inclure dans les commentaires du fichier.
     * 
     * @return 0 en cas de succès, 1 en cas d'erreur.
     */
    int ecrire_pgm(unsigned char *pixmap, char *nom_fichier, int taille_pixmap, int hauteur, int largeur, char *date);

#endif