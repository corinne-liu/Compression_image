#include <stdio.h>
#include <stdlib.h>
#include "quadtree.h"
#include "compression.h"
#include "decompression.h"
#include "filtrage.h"
#include "bitstream.h"

#define LIGNE 256 /**< Taille d'une ligne pour la lecture de fichiers */

/* Déclaration des variables globales*/
char *fichier_entrant = NULL;
char *fichier_sortant = NULL;
int a = 0, encode = 0, decode = 0, grille = 0, largeur = 0, longueur = 0, bavard = 0;
float alpha = 0.0;
Quadtree *quadtree = NULL;


/**
 * @brief Vérifie si le fichier a l'extension correcte .pgm ou .qtc.
 * 
 * @param nom_ficher Le nom du fichier à vérifier.
 * @return 1 si le fichier est valide, 0 sinon.
 */
int detecte_bon_format(char *nom_ficher) {
    if (!nom_ficher) return 0;
    char *extension = strrchr(nom_ficher, '.');
    if (!extension || (strcmp(extension, ".pgm") != 0 && strcmp(extension, ".qtc") != 0)) {
        return 0;
    }
    return 1;
}

/**
 * @brief Affiche les informations sur l'utilisation du programme.
 */
void affichage_help(void) {
    fprintf(stdout, "Usage : ./codec [OPTIONS]\n");
    fprintf(stdout, "Options disponibles :\n");
    fprintf(stdout, "  -c          : Active l'encodeur\n");
    fprintf(stdout, "  -u          : Active le décodeur\n");
    fprintf(stdout, "  -i <fichier>: Spécifie le fichier d'entrée (à mettre avant le nom du fichier)\n");
    fprintf(stdout, "  -o <fichier>: Spécifie le nom du fichier de sortie. (à mettre avant le nom du fichier)\n");
    fprintf(stdout, "  -g          : Active la grille de segmentation pour le décodeur et l'encodeur\n");
    fprintf(stdout, "  -h          : Affiche cette aide\n");
    fprintf(stdout, "  -v          : Mode bavard permet d'afficher les étapes suivies\n\n");
    fprintf(stdout, "Exemple : ./codec -c -i PGM/input.pgm -o QTC/out.qtc\n");
    fprintf(stdout, "Cette commande permet d'encoder le fichier PGM/input.pgm et sa sortie sera dans QTC/out.qtc\n");
}

/**
 * @brief Choix des arguments de la ligne de commande.
 * 
 * @param argc Le nombre d'arguments.
 * @param argv Les arguments en ligne de commande.
 * @param fichier_entrant Pointeur pour le nom du fichier d'entrée.
 * @param fichier_sortant Pointeur pour le nom du fichier de sortie.
 * @param encode Indicateur si l'encodage est activé.
 * @param decode Indicateur si le décodage est activé.
 * @param grille Indicateur si la grille de segmentation est activé.
 * @param bavard Indicateur si le mode bavard est activé.
 * @param a Indicateur pour activer le filtrage.
 * @return 0 si tout est valide, 1 sinon.
 */

int choix_arguments(int argc, char *argv[], char **fichier_entrant, char **fichier_sortant, int *encode, int *decode, int *grille, int *bavard, int *a) {
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] == '-') {
            if (strlen(arg) == 2) {
                switch (arg[1]) {
                    case 'c': 
                        *encode = 1;
                        break;
                    case 'u':
                        *decode = 1;
                        break;
                    case 'i':
                        if (i + 1 < argc) {
                            i++;
                            *fichier_entrant = malloc(strlen(argv[i]) + 1);
                            if (*fichier_entrant != NULL) {
                                strcpy(*fichier_entrant, argv[i]);
                            } else {
                                fprintf(stderr, "Erreur : allocation de mémoire pour le nom du fichier d'entrée\n");
                                return 1;
                            }
                            if (!detecte_bon_format(*fichier_entrant)) {
                                fprintf(stderr, "Erreur : <%s> n'est pas au bon format attendu soit .pgm soit .qtc\n", *fichier_entrant);
                                return 1;
                            }
                        } else {
                            fprintf(stderr, "Erreur : option -i nécessite un argument\n");
                            return 1;
                        }
                        break;
                    case 'a':
                        if (i + 1 < argc) {
                            *a = 1;
                            alpha = atof(argv[++i]);
                        } else {
                            fprintf(stderr, "Erreur : option -a nécessite un argument\n");
                            return 1;
                        }
                        break;
                    case 'o':
                        if (i + 1 < argc) {
                            i++;
                            *fichier_sortant = malloc(strlen(argv[i]) + 1);
                            if (*fichier_sortant != NULL) {
                                strcpy(*fichier_sortant, argv[i]);
                            } else {
                                fprintf(stderr, "Erreur : allocation de mémoire pour le nom du fichier de sortie\n");
                                return 1;
                            }
                            if (!detecte_bon_format(*fichier_sortant)) {
                                fprintf(stderr, "Erreur : <%s> n'est pas au bon format attendu soit .pgm soit .qtc\n", *fichier_sortant);
                                return 1;
                            }
                        } else {
                            fprintf(stderr, "Erreur : option -o nécessite un argument\n");
                            return 1;
                        }
                        break;
                    case 'g':
                        *grille = 1;
                        break;
                    case 'h':
                        affichage_help();
                        break;
                    case 'v':
                        *bavard = 1;
                        break;
                    default:
                        fprintf(stderr, "Erreur : <%s> n'est pas une option valide\n", arg);
                        return 1;
                }
            } else {
                fprintf(stderr, "Erreur : <%s> n'est pas une option valide\n", arg);
                return 1;
            }
        } else {
            fprintf(stderr, "Erreur : <%s> n'est pas une option valide\n", arg);
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Effectue un filtrage sur un quadtree en utilisant des seuils calculés à partir de la variance.
 * 
 * Cette fonction remplit la variance, calcule le médian et le maximum des variances (medvar et maxvar),
 * détermine un seuil, puis applique le filtrage en conséquence.
 * 
 * Ajout de cette fonction dans encodage et decodage
 */
void utiliser_filtrage() {
    if (a) {
        if (bavard) {
            fprintf(stdout, "Filtrage en cours de réalisation avec comme alpha : %0.2f\n", alpha);
        }
        remplir_variance(quadtree->tas, quadtree->taille, 0);
        double medvar = 0.0, maxvar = 0.0;
        calculer_medvar_maxvar(quadtree, quadtree->profondeur, &medvar, &maxvar);
        double seuil = (medvar > 0) ? medvar / maxvar : 0.0;
        if (bavard) {
            fprintf(stdout, "Obtention du medvar : %0.2f et du maxvar : %0.2f\n", medvar, maxvar);
        }
        filtrage(quadtree->tas, quadtree->taille, seuil, alpha, 0);
        if (bavard) {
            fprintf(stdout, "Filtrage réussi\n");
        }
    }
}

/**
 * @brief Effectue l'encodage du fichier d'entrée en format PGM au format QTC.
 * 
 * @return 0 si l'encodage a réussi, 1 sinon.
 */
int encodage() {
    if (bavard) {
        fprintf(stdout, "Encodage du fichier : %s\n", fichier_entrant);
    }
    FILE *fichier = fopen(fichier_entrant, "rb");
    if (!fichier) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier\n");
        return 1;
    }
    int niveau_couleur = 0;
    char ligne[LIGNE];

    if (bavard) {
        fprintf(stdout, "Lecture du fichier : %s\n", fichier_entrant);
    }
    int valeur_retour = lire_fichier_PGM(fichier, &largeur, &longueur, &niveau_couleur, ligne);
    if (valeur_retour) {
        fclose(fichier);
        fprintf(stderr, "Erreur dans le fichier entrant : %s\n", fichier_entrant);
        return 1; 
    }

    if (bavard) {
        fprintf(stdout, "Lecture réussi du fichier : %s\n", fichier_entrant);
        fprintf(stdout, "Création d'un pixmap\n");
    }
    unsigned char *pixmap = malloc(largeur * longueur);
    if (pixmap == NULL) {
        fclose(fichier);
        fprintf(stderr, "Erreur lors de l'allocation pour la pixmap\n");
        return 1;
    }

    if (bavard) {
        fprintf(stdout, "Initialisation du pixmap\n");
    }
    if (initialise_pixmap(fichier, ligne, largeur, longueur, pixmap)) {
        free(pixmap);
        fclose(fichier);
        fprintf(stderr, "Erreur dans l'initialisation de la pixmap\n");
        return 1;
    }

    if (bavard) {
        fprintf(stdout, "Initialisation réussi du pixmap\n");
        fprintf(stdout, "Initialisation du quadtree\n");
    }
    quadtree = initialisation_quadtree(pixmap, largeur, longueur);
    if (quadtree == NULL) {
        fprintf(stderr, "Erreur lors de l'initialisation du quadtree\n");
        free(pixmap);
        fclose(fichier);
        return 1;
    }
    utiliser_filtrage();
    if (fichier_sortant == NULL) {
        fichier_sortant = "QTC/out.qtc";
    }
    if (bavard) {
        fprintf(stdout, "Initialisation réussi du quadtree\n");
        fprintf(stdout, "Ecriture du fichier QTC : %s\n", fichier_sortant);
    }
    if (ecrire_qtc(quadtree, fichier_sortant, longueur, largeur)) {
        free(pixmap);
        fclose(fichier);
        return 1;
    }
    free(pixmap);
    fclose(fichier);
    if (bavard) {
        fprintf(stdout, "Fin de l'écriture du fichier QTC : %s\n", fichier_sortant);
        fprintf(stdout, "Encodage réussi du fichier : %s, dans le fichier : %s\n", fichier_entrant, fichier_sortant);
    }
    return 0;
}


/**
 * @brief Effectue le décodage du fichier QTC au fichier PGM.
 * 
 * @return 0 si le décodage a réussi, 1 sinon.
 */
int decodage() {
    if (bavard) {
        fprintf(stdout, "Décodage du fichier : %s\n", fichier_entrant);
    }
    FILE *fichier = fopen(fichier_entrant, "rb");
    if (!fichier) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier\n");
        return 1;
    }
    char *date = NULL;
    char format[2];
    int profondeur;
    unsigned char liste[LIGNE];
    char ligne[LIGNE];
    if (bavard) {
        fprintf(stdout, "Lecture du fichier : %s\n", fichier_entrant);
    }
    int valeur_retour = lire_fichier_qtc(fichier, format, &profondeur, ligne, liste, &date);
    if (valeur_retour) {
        fclose(fichier);
        fprintf(stderr, "Erreur dans le fichier %s\n", fichier_entrant);
        return 1; 
    }
    if (bavard) {
        fprintf(stdout, "Lecture réussi du fichier : %s\n", fichier_entrant);
        fprintf(stdout, "Initialisation du quadtree\n");
    }
    int nombre_feuille = 0;
    quadtree = initialisation_quadtree2(fichier, profondeur, &nombre_feuille);
    if (quadtree == NULL) {
        fprintf(stderr, "Erreur dans le fichier\n");
        return 1;
    }
    if (bavard) {
        fprintf(stdout, "Initialisation réussi du quadtree\n");
        fprintf(stdout, "Création du pixmap\n");
    }
    unsigned char * pixmap = (unsigned char *)malloc(sizeof(unsigned char) * nombre_feuille);
    if (pixmap == NULL) {
        return 1;
    }
    largeur = 1 << quadtree->profondeur;
    longueur = largeur;      
    int taille_bloc = largeur;
    utiliser_filtrage();
    if (bavard) {
        fprintf(stdout, "Initialisation du pixmap\n");
    }
    creation_pixmap(quadtree->tas, pixmap, quadtree->taille, largeur, longueur, 0, 0, 0, taille_bloc);
    if (fichier_sortant == NULL) {
        fichier_sortant = "PGM/out.pgm";
        if (bavard) {
            fprintf(stdout, "Renomage du fichier PGM : %s\n", fichier_sortant);
        }
    }
    if (bavard) {
        fprintf(stdout, "Initialisation réussi du pixmap\n");
        fprintf(stdout, "Ecriture du fichier PGM : %s\n", fichier_sortant);
    }
    ecrire_pgm(pixmap, fichier_sortant, nombre_feuille, longueur, largeur, date);
    free(pixmap);
    fclose(fichier);
    if (bavard) {
        fprintf(stdout, "Fin de l'écriture du fichier PGM : %s\n", fichier_sortant);
        fprintf(stdout, "Décodage réussi du fichier : %s, dans le fichier : %s\n", fichier_entrant, fichier_sortant);
    }
    return 0;
}


/**
 * @brief Génère un nom de fichier pour la grille de segmentation basée sur le fichier d'entrée ou de sortie.
 * 
 * @param fichier_entrant Le nom du fichier entrant.
 * @param fichier_sortant Le nom du fichier sortant.
 * @return Le nom généré du fichier pour la grille, NULL en cas d'erreur.
 */
char *generer_nom_fichier_grille(char *fichier_entrant, char *fichier_sortant) {
    char *source_fichier = NULL;
    if (strstr(fichier_entrant, ".pgm")) {
        source_fichier = fichier_entrant;
    } else if (strstr(fichier_sortant, ".pgm")) {
        source_fichier = fichier_sortant;
    }

    if (!source_fichier) {
        fprintf(stderr, "Erreur : Aucun fichier .pgm valide trouvé.\n");
        return NULL;
    }

    char *fichier_grille = malloc(strlen(source_fichier) + 3);
    if (!fichier_grille) {
        fprintf(stderr, "Erreur : Impossible d'allouer de la mémoire pour le nom du fichier grille.\n");
        return NULL;
    }

    char *extension = strrchr(source_fichier, '.');
    size_t position = extension ? (extension - source_fichier) : strlen(source_fichier);
    strncpy(fichier_grille, source_fichier, position);
    fichier_grille[position] = '\0';
    strcat(fichier_grille, "_g");
    strcat(fichier_grille, extension);

    return fichier_grille;
}

/**
 * @brief Le programme principale.
 * 
 * @param argc Le nombre d'arguments.
 * @param argv Les arguments en ligne de commande.
 * @return 0 en cas de succès, 1 en cas d'erreur.
 */
int main(int argc, char *argv[]) {
    if (bavard) {
        fprintf(stdout, "Mode bavard activé\n");
    }

    if (choix_arguments(argc, argv, &fichier_entrant, &fichier_sortant, &encode, &decode, &grille, &bavard, &a)) {
        return 1;
    }

    if (encode) {
        if (encodage()) {
            return 1;
        }
    }

    if (decode) {
        if (decodage()) {
            return 1;
        }
    }

    char *fichier_grille = generer_nom_fichier_grille(fichier_entrant, fichier_sortant);
    if (grille && (fichier_grille != NULL)) {
        
        if (bavard) {
            fprintf(stdout, "Création du nom du fichier de la grille de segmentation : %s\n", fichier_grille);
            fprintf(stdout, "Création du fichier de la grille de segmentation : %s\n", fichier_grille);
        }
        if (creation_grille(fichier_grille, quadtree, largeur, longueur)) {
            return 1;
        }
        if (bavard) {
            fprintf(stdout, "Création réussi de la grille de segmentation dans %s\n", fichier_grille);
        }
    }

    if (bavard) {
        fprintf(stdout, "Réalisation complète de toutes les options demandées\n");
    }
    return 0;
}
