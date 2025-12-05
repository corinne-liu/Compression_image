#include "decompression.h"

int lire_fichier_qtc(FILE *fichier, char *format, int *profondeur, char *ligne, unsigned char* liste_flux, char **date) {
    int nombre_ligne = 1;

    while (nombre_ligne < 4 && fgets(ligne, 256, fichier) != NULL) {
        if (ligne[0] != '#') {
            if (sscanf(ligne, "%2s ", format) == 1 && strcmp(format, "Q1") == 0) {
            }
        }
        else {
            char jour[4], mois[4];
            int numero, heure, minute, seconde, annee;
            if (sscanf(ligne, "# %3s %3s %d %d:%d:%d %d", jour, mois, &numero, &heure, &minute, &seconde, &annee) == 7) {
                *date = (char *)malloc((strlen(ligne) + 1) * sizeof(char));
                if (*date == NULL) {
                    fprintf(stderr, "Erreur d'allocation mémoire pour la date\n");
                    return 1;
                }
                strcpy(*date, ligne);
            }
        }
        nombre_ligne++;
    }

    char caractere = fgetc(fichier);
    if (caractere == EOF) {
        fprintf(stderr, "Erreur : fichier trop court.\n");
        return 1;
    }
    *profondeur = caractere;
    return 0;
}

/**
 * @brief Remplit le tas du quadtree à partir des données lues du fichier.
 * 
 * Cette fonction décode les données binaires dans le flux de bits et remplit le tableau des noeuds
 * du quadtree. Chaque noeud contient des informations sur la couleur moyenne (m), l'épsilon (e) et
 * la uniformité (u).
 * 
 * @param fichier Le fichier contenant les données de l'arbre.
 * @param stream Le flux de bits à partir duquel les données sont lues.
 * @param tas Le tableau des noeuds représentant l'arbre.
 * @param compte_noeud Compte les noeuds traités.
 * @param taille Taille totale de l'arbre.
 * @param nombre_feuille Nombre de feuilles du quadtree.
 * @param compte_fils Compte les fils traités pour chaque noeud.
 * @param indice2 Index pour les appels récursifs.
 * 
 * @return 0 en cas de succès.
 */
static int remplir_tas(FILE *fichier, BitStream *stream,  Noeud* tas, int taille, int nombre_feuille, int compte_fils) {
    for (int indice = 0; indice < taille; indice++) {
         if (tas[(indice - 1) / 4].u == 1) {
            tas[indice].m = tas[(indice - 1) / 4].m;
            tas[indice].e = tas[(indice - 1) / 4].e;
            tas[indice].u = tas[(indice - 1) / 4].u;
            continue;
        }

        if (compte_fils < 4) {
            unsigned char moyenne;
            pullbits(stream, &moyenne, 8);
            tas[indice].m = moyenne;
            compte_fils++;
        } 
        else {
            tas[indice].m = (4 * tas[(indice - 1) / 4].m + tas[(indice - 1) / 4].e) - (tas[indice - 1].m + tas[indice - 2].m + tas[indice - 3].m);
            compte_fils = 1;
        }

        if (taille - indice > nombre_feuille) {
            unsigned char epsilon;
            pullbits(stream, &epsilon, 2);
            tas[indice].e = epsilon;
            if (tas[indice].e == 0) {
                unsigned char uniforme;
                pullbits(stream, &uniforme, 1);
                tas[indice].u = uniforme;
                
            }
            else {
                tas[indice].u = 0;
            }
        }
        else {
            tas[indice].e = 0;
            tas[indice].u = 1;
        }
    }
    return 0;
}

void creation_pixmap(Noeud *tas, unsigned char *pixmap, int taille_tas, int largeur, int hauteur, int index, int x, int y, int taille_bloc) {
    if (index >= taille_tas) {
        return;
    }

    int m = tas[index].m;

    if (taille_bloc == 1) {
        for (int i = 0; i < taille_bloc; i++) {
            for (int j = 0; j < taille_bloc; j++) {
                pixmap[(y + i) * largeur + (x + j)] = m;
            }
        }
    }

    int demi_taille = taille_bloc / 2;

    creation_pixmap(tas, pixmap, taille_tas, largeur, hauteur, 4 * index + 1, x, y, demi_taille);
    creation_pixmap(tas, pixmap, taille_tas, largeur, hauteur, 4 * index + 2, x + demi_taille, y, demi_taille);
    creation_pixmap(tas, pixmap, taille_tas, largeur, hauteur, 4 * index + 3, x + demi_taille, y + demi_taille, demi_taille);
    creation_pixmap(tas, pixmap, taille_tas, largeur, hauteur, 4 * index + 4, x, y + demi_taille, demi_taille);
}

Quadtree* initialisation_quadtree2(FILE *fichier, int profondeur, int *nombre_feuille) {
    Quadtree *quadtree = malloc(sizeof(Quadtree));
    if (!quadtree) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le quadtree\n");
        return NULL;
    }

    quadtree->profondeur = profondeur;
    quadtree->taille = nombre_noeud(quadtree->profondeur);

    quadtree->tas = malloc(sizeof(Noeud) * quadtree->taille);
    if (!quadtree->tas) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le tas\n");
        free(quadtree);
        return NULL;
    }

    *nombre_feuille = quadtree->taille - nombre_noeud_interne(profondeur);

    unsigned char *buffer = (unsigned char*)calloc((quadtree->taille), sizeof(unsigned char));
    if (buffer == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le buffer\n");
        return NULL;
    }
    BitStream stream = {buffer, 8};
    fread(buffer, sizeof(unsigned char), quadtree->taille, fichier);
    remplir_tas(fichier, &stream, quadtree->tas, quadtree->taille, (*nombre_feuille), 0);
    return quadtree;
}

int ecrire_pgm(unsigned char *pixmap, char *nom_fichier, int taille_pixmap, int hauteur, int largeur, char *date) {
    FILE *fichier = fopen(nom_fichier, "wb");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur d'ouverture du fichier\n");
        return 1;
    }
    time_t now = time(NULL);
    char *date_maintenant = ctime(&now);
    date_maintenant[strlen(date_maintenant) - 1] = '\0';
    
    fprintf(fichier, "P5\n");
    fprintf(fichier, "%s", date);
    fprintf(fichier, "# %s\n", date_maintenant);
    fprintf(fichier, "%d %d\n", hauteur, largeur);
    fprintf(fichier, "255\n");

    unsigned char *buffer = (unsigned char*)calloc((taille_pixmap * 8), sizeof(unsigned char));
    if (buffer == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le buffer\n");
        return 1;
    }
    fwrite(pixmap, sizeof(unsigned char), taille_pixmap, fichier);
    fclose(fichier);
    return 0;
}
