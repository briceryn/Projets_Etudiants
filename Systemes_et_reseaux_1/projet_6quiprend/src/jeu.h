#ifndef JEU_H
#define JEU_H

/* ========== CONSTANTES ========== */
#define NB_CARTES_TOTAL 104
#define NB_MAX_CARTES_RANGEE 6
#define NB_RANGEES 4
#define NB_CARTES_MAIN 10
#define MAX_JOUEURS 10
#define TAILLE_NOM 50

/* ========== STRUCTURES ========== */

/* Une carte avec sa valeur et son nombre de têtes de bœuf */
typedef struct {
    int valeur;
    int tetes_de_boeuf;
} Carte;

/* Une rangée de cartes */
typedef struct {
    Carte cartes[NB_MAX_CARTES_RANGEE];
    int nb_cartes;
    int total_tetes;  /* Total des têtes dans la rangée */
} Rangee;

/* Un joueur */
typedef struct {
    int id;
    char nom[TAILLE_NOM];
    int score;
    Carte main[NB_CARTES_MAIN];
    int nb_cartes_main;
    int est_robot;
    int est_actif;
} Joueur;

/* Une partie complète */
typedef struct {
    int id_partie;
    Joueur* joueurs[MAX_JOUEURS];    /* max 10 joueurs */
    int nb_joueurs;
    int joueurs_actifs;

    Rangee rangees[NB_RANGEES];
    Carte pioche[NB_CARTES_TOTAL];
    int index_pioche;

    int manche_actuelle;
    int tour_actuel;
    int joueur_actif;                /* index du joueur dont c'est le tour */

    int partie_terminee;
    int gagnant_id;
} Partie;

/* ========== FONCTIONS POUR LES CARTES ========== */
int calculer_tetes_carte(int valeur);
void afficher_carte(Carte carte);
void afficher_cartes(const Carte cartes[], int nb_cartes);
void initialiser_pioche(Carte pioche[]);
void melanger_pioche(Carte pioche[], int taille);

/* ========== FONCTIONS POUR LES RANGEES ========== */
void initialiser_rangee(Rangee* rangee);
void afficher_rangee(const Rangee* rangee);
Carte derniere_carte_rangee(const Rangee* rangee);
int peut_ajouter_a_rangee(const Rangee* rangee, Carte carte);
int ajouter_carte_a_rangee(Rangee* rangee, Carte carte);
int vider_rangee(Rangee* rangee);
int calculer_tetes_rangee(const Rangee* rangee);

/* ========== FONCTIONS POUR LES JOUEURS ========== */
Joueur* creer_joueur(int id, const char* nom, int est_robot);
void detruire_joueur(Joueur* joueur);
void afficher_joueur(const Joueur* joueur);
void ajouter_carte_main(Joueur* joueur, Carte carte);
Carte retirer_carte_main(Joueur* joueur, int index);

/* ========== FONCTIONS POUR LES PARTIES ========== */
Partie* creer_partie(int id_partie);
void detruire_partie(Partie* partie);
int ajouter_joueur_partie(Partie* partie, Joueur* joueur);
void afficher_etat_partie(const Partie* partie);
void distribuer_cartes_partie(Partie* partie);
void initialiser_rangees_partie(Partie* partie);
int trouver_rangee_pour_carte(const Partie* partie, Carte carte);
int trouver_rangee_moins_tetes(const Partie* partie);
int jouer_carte_partie(Partie* partie, int id_joueur, Carte carte);
int verifier_partie_terminee(Partie* partie);

/* ========== FONCTIONS UTILITAIRES ========== */
int comparer_cartes(const void* a, const void* b);
void trier_cartes(Carte cartes[], int nb_cartes);

/* ========== FONCTIONS DE TEST ========== */
void tester_logique_jeu(void);

#endif /* JEU_H */
