#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* ========== CONSTANTES TCP ========== */
#define PORT_DEFAUT 8080
#define MAX_CLIENTS_TCP 100
#define TAILLE_BUFFER_TCP 1024
#define BACKLOG_TCP 10
#define NOM_FIFO_PUBLIC "/tmp/6quiprend_public"
#define CLE_SEM 0x6A1B2C3D  /* Corrigé: nombre hexadécimal valide */

/* ========== STRUCTURES TCP ========== */

typedef struct {
    int socket;
    char adresse_ip[INET_ADDRSTRLEN];
    int port;
    int est_actif;
    time_t derniere_activite;
} ClientTCP;

typedef struct {
    int sem_id;
    int nb_sem;
} SemaphoreJeu;

/* ========== FONCTIONS FILES D'ATTENTE (FIFOs) ========== */
int creer_fifo(const char* chemin);
int ouvrir_fifo(const char* chemin, int mode);
void fermer_fifo(int fd, const char* chemin, int supprimer);
int envoyer_fifo(int fd, const void* data, size_t taille);
int recevoir_fifo(int fd, void* data, size_t taille, int timeout_sec);

/* ========== FONCTIONS SÉMAPHORES ========== */
SemaphoreJeu* creer_semaphores(int nb_sem, int valeur_init);
void detruire_semaphores(SemaphoreJeu* sem);
void sem_wait_jeu(SemaphoreJeu* sem, int num_sem);
void sem_signal_jeu(SemaphoreJeu* sem, int num_sem);
int sem_getvalue(SemaphoreJeu* sem, int num_sem);

/* ========== FONCTIONS TCP (SOCKETS STREAM) ========== */
int creer_serveur_tcp(int port);
int accepter_connexion_tcp(int sock_serveur, char* ip_client, int* port_client);
int connecter_client_tcp(const char* adresse, int port);
int envoyer_tcp(int socket, const void* data, size_t taille);
int recevoir_tcp(int socket, void* buffer, size_t taille_max, int timeout_sec);
int surveiller_sockets_tcp(int sock_serveur, ClientTCP clients[],
                          int* nb_clients, int max_clients);
int ajouter_client_tcp(ClientTCP clients[], int* nb_clients,
                      int socket, const char* ip, int port);
ClientTCP* trouver_client_par_socket(ClientTCP clients[], int nb_clients, int socket);
void fermer_connexion_tcp(int socket);

/* ========== FONCTIONS DE JOURNALSATION ========== */
void ecrire_log(const char* fichier_log, const char* message);

/* ========== FONCTIONS DE TEST ========== */
void tester_communications(void);
void tester_serveur_tcp(int port);

#endif /* COMMUNICATION_H */
