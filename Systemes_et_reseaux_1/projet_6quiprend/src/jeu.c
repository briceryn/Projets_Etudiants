/*
 * fichier : jeu.c
 * but : Contient toute la logique du jeu 6 qui prend
 *       - Gestion des cartes
 *       - Gestion des rangées
 *       - Logique des parties
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "jeu.h"

/* ========== CONSTANTES INTERNES ========== */
#define MAX_CARTES_PAR_RANGEE 5  /* avant qu'un joueur ne doive la ramasser */
#define CARTES_PAR_JOUEUR 10
#define SEUIL_DEFAITE 66         /* 66 têtes de bœuf pour perdre */

/* ========== FONCTIONS DES CARTES ========== */

/**
 * Calcule le nombre de têtes de bœuf pour une carte
 * Règles :
 * - multiple de 55 -> 7 têtes
 * - multiple de 11 -> 5 têtes
 * - multiple de 10 -> 3 têtes
 * - multiple de 5  -> 2 têtes
 * - sinon         -> 1 tête
 */
int calculer_tetes_carte(int valeur) {
    if (valeur <= 0 || valeur > NB_CARTES_TOTAL) return 0;

    if (valeur % 55 == 0) return 7;
    if (valeur % 11 == 0) return 5;
    if (valeur % 10 == 0) return 3;
    if (valeur % 5 == 0) return 2;
    return 1;
}

/**
 * Initialise toutes les cartes du jeu (1 à 104)
 */
void initialiser_pioche(Carte pioche[]) {
    for (int i = 0; i < NB_CARTES_TOTAL; i++) {
        pioche[i].valeur = i + 1;
        pioche[i].tetes_de_boeuf = calculer_tetes_carte(i + 1);
    }
}

/**
 * Mélange la pioche aléatoirement
 */
void melanger_pioche(Carte pioche[], int taille) {
    srand(time(NULL) ^ getpid());  /* graine aléatoire */

    for (int i = 0; i < taille; i++) {
        int j = rand() % taille;
        Carte temp = pioche[i];
        pioche[i] = pioche[j];
        pioche[j] = temp;
    }
}

/**
 * Affiche une carte à l'écran
 */
void afficher_carte(Carte c) {
    printf("[%3d (%d tête%s)]", c.valeur, c.tetes_de_boeuf,
           c.tetes_de_boeuf > 1 ? "s" : "");
}

/**
 * Affiche plusieurs cartes (pour la main d'un joueur)
 */
void afficher_cartes(const Carte cartes[], int nb_cartes) {
    for (int i = 0; i < nb_cartes; i++) {
        printf("%d: ", i + 1);
        afficher_carte(cartes[i]);
        if (i < nb_cartes - 1) printf(", ");
    }
    printf("\n");
}

/* ========== FONCTIONS DES RANGEES ========== */

/**
 * Initialise une rangée (vide)
 */
void initialiser_rangee(Rangee* r) {
    r->nb_cartes = 0;
    r->total_tetes = 0;
}

/**
 * Trouve la dernière carte d'une rangée (celle avec la plus grande valeur)
 */
Carte derniere_carte_rangee(const Rangee* r) {
    if (r->nb_cartes == 0) {
        Carte vide = {0, 0};
        return vide;
    }
    return r->cartes[r->nb_cartes - 1];
}

/**
 * Vérifie si une carte peut être ajoutée à une rangée
 * (doit être plus grande que la dernière carte de la rangée)
 */
int peut_ajouter_a_rangee(const Rangee* r, Carte c) {
    if (r->nb_cartes == 0) return 1;  /* rangée vide */

    Carte derniere = derniere_carte_rangee(r);
    return c.valeur > derniere.valeur;
}

/**
 * Ajoute une carte à une rangée
 * Retourne 1 si la rangée était pleine (5ème carte), 0 sinon
 */
int ajouter_carte_a_rangee(Rangee* r, Carte c) {
    if (r->nb_cartes >= NB_MAX_CARTES_RANGEE) {
        printf("ERREUR: tentative d'ajouter à une rangée pleine!\n");
        return 0;
    }

    r->cartes[r->nb_cartes] = c;
    r->nb_cartes++;
    r->total_tetes += c.tetes_de_boeuf;

    /* Si c'est la 5ème carte, la rangée est "pleine" */
    return (r->nb_cartes == MAX_CARTES_PAR_RANGEE);
}

/**
 * Vide une rangée et retourne le total des têtes
 */
int vider_rangee(Rangee* r) {
    int tetes = r->total_tetes;
    initialiser_rangee(r);
    return tetes;
}

/**
 * Calcule le total des têtes dans une rangée
 */
int calculer_tetes_rangee(const Rangee* rangee) {
    int total = 0;
    if (rangee) {
        for (int i = 0; i < rangee->nb_cartes; i++) {
            total += rangee->cartes[i].tetes_de_boeuf;
        }
    }
    return total;
}

/**
 * Affiche une rangée
 */
void afficher_rangee(const Rangee* r) {
    printf("Rangée (%d carte%s, total: %d tête%s): ",
           r->nb_cartes, r->nb_cartes > 1 ? "s" : "",
           r->total_tetes, r->total_tetes > 1 ? "s" : "");

    if (r->nb_cartes == 0) {
        printf("vide");
    } else {
        for (int i = 0; i < r->nb_cartes; i++) {
            afficher_carte(r->cartes[i]);
            if (i < r->nb_cartes - 1) printf(" < ");
        }
    }
    printf("\n");
}

/* ========== FONCTIONS DES JOUEURS ========== */

/**
 * Crée un nouveau joueur
 */
Joueur* creer_joueur(int id, const char* nom, int est_robot) {
    Joueur* j = (Joueur*)malloc(sizeof(Joueur));
    if (!j) {
        printf("ERREUR: impossible d'allouer mémoire pour joueur\n");
        return NULL;
    }

    j->id = id;
    strncpy(j->nom, nom, TAILLE_NOM - 1);
    j->nom[TAILLE_NOM - 1] = '\0';
    j->score = 0;
    j->nb_cartes_main = 0;
    j->est_robot = est_robot;
    j->est_actif = 1;

    return j;
}

/**
 * Libère la mémoire d'un joueur
 */
void detruire_joueur(Joueur* j) {
    if (j) {
        free(j);
    }
}

/**
 * Ajoute une carte à la main d'un joueur
 */
void ajouter_carte_main(Joueur* j, Carte c) {
    if (j->nb_cartes_main >= NB_CARTES_MAIN) {
        printf("ERREUR: main pleine pour joueur %s\n", j->nom);
        return;
    }

    j->main[j->nb_cartes_main] = c;
    j->nb_cartes_main++;
}

/**
 * Retire une carte de la main d'un joueur et la retourne
 */
Carte retirer_carte_main(Joueur* j, int index) {
    if (index < 0 || index >= j->nb_cartes_main) {
        Carte vide = {0, 0};
        printf("ERREUR: index %d invalide pour main de %s\n", index, j->nom);
        return vide;
    }

    Carte carte_retiree = j->main[index];

    /* Décaler les cartes suivantes */
    for (int i = index; i < j->nb_cartes_main - 1; i++) {
        j->main[i] = j->main[i + 1];
    }

    j->nb_cartes_main--;
    return carte_retiree;
}

/**
 * Affiche l'état d'un joueur
 */
void afficher_joueur(const Joueur* j) {
    printf("Joueur %d: %s (Score: %d, %s, %s)\n",
           j->id, j->nom, j->score,
           j->est_robot ? "robot" : "humain",
           j->est_actif ? "actif" : "inactif");

    if (j->nb_cartes_main > 0) {
        printf("  Main: ");
        afficher_cartes(j->main, j->nb_cartes_main);
    }
}

/* ========== FONCTIONS DES PARTIES ========== */

/**
 * Crée une nouvelle partie
 */
Partie* creer_partie(int id_partie) {
    Partie* p = (Partie*)malloc(sizeof(Partie));
    if (!p) {
        printf("ERREUR: impossible d'allouer mémoire pour partie\n");
        return NULL;
    }

    p->id_partie = id_partie;
    p->nb_joueurs = 0;
    p->joueurs_actifs = 0;
    p->manche_actuelle = 1;
    p->tour_actuel = 1;
    p->joueur_actif = 0;
    p->partie_terminee = 0;
    p->gagnant_id = -1;
    p->index_pioche = 0;

    /* Initialiser les rangées */
    for (int i = 0; i < NB_RANGEES; i++) {
        initialiser_rangee(&p->rangees[i]);
    }

    /* Initialiser et mélanger la pioche */
    initialiser_pioche(p->pioche);
    melanger_pioche(p->pioche, NB_CARTES_TOTAL);

    return p;
}

/**
 * Libère la mémoire d'une partie
 */
void detruire_partie(Partie* p) {
    if (p) {
        free(p);
    }
}

/**
 * Ajoute un joueur à une partie
 */
int ajouter_joueur_partie(Partie* p, Joueur* j) {
    if (p->nb_joueurs >= MAX_JOUEURS) {
        printf("ERREUR: partie pleine (max %d joueurs)\n", MAX_JOUEURS);
        return 0;
    }

    p->joueurs[p->nb_joueurs] = j;
    p->nb_joueurs++;
    p->joueurs_actifs++;

    return 1;
}

/**
 * Distribue les cartes aux joueurs pour une manche
 */
void distribuer_cartes_partie(Partie* p) {
    /* Réinitialiser les mains des joueurs */
    for (int i = 0; i < p->nb_joueurs; i++) {
        p->joueurs[i]->nb_cartes_main = 0;
    }

    /* Distribuer 10 cartes à chaque joueur */
    for (int carte = 0; carte < CARTES_PAR_JOUEUR; carte++) {
        for (int joueur = 0; joueur < p->nb_joueurs; joueur++) {
            if (p->index_pioche < NB_CARTES_TOTAL && p->joueurs[joueur]->est_actif) {
                Carte c = p->pioche[p->index_pioche];
                ajouter_carte_main(p->joueurs[joueur], c);
                p->index_pioche++;
            }
        }
    }
}

/**
 * Initialise les 4 premières cartes sur les rangées
 */
void initialiser_rangees_partie(Partie* p) {
    for (int i = 0; i < NB_RANGEES; i++) {
        if (p->index_pioche < NB_CARTES_TOTAL) {
            Carte c = p->pioche[p->index_pioche];
            ajouter_carte_a_rangee(&p->rangees[i], c);
            p->index_pioche++;
        }
    }
}

/**
 * Trouve la rangée où placer une carte
 * Retourne -1 si la carte est trop petite pour toutes les rangées
 */
int trouver_rangee_pour_carte(const Partie* p, Carte c) {
    int meilleure_rangee = -1;
    int plus_petite_diff = NB_CARTES_TOTAL + 1;  /* plus grand que n'importe quelle carte */

    for (int i = 0; i < NB_RANGEES; i++) {
        Carte derniere = derniere_carte_rangee(&p->rangees[i]);

        /* Si la carte est plus grande que la dernière de la rangée */
        if (c.valeur > derniere.valeur) {
            int diff = c.valeur - derniere.valeur;

            /* On choisit la rangée où la différence est la plus petite */
            if (diff < plus_petite_diff) {
                plus_petite_diff = diff;
                meilleure_rangee = i;
            }
        }
    }

    return meilleure_rangee;
}

/**
 * Trouve la rangée avec le moins de têtes de bœuf
 */
int trouver_rangee_moins_tetes(const Partie* p) {
    int min_tetes = 999;
    int rangee_min = 0;

    for (int i = 0; i < NB_RANGEES; i++) {
        if (p->rangees[i].total_tetes < min_tetes) {
            min_tetes = p->rangees[i].total_tetes;
            rangee_min = i;
        }
    }

    return rangee_min;
}

/**
 * Joue une carte dans la partie
 * Retourne le nombre de têtes de bœuf ramassées (0 si pas ramassé)
 */
int jouer_carte_partie(Partie* p, int id_joueur, Carte c) {
    /* Trouver le joueur */
    Joueur* joueur = NULL;
    int idx_joueur = -1;

    for (int i = 0; i < p->nb_joueurs; i++) {
        if (p->joueurs[i]->id == id_joueur) {
            joueur = p->joueurs[i];
            idx_joueur = i;
            break;
        }
    }

    if (!joueur || !joueur->est_actif) {
        printf("ERREUR: joueur %d non trouvé ou inactif\n", id_joueur);
        return 0;
    }

    /* Trouver où placer la carte */
    int rangee_idx = trouver_rangee_pour_carte(p, c);

    if (rangee_idx == -1) {
        /* Carte trop petite pour toutes les rangées */
        /* Le joueur doit choisir une rangée à ramasser */
        int choix = trouver_rangee_moins_tetes(p);  /* stratégie simple */
        int tetes_ramassees = vider_rangee(&p->rangees[choix]);

        /* Ajouter la carte comme nouvelle première carte */
        ajouter_carte_a_rangee(&p->rangees[choix], c);

        /* Mettre à jour le score du joueur */
        joueur->score += tetes_ramassees;

        return tetes_ramassees;
    } else {
        /* On peut ajouter la carte normalement */
        int rangee_pleine = ajouter_carte_a_rangee(&p->rangees[rangee_idx], c);

        if (rangee_pleine) {
            /* La rangée avait déjà 4 cartes, le joueur la ramasse (5ème carte) */
            int tetes_ramassees = vider_rangee(&p->rangees[rangee_idx]);

            /* Remettre la carte jouée comme première de la rangée */
            ajouter_carte_a_rangee(&p->rangees[rangee_idx], c);

            joueur->score += tetes_ramassees;

            return tetes_ramassees;
        } else {
            return 0;
        }
    }
}

/**
 * Affiche l'état complet de la partie
 */
void afficher_etat_partie(const Partie* p) {
    printf("\n=== ÉTAT DE LA PARTIE #%d ===\n", p->id_partie);
    printf("Manche: %d, Tour: %d, Joueur actif: %d\n",
           p->manche_actuelle, p->tour_actuel, p->joueur_actif);

    printf("\n--- Rangées ---\n");
    for (int i = 0; i < NB_RANGEES; i++) {
        printf("%d. ", i + 1);
        afficher_rangee(&p->rangees[i]);
    }

    printf("\n--- Joueurs ---\n");
    for (int i = 0; i < p->nb_joueurs; i++) {
        if (p->joueurs[i]->est_actif) {
            printf("%d. ", i + 1);
            afficher_joueur(p->joueurs[i]);
        }
    }

    if (p->partie_terminee) {
        printf("\n*** PARTIE TERMINÉE ***\n");
        printf("Gagnant: Joueur %d\n", p->gagnant_id);
    }
    printf("=================================\n\n");
}

/**
 * Vérifie si la partie est terminée
 * Retourne 1 si terminée, 0 sinon
 */
int verifier_partie_terminee(Partie* p) {
    if (p->partie_terminee) return 1;

    /* Vérifier si un joueur a dépassé le seuil */
    for (int i = 0; i < p->nb_joueurs; i++) {
        if (p->joueurs[i]->score >= SEUIL_DEFAITE && p->joueurs[i]->est_actif) {
            p->joueurs[i]->est_actif = 0;
            p->joueurs_actifs--;
            printf("%s est éliminé (score: %d)\n",
                   p->joueurs[i]->nom, p->joueurs[i]->score);
        }
    }

    /* Si un seul joueur actif reste, c'est le gagnant */
    if (p->joueurs_actifs <= 1) {
        p->partie_terminee = 1;

        /* Trouver le joueur avec le score le plus bas */
        int min_score = 999;
        int gagnant_id = -1;

        for (int i = 0; i < p->nb_joueurs; i++) {
            if (p->joueurs[i]->score < min_score) {
                min_score = p->joueurs[i]->score;
                gagnant_id = p->joueurs[i]->id;
            }
        }

        p->gagnant_id = gagnant_id;
        printf("\n*** PARTIE TERMINÉE ***\n");
        printf("Gagnant: Joueur %d avec %d têtes de bœuf\n",
               gagnant_id, min_score);

        return 1;
    }

    return 0;
}

/* ========== FONCTIONS UTILITAIRES ========== */

/**
 * Comparateur pour trier les cartes
 */
int comparer_cartes(const void* a, const void* b) {
    const Carte* carte_a = (const Carte*)a;
    const Carte* carte_b = (const Carte*)b;
    return carte_a->valeur - carte_b->valeur;
}

/**
 * Trie un tableau de cartes par valeur croissante
 */
void trier_cartes(Carte cartes[], int nb_cartes) {
    qsort(cartes, nb_cartes, sizeof(Carte), comparer_cartes);
}

/* ========== FONCTIONS DE TEST ========== */

/**
 * Fonction de test pour vérifier que tout fonctionne
 */
void tester_logique_jeu(void) {
    printf("\n=== TEST DE LA LOGIQUE DU JEU ===\n");

    /* Créer une partie test */
    Partie* partie_test = creer_partie(1);

    /* Ajouter des joueurs */
    Joueur* j1 = creer_joueur(1, "Alice", 0);
    Joueur* j2 = creer_joueur(2, "Bob", 1);
    Joueur* j3 = creer_joueur(3, "Robot1", 1);

    ajouter_joueur_partie(partie_test, j1);
    ajouter_joueur_partie(partie_test, j2);
    ajouter_joueur_partie(partie_test, j3);

    /* Distribuer les cartes */
    distribuer_cartes_partie(partie_test);

    /* Initialiser les rangées */
    initialiser_rangees_partie(partie_test);

    /* Afficher l'état initial */
    afficher_etat_partie(partie_test);

    /* Tester quelques coups */
    printf("\n=== TEST DE QUELQUES COUPS ===\n");

    /* Simuler un coup d'Alice */
    if (j1->nb_cartes_main > 0) {
        Carte carte_a_jouer = retirer_carte_main(j1, 0);
        int tetes = jouer_carte_partie(partie_test, j1->id, carte_a_jouer);
        printf("Alice a pris %d têtes\n", tetes);
        afficher_etat_partie(partie_test);
    }

    /* Simuler un coup de Bob */
    if (j2->nb_cartes_main > 0) {
        Carte carte_a_jouer = retirer_carte_main(j2, 0);
        int tetes = jouer_carte_partie(partie_test, j2->id, carte_a_jouer);
        printf("Bob a pris %d têtes\n", tetes);
        afficher_etat_partie(partie_test);
    }

    /* Nettoyer */
    detruire_partie(partie_test);
    detruire_joueur(j1);
    detruire_joueur(j2);
    detruire_joueur(j3);

    printf("=== FIN DES TESTS ===\n");
}

/* ========== POINT D'ENTRÉE POUR TEST ========== */
#ifdef TEST_JEU
int main() {
    printf("=== TEST MODULE JEU ===\n");

    /* Test création joueur */
    Joueur* joueur = creer_joueur(1, "Testeur", 0);
    if (joueur) {
        printf("✅ Joueur créé: %s (ID: %d)\n", joueur->nom, joueur->id);
    }

    /* Test cartes */
    Carte test_carte = {55, 7};
    printf("Carte test: ");
    afficher_carte(test_carte);
    printf("\n");

    /* Test rangée */
    Rangee rangee;
    initialiser_rangee(&rangee);
    ajouter_carte_a_rangee(&rangee, (Carte){10, 3});
    ajouter_carte_a_rangee(&rangee, (Carte){20, 3});
    printf("Rangée: ");
    afficher_rangee(&rangee);

    /* Test pioche */
    Carte pioche[5];
    for (int i = 0; i < 5; i++) {
        pioche[i].valeur = (i+1) * 10;
        pioche[i].tetes_de_boeuf = calculer_tetes_carte(pioche[i].valeur);
    }
    printf("Pioche: ");
    afficher_cartes(pioche, 5);

    /* Tester la logique complète */
    tester_logique_jeu();

    if (joueur) detruire_joueur(joueur);

    printf("=== FIN TEST JEU ===\n");
    return 0;
}
#endif
