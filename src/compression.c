#include "compression.h"

int lire_fichier_PGM(FILE *fichier, int *largeur, int *longueur, int *niveau_couleur, char *ligne) {
    char magic_number[3];

    while (fgets(ligne, 256, fichier) != NULL) {
        if (ligne[0] != '#') {
            if (sscanf(ligne, "%2s ", magic_number) == 1 && strcmp(magic_number, "P5") == 0) {
            }
            else if (sscanf(ligne, "%d %d %d ", longueur, largeur, niveau_couleur) == 3) {
                return 0;
            }
            else if(sscanf(ligne, "%d %d ", longueur, largeur) == 2) {
                if(fgets(ligne, 256, fichier) != NULL && sscanf(ligne, "%d ", niveau_couleur) == 1) {
                    return 0;
                }
            } 
            else {
                fprintf(stderr, "Echec lecture dimensions ou niveau de gris ou format\n");
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @brief Remplir un tas (quadtree) à partir d'une pixmap.
 *
 * Cette fonction remplit récursivement un tas représentant un quadtree à partir des données de l'image. Chaque pixel est divisé en quatre sous-ensembles jusqu'à ce que la taille de la zone atteigne 1.
 *
 * @param tas Pointeur vers le tableau de nœuds du quadtree.
 * @param pixmap Le tableau des pixels de l'image.
 * @param largeur La largeur de l'image.
 * @param x La coordonnée x du coin supérieur gauche de la zone à traiter.
 * @param y La coordonnée y du coin supérieur gauche de la zone à traiter.
 * @param taille La taille de la zone à traiter.
 * @param indice L'indice du nœud dans le tableau du quadtree.
 */
static void remplir_tas(Noeud *tas, unsigned char *pixmap, int largeur, int x, int y, int taille, int indice) {
    if (x + taille > largeur || y + taille > largeur) {
        return;
    }

    int somme = 0;
    unsigned char premier_pixel = pixmap[y * largeur + x];


    if (taille == 1) {
        tas[indice].m = premier_pixel;
        tas[indice].u = 1;
        tas[indice].e = 0;
        return;
    }

    int demi_taille = taille / 2;

    remplir_tas(tas, pixmap, largeur, x, y, demi_taille, 4 * indice + 1);
    remplir_tas(tas, pixmap, largeur, x + demi_taille, y, demi_taille, 4 * indice + 2);
    remplir_tas(tas, pixmap, largeur, x + demi_taille, y + demi_taille, demi_taille, 4 * indice + 3);
    remplir_tas(tas, pixmap, largeur, x, y + demi_taille, demi_taille, 4 * indice + 4);

    somme = tas[4 * indice + 1].m + tas[4 * indice + 2].m + tas[4 * indice + 3].m + tas[4 * indice + 4].m;
    tas[indice].m = somme / 4;

    if (tas[4 * indice + 1].u == 1 && tas[4 * indice + 2].u == 1 && tas[4 * indice + 3].u == 1 && tas[4 * indice + 4].u == 1) {
        if (tas[4 * indice + 1].m == tas[4 * indice + 2].m && tas[4 * indice + 1].m == tas[4 * indice + 3].m && tas[4 * indice + 1].m == tas[4 * indice + 4].m) {
            tas[indice].u = 1;
        } else {
            tas[indice].u = 0;
        }
    } else {
        tas[indice].u = 0;
    }
}

/**
 * @brief Remplit les valeurs epsilon des nœuds du quadtree.
 *
 * Cette fonction parcourt récursivement le quadtree pour calculer et remplir la valeur epsilon de chaque nœud en fonction des valeurs de ses fils.
 *
 * @param tas Pointeur vers le tableau de nœuds du quadtree.
 * @param taille_tas La taille du tableau du quadtree.
 * @param i L'indice du nœud à traiter.
 */
static void remplir_epsilon(Noeud *tas, int taille_tas, int i) {
    if (tas == NULL || i >= taille_tas) {
        return;
    }

    if  (4*i+4 < taille_tas) {
        int somme = tas[4 * i + 1].m + tas[4 * i + 2].m + tas[4 * i + 3].m + tas[4 * i + 4].m;
        int epsilon = somme % 4;
        tas[i].e = epsilon;

        remplir_epsilon(tas, taille_tas, 4 * i + 1);
        remplir_epsilon(tas, taille_tas, 4 * i + 2);
        remplir_epsilon(tas, taille_tas, 4 * i + 3);
        remplir_epsilon(tas, taille_tas, 4 * i + 4);
    }
}

Quadtree* initialisation_quadtree(unsigned char* pixmap, int largeur, int longueur) {
    Quadtree *quadtree = malloc(sizeof(Quadtree));
    if (!quadtree) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le quadtree\n");
        return NULL;
    }

    quadtree->profondeur = calcule_profondeur(longueur);
    quadtree->taille = nombre_noeud(quadtree->profondeur);
    quadtree->tas = malloc(sizeof(Noeud) * quadtree->taille);

    if (!quadtree->tas) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le tas\n");
        free(quadtree);
        return NULL;
    }

    remplir_tas(quadtree->tas, pixmap, largeur, 0, 0, largeur, 0);
    remplir_epsilon(quadtree->tas, quadtree->taille, 0);
    return quadtree;
}

/**
 * @brief Parcourt le tas du quadtree et écrit les données dans un flux de bits.
 *
 * Cette fonction parcourt le tas du quadtree et écrit les informations sur chaque nœud dans un flux de bits (BitStream).
 *
 * @param tas Pointeur vers le tableau de nœuds du quadtree.
 * @param taille_tas La taille du tableau du quadtree.
 * @param stream Pointeur vers le flux de bits dans lequel les données seront écrites.
 * @return Le nombre de bits écrits.
 */
static int parcours_tas(Noeud *tas, int taille_tas, BitStream *stream) {
    int nombre_bit = 0;
    for (int i = 0; i < taille_tas; i++) {
        
        if (i > 0) {
            if (tas[(i - 1) / 4].u == 1) {
                continue;
            }
        }
        if (4 * i + 4 >= taille_tas) {
            if (i % 4 != 0 ) {
                pushbits(stream, tas[i].m, 8);
                nombre_bit += 8;
            }
        }
        else {
            if (i % 4 == 0 && i != 0) {
                pushbits(stream, tas[i].e, 2);
                nombre_bit += 2;
                if (tas[i].e == 0) {
                    pushbits(stream, tas[i].u, 1);
                    nombre_bit += 1;
                }
            }
            else {
                if (tas[i].e == 0) {
                    pushbits(stream, tas[i].m, 8);
                    pushbits(stream, tas[i].e, 2);
                    pushbits(stream, tas[i].u, 1);
                    nombre_bit += 8 + 2 + 1;
                }
                else {
                    pushbits(stream, tas[i].m, 8);
                    pushbits(stream, tas[i].e, 2);
                    nombre_bit += 8 + 2;
                }
            }
        }
    }
    return nombre_bit;
}


int ecrire_qtc(Quadtree *quadtree, char *nom_fichier, int longueur, int largeur) {
    FILE *fichier = fopen(nom_fichier, "wb");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur d'ouverture du fichier\n");
        return 1;
    }

    fprintf(fichier, "Q1\n");

    time_t now = time(NULL);
    char *date = ctime(&now);
    date[strlen(date) - 1] = '\0';
    fprintf(fichier, "# %s\n", date);

    unsigned char *buffer = (unsigned char*)calloc((quadtree->taille * 8), sizeof(unsigned char));
    if (buffer == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le buffer\n");
        return 1;
    }
    BitStream stream = {buffer, 8};
    pushbits(&stream, quadtree->profondeur, 8);
    int nombre_bit = parcours_tas(quadtree->tas, quadtree->taille, &stream);
    double nombre_bit_total = (largeur * longueur) * 8;
    nombre_bit = nombre_bit + nombre_bit % 8;
    double taux_compression = (nombre_bit / nombre_bit_total) * 100;
    fprintf(fichier, "# compression rate %.2f%%\n", taux_compression);
    fwrite(buffer, sizeof(unsigned char), (nombre_bit + CHAR_BIT) / CHAR_BIT, fichier);

    fclose(fichier);
    return 0;
}