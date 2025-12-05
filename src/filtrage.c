#include "filtrage.h"

void remplir_variance(Noeud *tas, int taille_tas, int indice) {
    for(int i = taille_tas; i >= 0; i--) {
        if ( 4 * i + 4 > taille_tas) {
            tas[i].v = 0.0;
        }
        else {
            double mu = 0.0;
            for (int j = 1; j <= 4; j++) {
                mu += pow(tas[ 4 * i + j].v, 2) + pow(tas[i].m - tas[4 * i + j].m, 2);
            }
            tas[i].v = sqrt(mu) / 4;
        }
    }
}

int filtrage(Noeud *tas, int taille_tas, double seuil, double alpha, int indice) {
    if (tas[indice].u == 1) {
        return 1;
    }
    if (4 * indice + 4 > taille_tas) {
        return 1;
    }
    int s = 0;
    s += filtrage(tas, taille_tas, seuil * alpha, alpha, 4 * indice + 1);
    s += filtrage(tas, taille_tas, seuil * alpha, alpha, 4 * indice + 2);
    s += filtrage(tas, taille_tas, seuil * alpha, alpha, 4 * indice + 3);
    s += filtrage(tas, taille_tas, seuil * alpha, alpha, 4 * indice + 4);
    if (s < 4 || tas[indice].v > seuil) {
        return 0;
    }
    tas[indice].e = 0;
    tas[indice].u = 1;
    return 1;
}

/**
 * @brief Calculer la variance moyenne et la variance maximale de tous les noeuds.
 * 
 * Cette fonction parcourt tous les noeuds du quadtree et calcule la variance moyenne 
 * et prend la variance maximal.
 * 
 * @param tas Le tableau des noeuds représentant l'arbre.
 * @param taille_tas Taille totale du tas.
 * @param medvar Un pointeur vers la variable où la variance moyenne sera stockée.
 * @param maxvar Un pointeur vers la variable où la variance maximale sera stockée.
 * @param indice L'indice du noeud à partir duquel commencer le calcul.
 */
static void stocke_medvar_maxvar(Noeud *tas, int taille_tas, double *medvar, double *maxvar, int indice) {
    for(int i = 0; i < taille_tas; i++) {
        *medvar += tas[i].v;
        if (tas[i].v > *maxvar) {
            *maxvar = tas[i].v;
        }
    }
}

void calculer_medvar_maxvar(Quadtree *node, int profondeur, double *medvar, double *maxvar) {
    stocke_medvar_maxvar(node->tas, node->taille, medvar, maxvar, 0);
    int noeurd_interne = nombre_noeud_interne(node->profondeur);
    *medvar = (noeurd_interne > 0) ? (*medvar / noeurd_interne) : 0.0;
}
