#ifndef GESTIONNAIRE_H
#define GESTIONNAIRE_H

#include "jeu.h"
#include "joueurs.h"
#include "communication.h"

/* ========== CONSTANTES ========== */
#ifndef MAX_CLIENTS_TCP
#define MAX_CLIENTS_TCP 100  // Valeur par défaut
#endif

#define MAX_PARTIES 10
#define MAX_JOUEURS_PAR_PARTIE 10
#define CHEMIN_LOG "logs/parties.log"

/* ========== STRUCTURES ========== */

/* Information sur un joueur connecté */
typedef struct {
    int id;
    Joueur* joueur;
    int fd_in;
    int fd_out;
    int socket_fd;
    pid_t pid;
    pthread_t thread;
    int actif;
} JoueurConnecte;

/* État global du gestionnaire */
typedef struct {
    Partie* parties[MAX_PARTIES];
    int nb_parties;

    JoueurConnecte* joueurs_connectes[MAX_JOUEURS_PAR_PARTIE * MAX_PARTIES];
    int nb_joueurs_connectes;

    ClientTCP clients_tcp[MAX_CLIENTS_TCP];
    int nb_clients_tcp;

    int pipe_public;
    int socket_serveur;

    SemaphoreJeu* sem_rangées;
    SemaphoreJeu* sem_scores;

    char* fichier_log;
    int arreter;
} Gestionnaire;

/* ========== FONCTIONS D'INITIALISATION ========== */
Gestionnaire* creer_gestionnaire(const char* log_path, int port_reseau);
void detruire_gestionnaire(Gestionnaire* g);

/* ========== GESTION DES JOUEURS ========== */
JoueurConnecte* ajouter_joueur(Gestionnaire* g, Joueur* joueur, int est_reseau);
JoueurConnecte* trouver_joueur_connecte(Gestionnaire* g, int id_joueur);
void deconnecter_joueur(Gestionnaire* g, int id_joueur);

/* ========== GESTION DES PARTIES ========== */
Partie* trouver_partie_disponible(Gestionnaire* g);
int ajouter_joueur_a_partie(Gestionnaire* g, int id_joueur, int id_partie);
void demarrer_partie(Gestionnaire* g, Partie* p);
void gerer_tour_partie(Gestionnaire* g, Partie* p);
void terminer_partie(Gestionnaire* g, Partie* p);

/* ========== GESTION RÉSEAU ========== */
void accepter_joueurs_reseau(Gestionnaire* g);
void gerer_message_reseau(Gestionnaire* g, int socket_client);

/* ========== FONCTIONS PRINCIPALES ========== */
void boucle_principale(Gestionnaire* g);
void generer_statistiques(Gestionnaire* g);

/* ========== FONCTIONS UTILITAIRES ========== */
void enregistrer_coup(Gestionnaire* g, Partie* p, int id_joueur, Carte carte, int tetes_prises);
void afficher_etat_global(const Gestionnaire* g);

/* ========== FONCTION POUR LANCER LE GESTIONNAIRE ========== */
void lancer_gestionnaire(int port);  // AJOUTÉ

/* Nouvelles fonctions pour le réseau */
void demarrer_partie_reseau(Gestionnaire* g);
void gerer_commande_reseau(Gestionnaire* g, int socket, const char* commande);
void envoyer_etat_partie(Gestionnaire* g, Partie* p, int socket);
void traiter_nouveau_client(Gestionnaire* g, int socket_client, const char* ip, int port);
void traiter_message_client(Gestionnaire* g, int socket_client);
void nettoyer_clients_deconnectes(Gestionnaire* g);

#endif /* GESTIONNAIRE_H */
