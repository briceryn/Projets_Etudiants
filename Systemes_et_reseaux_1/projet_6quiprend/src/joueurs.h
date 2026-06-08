#ifndef JOUEURS_H
#define JOUEURS_H

#include "jeu.h"

/* ========== CONSTANTES ========== */
#define TIMEOUT_COUP 30
#define NOM_FIFO_BASE "/tmp/6quiprend_joueur_"

/* ========== STRUCTURES DE COMMUNICATION ========== */

/* Message entre joueur et gestionnaire */
typedef struct {
    int type;
    int id_joueur;
    int id_partie;
    union {
        Carte carte;
        int score;
        int choix;
        char message[256];
    } donnees;
} Message;

/* Types de messages */
enum TypeMessage {
    MSG_DEMANDE_CARTE = 1,
    MSG_CARTE_JOUE = 2,
    MSG_INFO_PARTIE = 3,
    MSG_SCORE = 4,
    MSG_FIN_MANCHE = 5,
    MSG_FIN_PARTIE = 6,
    MSG_ERREUR = 7,
    MSG_PING = 8,
    MSG_CONFIRMATION = 9
};

/* ========== FONCTIONS D'INTERFACE HUMAINE ========== */
void afficher_accueil_joueur(Joueur* j);
int demander_choix_carte_humain(Joueur* j);
void afficher_rangees_joueur(const Rangee rangees[], int nb_rangees);
void afficher_scores_joueurs(Joueur* joueurs[], int nb_joueurs);

/* ========== STRATÉGIES DES ROBOTS ========== */
Carte strategie_petite_carte(Joueur* robot, const Rangee rangees[], int nb_rangees);
Carte strategie_eviter_rangees(Joueur* robot, const Rangee rangees[], int nb_rangees);
Carte strategie_agressive(Joueur* robot, const Rangee rangees[], int nb_rangees);
Carte choisir_carte_robot(Joueur* robot, const Rangee rangees[], int nb_rangees, int niveau);
void retirer_carte_robot(Joueur* robot, Carte carte);

/* ========== GESTION DU TEMPS ========== */
void gestionnaire_timeout(int sig);
int configurer_timeout(int secondes);
void desactiver_timeout(void);

/* ========== COMMUNICATION PAR FIFOs ========== */
void creer_nom_fifo_joueur(char* buffer, int taille, int id_joueur, const char* suffixe);
int creer_fifos_joueur(int id_joueur);
int ouvrir_fifos_joueur(int id_joueur, int* fd_in, int* fd_out);
void fermer_fifos_joueur(int fd_in, int fd_out);
int envoyer_message_fifo(int fd_fifo, Message* msg);  // DÉCLARATION AJOUTÉE
int recevoir_message_fifo(int fd_fifo, Message* msg, int timeout_sec);

/* ========== FONCTIONS PRINCIPALES POUR LES JOUEURS ========== */
void boucle_joueur_humain(Joueur* joueur, int fd_in, int fd_out);
void boucle_joueur_robot(Joueur* robot, int niveau_strategie, int fd_in, int fd_out);

/* ========== FONCTIONS DE TEST ========== */
void tester_interface_joueur(void);
void tester_strategies_robot(void);

#endif /* JOUEURS_H */
