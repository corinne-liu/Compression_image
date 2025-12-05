#include "quadtree.h"

double nombre_noeud(int n) {
    double res = 0.0;
    for (int k = 0; k <= n; k++) {
        res += pow(4, k);
    }
    return round(res);
}

int calcule_profondeur(int longueur) {
    int n = 0;
    while (pow(2, n) < longueur) {
        n++;
    }
    return n;
}

int nombre_noeud_interne(int profondeur) {
    int resultat = 0;
    for(int i = 0; i < profondeur; i++) {
        resultat += pow(4, i);
    }
    return resultat;
}

int initialise_pixmap(FILE *fichier, char *ligne, int largeur, int longueur, unsigned char *pixmap) {
    if (!pixmap) {
        fprintf(stderr, "Erreur allocation mémoire \n");
        return 1;
    }

    size_t pixel_lu = fread(pixmap, sizeof(unsigned char), largeur * longueur, fichier);
    if (pixel_lu != (size_t)(largeur * longueur)) {
        fprintf(stderr, "Dimensions attendues : %d x %d = %d pixmap\nDimension eu : %ld\n", largeur, longueur, largeur * longueur, pixel_lu);
        fprintf(stderr, "Données de la pixmap incomplètes");
        free(pixmap);
        return 1;
    }
   return 0;
}

void grille_segmentation(FILE *fichier, Noeud *tas, unsigned char *pixmap, int taille_tas, int largeur, int hauteur, int index, int x, int y, int taille_bloc) {
    if (index >= taille_tas) {
        return;
    }

    if (tas[index].u == 1) {
        for (int i = y; i < y + taille_bloc; i++) {
            for (int j = x; j < x + taille_bloc; j++) {
                if (i == y || i == y + taille_bloc - 1 || j == x || j == x + taille_bloc - 1) {
                    pixmap[i * largeur + j] = 0;
                } else {
                    if(tas[index].u == 1) {
                        pixmap[i * largeur + j] =  255;
                    }
                    else {
                        pixmap[i * largeur + j] = 0;
                    }
                }
            }
            
        }
    } else {
        int demi_taille = taille_bloc / 2;

        grille_segmentation(fichier, tas, pixmap, taille_tas, largeur, hauteur, 4 * index + 1, x, y, demi_taille);
        grille_segmentation(fichier, tas, pixmap, taille_tas, largeur, hauteur, 4 * index + 2, x + demi_taille, y, demi_taille);
        grille_segmentation(fichier, tas, pixmap, taille_tas, largeur, hauteur, 4 * index + 3, x + demi_taille, y + demi_taille, demi_taille);
        grille_segmentation(fichier, tas, pixmap, taille_tas, largeur, hauteur, 4 * index + 4, x, y + demi_taille, demi_taille);
    }
}

int creation_grille(char *nom_fichier, Quadtree *quadtree, int largeur, int hauteur) {
    FILE *fichier = fopen(nom_fichier, "wb");
    if (!fichier) {
        fprintf(stderr, "Erreur d'ouverture de fichier\n");
        return 1;
    }

    fprintf(fichier, "P5\n");
    fprintf(fichier, "%d %d\n", largeur, hauteur);
    fprintf(fichier, "255\n");

    unsigned char *grille = (unsigned char *)calloc(largeur * hauteur, sizeof(unsigned char));
    if(grille == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le grille\n");
        return 1;
    }
    grille_segmentation(fichier, quadtree->tas, grille, quadtree->taille, largeur, hauteur, 0, 0, 0, largeur);

    fwrite(grille, sizeof(unsigned char), largeur * hauteur, fichier);
    fclose(fichier);
    return 0;
}