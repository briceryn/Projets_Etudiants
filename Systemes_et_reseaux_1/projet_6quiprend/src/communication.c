/* ====================================================
 * fichier : communication.c
 * but : Communication TCP, FIFOs et sémaphores
 *       - Sockets TCP pour le réseau
 *       - FIFOs pour communication locale
 *       - Sémaphores pour synchronisation
 * ==================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include "communication.h"

/* ========== FONCTIONS FILES D'ATTENTE (FIFOs) ========== */

/**
 * Crée un FIFO (tube nommé)
 */
int creer_fifo(const char* chemin) {
    if (mkfifo(chemin, 0666) == -1) {
        if (errno != EEXIST) {
            perror("[FIFO] Erreur création");
            return -1;
        }
    }
    printf("[FIFO] Créé: %s\n", chemin);
    return 0;
}

/**
 * Ouvre un FIFO
 */
int ouvrir_fifo(const char* chemin, int mode) {
    int fd = open(chemin, mode);
    if (fd == -1) {
        perror("[FIFO] Erreur ouverture");
    } else {
        printf("[FIFO] Ouvert: %s (fd: %d)\n", chemin, fd);
    }
    return fd;
}

/**
 * Ferme un FIFO
 */
void fermer_fifo(int fd, const char* chemin, int supprimer) {
    if (fd != -1) {
        close(fd);
        printf("[FIFO] Fermé: %s (fd: %d)\n", chemin, fd);
    }
    if (supprimer && chemin != NULL) {
        unlink(chemin);
        printf("[FIFO] Supprimé: %s\n", chemin);
    }
}

/**
 * Envoie des données via FIFO
 */
int envoyer_fifo(int fd, const void* data, size_t taille) {
    ssize_t bytes_ecrits = write(fd, data, taille);
    if (bytes_ecrits != (ssize_t)taille) {
        perror("[FIFO] Erreur écriture");
        return 0;
    }
    return 1;
}

/**
 * Reçoit des données via FIFO
 */
int recevoir_fifo(int fd, void* data, size_t taille, int timeout_sec) {
    if (timeout_sec > 0) {
        /* Implémentation basique du timeout pour FIFO */
        fd_set set;
        struct timeval timeout;

        FD_ZERO(&set);
        FD_SET(fd, &set);

        timeout.tv_sec = timeout_sec;
        timeout.tv_usec = 0;

        if (select(fd + 1, &set, NULL, NULL, &timeout) <= 0) {
            printf("[FIFO] Timeout lecture\n");
            return 0;
        }
    }

    ssize_t bytes_lus = read(fd, data, taille);
    if (bytes_lus <= 0) {
        perror("[FIFO] Erreur lecture");
        return 0;
    }

    return 1;
}

/* ========== FONCTIONS SÉMAPHORES ========== */

/**
 * Crée un ensemble de sémaphores
 */
SemaphoreJeu* creer_semaphores(int nb_sem, int valeur_init) {
    key_t key = CLE_SEM;
    int sem_id;

    /* Créer ou récupérer l'ensemble de sémaphores */
    sem_id = semget(key, nb_sem, IPC_CREAT | IPC_EXCL | 0666);
    if (sem_id == -1 && errno == EEXIST) {
        sem_id = semget(key, nb_sem, 0);
    }

    if (sem_id == -1) {
        perror("[SEM] Erreur semget");
        return NULL;
    }

    /* Initialiser les sémaphores */
    if (errno != EEXIST) {
        union semun {
            int val;
            struct semid_ds *buf;
            unsigned short *array;
        } arg;

        arg.val = valeur_init;

        for (int i = 0; i < nb_sem; i++) {
            if (semctl(sem_id, i, SETVAL, arg) == -1) {
                perror("[SEM] Erreur initialisation");
                semctl(sem_id, 0, IPC_RMID);
                return NULL;
            }
        }
    }

    SemaphoreJeu* sem = malloc(sizeof(SemaphoreJeu));
    sem->sem_id = sem_id;
    sem->nb_sem = nb_sem;

    printf("[SEM] Créé (id: %d, nb: %d)\n", sem_id, nb_sem);
    return sem;
}

/**
 * Détruit les sémaphores
 */
void detruire_semaphores(SemaphoreJeu* sem) {
    if (sem != NULL) {
        if (semctl(sem->sem_id, 0, IPC_RMID) == -1) {
            perror("[SEM] Erreur suppression");
        } else {
            printf("[SEM] Détruit (id: %d)\n", sem->sem_id);
        }
        free(sem);
    }
}

/**
 * Opération wait (P) sur un sémaphore
 */
void sem_wait_jeu(SemaphoreJeu* sem, int num_sem) {
    struct sembuf op = {num_sem, -1, 0};
    if (semop(sem->sem_id, &op, 1) == -1) {
        perror("[SEM] Erreur wait");
    }
}

/**
 * Opération signal (V) sur un sémaphore
 */
void sem_signal_jeu(SemaphoreJeu* sem, int num_sem) {
    struct sembuf op = {num_sem, 1, 0};
    if (semop(sem->sem_id, &op, 1) == -1) {
        perror("[SEM] Erreur signal");
    }
}

/**
 * Obtient la valeur d'un sémaphore
 */
int sem_getvalue(SemaphoreJeu* sem, int num_sem) {
    int val = semctl(sem->sem_id, num_sem, GETVAL);
    if (val == -1) {
        perror("[SEM] Erreur getvalue");
    }
    return val;
}

/* ========== FONCTIONS TCP (SOCKETS STREAM) ========== */

/**
 * Crée un socket serveur TCP
 * Retourne le descripteur de socket, -1 en cas d'erreur
 */
int creer_serveur_tcp(int port) {
    int sockfd;
    struct sockaddr_in serveur_addr;
    int opt = 1;

    /* Création du socket TCP (SOCK_STREAM) */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("[TCP] Erreur création socket");
        return -1;
    }

    /* Options du socket : réutiliser l'adresse */
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("[TCP] Erreur setsockopt");
        close(sockfd);
        return -1;
    }

    /* Configuration de l'adresse du serveur */
    memset(&serveur_addr, 0, sizeof(serveur_addr));
    serveur_addr.sin_family = AF_INET;
    serveur_addr.sin_addr.s_addr = INADDR_ANY;  /* Accepte toutes les interfaces */
    serveur_addr.sin_port = htons(port);

    /* Liaison du socket à l'adresse */
    if (bind(sockfd, (struct sockaddr*)&serveur_addr, sizeof(serveur_addr)) == -1) {
        perror("[TCP] Erreur bind");
        close(sockfd);
        return -1;
    }

    /* Mise en écoute du socket */
    if (listen(sockfd, BACKLOG_TCP) == -1) {
        perror("[TCP] Erreur listen");
        close(sockfd);
        return -1;
    }

    printf("[TCP] Serveur démarré sur le port %d (socket: %d)\n", port, sockfd);
    return sockfd;
}

/**
 * Accepte une connexion TCP entrante
 */
int accepter_connexion_tcp(int sock_serveur, char* ip_client, int* port_client) {
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    client_sock = accept(sock_serveur, (struct sockaddr*)&client_addr, &addr_len);
    if (client_sock == -1) {
        perror("[TCP] Erreur accept");
        return -1;
    }

    /* Récupération des infos du client */
    if (ip_client != NULL) {
        inet_ntop(AF_INET, &client_addr.sin_addr, ip_client, INET_ADDRSTRLEN);
    }

    if (port_client != NULL) {
        *port_client = ntohs(client_addr.sin_port);
    }

    printf("[TCP] Nouveau client: %s:%d (socket: %d)\n",
           ip_client ? ip_client : "?",
           port_client ? *port_client : 0,
           client_sock);

    return client_sock;
}

/**
 * Connecte un client TCP à un serveur
 */
int connecter_client_tcp(const char* adresse, int port) {
    int sockfd;
    struct sockaddr_in serveur_addr;

    /* Création du socket TCP */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("[TCP Client] Erreur création socket");
        return -1;
    }

    /* Configuration de l'adresse du serveur */
    memset(&serveur_addr, 0, sizeof(serveur_addr));
    serveur_addr.sin_family = AF_INET;
    serveur_addr.sin_port = htons(port);

    /* Conversion de l'adresse IP */
    if (inet_pton(AF_INET, adresse, &serveur_addr.sin_addr) <= 0) {
        fprintf(stderr, "[TCP Client] Adresse invalide: %s\n", adresse);
        close(sockfd);
        return -1;
    }

    /* Connexion au serveur */
    if (connect(sockfd, (struct sockaddr*)&serveur_addr, sizeof(serveur_addr)) == -1) {
        perror("[TCP Client] Erreur connect");
        close(sockfd);
        return -1;
    }

    printf("[TCP Client] Connecté à %s:%d\n", adresse, port);
    return sockfd;
}

/**
 * Envoie des données via TCP
 * Retourne 1 si succès, 0 sinon
 */
int envoyer_tcp(int socket, const void* data, size_t taille) {
    ssize_t bytes_envoyes;

    /* D'abord envoyer la taille des données */
    uint32_t taille_network = htonl(taille);
    bytes_envoyes = send(socket, &taille_network, sizeof(taille_network), 0);
    if (bytes_envoyes != sizeof(taille_network)) {
        perror("[TCP] Erreur envoi taille");
        return 0;
    }

    /* Ensuite envoyer les données elles-mêmes */
    bytes_envoyes = send(socket, data, taille, 0);
    if (bytes_envoyes != (ssize_t)taille) {
        perror("[TCP] Erreur envoi données");
        return 0;
    }

    return 1;
}

/**
 * Reçoit des données via TCP
 * Retourne 1 si succès, 0 sinon
 */
int recevoir_tcp(int socket, void* buffer, size_t taille_max, int timeout_sec) {
    uint32_t taille_reseau;
    uint32_t taille_donnees;
    ssize_t bytes_recus;

    /* Configurer le timeout si demandé */
    if (timeout_sec > 0) {
        struct timeval tv;
        tv.tv_sec = timeout_sec;
        tv.tv_usec = 0;

        if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
            perror("[TCP] Erreur setsockopt timeout");
        }
    }

    /* D'abord recevoir la taille des données */
    bytes_recus = recv(socket, &taille_reseau, sizeof(taille_reseau), 0);
    if (bytes_recus <= 0) {
        if (bytes_recus == 0) {
            printf("[TCP] Connexion fermée par le pair\n");
        } else {
            perror("[TCP] Erreur réception taille");
        }
        return 0;
    }

    taille_donnees = ntohl(taille_reseau);

    /* Vérifier que la taille est raisonnable */
    if (taille_donnees > taille_max) {
        fprintf(stderr, "[TCP] Taille des données trop grande: %u > %lu\n",
                taille_donnees, taille_max);
        return 0;
    }

    /* Recevoir les données */
    bytes_recus = recv(socket, buffer, taille_donnees, 0);
    if (bytes_recus != (ssize_t)taille_donnees) {
        perror("[TCP] Erreur réception données");
        return 0;
    }

    /* Désactiver le timeout */
    if (timeout_sec > 0) {
        struct timeval tv = {0, 0};
        setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    }

    return 1;
}

/**
 * Gestion de multiples sockets avec select() (pour le serveur)
 */
int surveiller_sockets_tcp(int sock_serveur, ClientTCP clients[],
                          int* nb_clients, int max_clients) {
    fd_set readfds;
    int max_fd = sock_serveur;
    int activite;

    /* Initialiser l'ensemble de descripteurs */
    FD_ZERO(&readfds);
    FD_SET(sock_serveur, &readfds);

    /* Ajouter les sockets clients actifs */
    for (int i = 0; i < *nb_clients; i++) {
        if (clients[i].est_actif) {
            FD_SET(clients[i].socket, &readfds);
            if (clients[i].socket > max_fd) {
                max_fd = clients[i].socket;
            }
        }
    }

    /* Timeout court pour ne pas bloquer */
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;  /* 100ms */

    activite = select(max_fd + 1, &readfds, NULL, NULL, &timeout);

    if (activite == -1) {
        perror("[TCP Select] Erreur");
        return -1;
    }

    if (activite == 0) {
        /* Timeout, aucune activité */
        return 0;
    }

    /* Nouvelle connexion sur le socket serveur ? */
    if (FD_ISSET(sock_serveur, &readfds)) {
        return -2;  /* Code spécial pour nouvelle connexion */
    }

    /* Chercher quel client a des données à lire */
    for (int i = 0; i < *nb_clients; i++) {
        if (clients[i].est_actif && FD_ISSET(clients[i].socket, &readfds)) {
            return clients[i].socket;
        }
    }

    return 0;
}

/**
 * Ajoute un client TCP à la liste
 */
int ajouter_client_tcp(ClientTCP clients[], int* nb_clients,
                      int socket, const char* ip, int port) {
    if (*nb_clients >= MAX_CLIENTS_TCP) {
        printf("[TCP] Nombre maximum de clients atteint\n");
        return 0;
    }

    clients[*nb_clients].socket = socket;
    strncpy(clients[*nb_clients].adresse_ip, ip, INET_ADDRSTRLEN);
    clients[*nb_clients].port = port;
    clients[*nb_clients].est_actif = 1;
    clients[*nb_clients].derniere_activite = time(NULL);

    (*nb_clients)++;
    return 1;
}

/**
 * Trouve un client par son socket
 */
ClientTCP* trouver_client_par_socket(ClientTCP clients[], int nb_clients, int socket) {
    for (int i = 0; i < nb_clients; i++) {
        if (clients[i].socket == socket && clients[i].est_actif) {
            return &clients[i];
        }
    }
    return NULL;
}

/**
 * Ferme proprement une connexion TCP
 */
void fermer_connexion_tcp(int socket) {
    if (socket != -1) {
        shutdown(socket, SHUT_RDWR);  /* Arrête la communication dans les deux sens */
        close(socket);
        printf("[TCP] Connexion fermée (socket: %d)\n", socket);
    }
}

/* ========== FONCTIONS DE JOURNALSATION ========== */

/**
 * Écrit un message dans un fichier de log
 */
void ecrire_log(const char* fichier_log, const char* message) {
    FILE* log_file = fopen(fichier_log, "a");
    if (log_file == NULL) {
        perror("[LOG] Erreur ouverture fichier");
        return;
    }

    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(log_file, "[%s] %s\n", timestamp, message);
    fclose(log_file);
}

/* ========== FONCTIONS DE TEST ========== */

/**
 * Test complet des communications
 */
void tester_communications(void) {
    printf("\n=== TEST COMPLET DES COMMUNICATIONS ===\n");

    /* Test FIFOs */
    printf("\n1. Test FIFOs...\n");
    const char* fifo_test = "/tmp/test_fifo_6quiprend";

    if (creer_fifo(fifo_test) == 0) {
        printf("✓ FIFO créé avec succès\n");

        /* Test écriture/lecture dans un processus enfant */
        if (fork() == 0) {
            /* Processus enfant (lecteur) */
            int fd = ouvrir_fifo(fifo_test, O_RDONLY);
            if (fd != -1) {
                char buffer[100];
                if (recevoir_fifo(fd, buffer, sizeof(buffer), 2)) {
                    printf("✓ Processus enfant a reçu: %s\n", buffer);
                }
                fermer_fifo(fd, fifo_test, 0);
            }
            exit(0);
        } else {
            /* Processus parent (écrivain) */
            sleep(1);  /* Attendre que l'enfant soit prêt */
            int fd = ouvrir_fifo(fifo_test, O_WRONLY);
            if (fd != -1) {
                const char* message = "Test FIFO réussi!";
                if (envoyer_fifo(fd, message, strlen(message) + 1)) {
                    printf("✓ Processus parent a envoyé: %s\n", message);
                }
                fermer_fifo(fd, fifo_test, 1);  /* Supprimer le FIFO */
            }
            wait(NULL);  /* Attendre l'enfant */
        }
    }

    /* Test sémaphores */
    printf("\n2. Test sémaphores...\n");
    SemaphoreJeu* sem = creer_semaphores(1, 1);
    if (sem != NULL) {
        printf("✓ Sémaphore créé (valeur initiale: %d)\n", sem_getvalue(sem, 0));

        printf("  Avant wait: valeur = %d\n", sem_getvalue(sem, 0));
        sem_wait_jeu(sem, 0);
        printf("  Après wait: valeur = %d\n", sem_getvalue(sem, 0));
        sem_signal_jeu(sem, 0);
        printf("  Après signal: valeur = %d\n", sem_getvalue(sem, 0));

        detruire_semaphores(sem);
        printf("✓ Sémaphore détruit\n");
    }

    /* Test sockets TCP */
    printf("\n3. Test sockets TCP...\n");
    int test_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (test_sock != -1) {
        printf("✓ Socket TCP créé avec succès\n");
        close(test_sock);
    } else {
        printf("✗ Erreur création socket TCP\n");
    }

    printf("\n=== FIN DES TESTS DE COMMUNICATION ===\n");
}

/**
 * Test serveur TCP simple
 */
void tester_serveur_tcp(int port) {
    printf("\n=== TEST SERVEUR TCP (port: %d) ===\n", port);

    int sock_serveur = creer_serveur_tcp(port);
    if (sock_serveur == -1) {
        printf("Échec création serveur\n");
        return;
    }

    ClientTCP clients[MAX_CLIENTS_TCP];
    int nb_clients = 0;

    printf("Serveur en attente de connexions...\n");
    printf("Ctrl+C pour arrêter\n\n");

    /* Ignorer SIGPIPE pour éviter les crashs */
    signal(SIGPIPE, SIG_IGN);

    while (1) {
        int result = surveiller_sockets_tcp(sock_serveur, clients, &nb_clients, MAX_CLIENTS_TCP);

        if (result == -2) {
            /* Nouvelle connexion */
            char ip[INET_ADDRSTRLEN];
            int port_client;

            int new_sock = accepter_connexion_tcp(sock_serveur, ip, &port_client);
            if (new_sock != -1) {
                ajouter_client_tcp(clients, &nb_clients, new_sock, ip, port_client);
                printf("Total clients: %d\n", nb_clients);

                /* Envoyer un message de bienvenue */
                char bienvenue[] = "Bienvenue sur le serveur 6 qui prend!\n";
                envoyer_tcp(new_sock, bienvenue, sizeof(bienvenue));
            }
        }
        else if (result > 0) {
            /* Données reçues d'un client */
            char buffer[TAILLE_BUFFER_TCP];

            if (recevoir_tcp(result, buffer, sizeof(buffer), 1)) {
                ClientTCP* client = trouver_client_par_socket(clients, nb_clients, result);
                printf("Message de %s:%d: %s\n",
                       client ? client->adresse_ip : "inconnu",
                       client ? client->port : 0,
                       buffer);

                /* Répondre */
                char reponse[TAILLE_BUFFER_TCP];
                snprintf(reponse, sizeof(reponse), "Reçu: %s", buffer);
                envoyer_tcp(result, reponse, strlen(reponse) + 1);
            } else {
                /* Client déconnecté */
                printf("Client %d déconnecté\n", result);
                fermer_connexion_tcp(result);

                /* Retirer de la liste */
                for (int i = 0; i < nb_clients; i++) {
                    if (clients[i].socket == result) {
                        clients[i].est_actif = 0;
                        break;
                    }
                }
            }
        }

        usleep(100000);  /* 100ms */
    }

    fermer_connexion_tcp(sock_serveur);
}

/* ========== POINT D'ENTRÉE POUR TEST ========== */
#ifdef TEST_COMM
int main(int argc, char* argv[]) {
    printf("=== TEST MODULE COMMUNICATION ===\n");

    if (argc < 2) {
        printf("Usage: %s --test\n", argv[0]);
        printf("       %s --serveur [port]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--test") == 0) {
        tester_communications();
    }
    else if (strcmp(argv[1], "--serveur") == 0) {
        int port = (argc > 2) ? atoi(argv[2]) : PORT_DEFAUT;
        tester_serveur_tcp(port);
    }
    else {
        printf("Option non reconnue\n");
        printf("Options valides: --test, --serveur\n");
    }

    return 0;
}
#endif
