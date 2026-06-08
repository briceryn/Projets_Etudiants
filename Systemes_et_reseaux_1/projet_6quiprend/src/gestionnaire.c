#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include <sys/select.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "jeu.h"
#include "joueurs.h"
#include "communication.h"
#include "gestionnaire.h"

/* Constantes */
#define TIMEOUT_JOUEUR 30
#define LOG_DIR "logs"

/* Créer un répertoire pour les logs */
static void creer_repertoire_logs(void) {
    struct stat st = {0};
    if (stat(LOG_DIR, &st) == -1) {
        mkdir(LOG_DIR, 0700);
    }
}

/* Version complète de creer_gestionnaire */
Gestionnaire* creer_gestionnaire(const char* log_path, int port_reseau) {
    Gestionnaire* g = malloc(sizeof(Gestionnaire));
    if (!g) return NULL;

    /* Initialiser les champs */
    g->nb_parties = 0;
    g->nb_joueurs_connectes = 0;
    g->nb_clients_tcp = 0;
    g->arreter = 0;
    g->socket_serveur = -1;
    g->pipe_public = -1;
    g->sem_rangées = NULL;
    g->sem_scores = NULL;

    /* Créer le chemin de log */
    if (log_path) {
        g->fichier_log = strdup(log_path);
    } else {
        g->fichier_log = strdup(CHEMIN_LOG);
    }

    /* Initialiser les tableaux */
    for (int i = 0; i < MAX_PARTIES; i++) g->parties[i] = NULL;
    for (int i = 0; i < MAX_JOUEURS_PAR_PARTIE * MAX_PARTIES; i++) {
        g->joueurs_connectes[i] = NULL;
    }
    for (int i = 0; i < MAX_CLIENTS_TCP; i++) {
        g->clients_tcp[i].socket = -1;
        g->clients_tcp[i].est_actif = 0;
    }

    /* Créer le répertoire de logs */
    creer_repertoire_logs();

    /* Démarrer le serveur TCP si un port est spécifié */
    if (port_reseau > 0) {
        g->socket_serveur = creer_serveur_tcp(port_reseau);
        if (g->socket_serveur == -1) {
            printf("⚠️ Impossible de démarrer le serveur TCP sur le port %d\n", port_reseau);
        } else {
            printf("✅ Serveur TCP démarré sur le port %d\n", port_reseau);
        }
    }

    /* Créer les sémaphores */
    g->sem_rangées = creer_semaphores(1, 1);  /* Pour synchroniser l'accès aux rangées */
    g->sem_scores = creer_semaphores(1, 1);   /* Pour synchroniser les scores */

    printf("✅ Gestionnaire initialisé\n");
    printf("   Port réseau: %d\n", port_reseau);
    printf("   Fichier log: %s\n", g->fichier_log);

    return g;
}

/* Détruire le gestionnaire */
void detruire_gestionnaire(Gestionnaire* g) {
    if (!g) return;

    printf("🔧 Nettoyage du gestionnaire...\n");

    /* Fermer les connexions TCP */
    if (g->socket_serveur != -1) {
        close(g->socket_serveur);
    }

    /* Fermer les FIFOs publics */
    if (g->pipe_public != -1) {
        close(g->pipe_public);
    }

    /* Détruire les sémaphores */
    if (g->sem_rangées) detruire_semaphores(g->sem_rangées);
    if (g->sem_scores) detruire_semaphores(g->sem_scores);

    /* Libérer la mémoire */
    if (g->fichier_log) free(g->fichier_log);

    /* Détruire toutes les parties */
    for (int i = 0; i < g->nb_parties; i++) {
        if (g->parties[i]) {
            /* Libérer la mémoire de la partie */
            free(g->parties[i]);
        }
    }

    /* Détruire les joueurs connectés */
    for (int i = 0; i < g->nb_joueurs_connectes; i++) {
        if (g->joueurs_connectes[i]) {
            if (g->joueurs_connectes[i]->joueur) {
                detruire_joueur(g->joueurs_connectes[i]->joueur);
            }
            free(g->joueurs_connectes[i]);
        }
    }

    free(g);
    printf("✅ Gestionnaire détruit\n");
}

void boucle_principale(Gestionnaire* g) {
    if (!g) return;

    printf("🔄 Boucle principale démarrée\n");
    printf("   Mode: %s\n", g->socket_serveur != -1 ? "Réseau" : "Local");

    fd_set readfds;
    struct timeval timeout;

    while (!g->arreter) {
        /* Si en mode réseau, surveiller les sockets */
        if (g->socket_serveur != -1) {
            FD_ZERO(&readfds);
            FD_SET(g->socket_serveur, &readfds);
            int max_fd = g->socket_serveur;

            /* Ajouter les clients TCP */
            for (int i = 0; i < g->nb_clients_tcp; i++) {
                if (g->clients_tcp[i].est_actif && g->clients_tcp[i].socket != -1) {
                    FD_SET(g->clients_tcp[i].socket, &readfds);
                    if (g->clients_tcp[i].socket > max_fd) {
                        max_fd = g->clients_tcp[i].socket;
                    }
                }
            }

            /* Timeout de 1 seconde */
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            int activite = select(max_fd + 1, &readfds, NULL, NULL, &timeout);

            if (activite > 0) {
                /* ========== 1. NOUVELLE CONNEXION ========== */
                if (FD_ISSET(g->socket_serveur, &readfds)) {
                    char ip_client[INET_ADDRSTRLEN];
                    int port_client;

                    int new_sock = accepter_connexion_tcp(g->socket_serveur, ip_client, &port_client);
                    if (new_sock != -1) {
                        printf("➕ Nouveau client: %s:%d (socket: %d)\n", ip_client, port_client, new_sock);

                        /* Vérifier limite clients */
                        if (g->nb_clients_tcp < MAX_CLIENTS_TCP) {
                            /* Ajouter à liste clients TCP */
                            int index = g->nb_clients_tcp;
                            g->clients_tcp[index].socket = new_sock;
                            strncpy(g->clients_tcp[index].adresse_ip, ip_client, INET_ADDRSTRLEN);
                            g->clients_tcp[index].port = port_client;
                            g->clients_tcp[index].est_actif = 1;
                            g->clients_tcp[index].derniere_activite = time(NULL);
                            g->nb_clients_tcp++;

                            /* Créer un joueur pour ce client */
                            char nom_joueur[50];
                            snprintf(nom_joueur, sizeof(nom_joueur), "Joueur%d", index + 1);

                            Joueur* nouveau_joueur = creer_joueur(index + 1, nom_joueur, 0);
                            JoueurConnecte* jc = ajouter_joueur(g, nouveau_joueur, 1);

                            if (jc) {
                                jc->socket_fd = new_sock;

                                /* Envoyer message de bienvenue détaillé */
                                char bienvenue[512];
                                snprintf(bienvenue, sizeof(bienvenue),
                                        "\n=== BIENVENUE À 6 QUI PREND ===\n\n"
                                        "Vous êtes: %s (ID: %d)\n\n"
                                        "Commandes disponibles:\n"
                                        "  cartes    - Voir vos cartes\n"
                                        "  jouer X   - Jouer la carte X (ex: 'jouer 1')\n"
                                        "  etat      - Voir l'état de la partie\n"
                                        "  quit      - Quitter\n\n"
                                        "En attente d'autres joueurs...\n"
                                        "(La partie démarre à 2 joueurs minimum)\n\n",
                                        nom_joueur, nouveau_joueur->id);

                                send(new_sock, bienvenue, strlen(bienvenue), 0);

                                /* Démarrer une partie si on a assez de joueurs */
                                if (g->nb_clients_tcp >= 2) {
                                    demarrer_partie_reseau(g);
                                }
                            }
                        } else {
                            char* msg = "❌ Serveur plein! Désolé.\n";
                            send(new_sock, msg, strlen(msg), 0);
                            close(new_sock);
                            printf("❌ Trop de clients connectés\n");
                        }
                    }
                }

                /* ========== 2. MESSAGES DES CLIENTS EXISTANTS ========== */
                for (int i = 0; i < g->nb_clients_tcp; i++) {
                    if (g->clients_tcp[i].est_actif &&
                        g->clients_tcp[i].socket != -1 &&
                        FD_ISSET(g->clients_tcp[i].socket, &readfds)) {

                        char buffer[256];
                        memset(buffer, 0, sizeof(buffer));
                        int bytes = recv(g->clients_tcp[i].socket, buffer, sizeof(buffer) - 1, 0);

                        if (bytes > 0) {
                            buffer[bytes] = '\0';
                            buffer[strcspn(buffer, "\n")] = '\0';  // Enlever \n

                            printf("📨 Message de %s:%d: %s\n",
                                   g->clients_tcp[i].adresse_ip,
                                   g->clients_tcp[i].port,
                                   buffer);

                            /* Mettre à jour timestamp activité */
                            g->clients_tcp[i].derniere_activite = time(NULL);

                            /* Traiter la commande */
                            gerer_commande_reseau(g, g->clients_tcp[i].socket, buffer);

                        } else if (bytes == 0) {
                            /* Client déconnecté proprement */
                            printf("🔌 Client déconnecté: %s:%d\n",
                                   g->clients_tcp[i].adresse_ip,
                                   g->clients_tcp[i].port);

                            close(g->clients_tcp[i].socket);
                            g->clients_tcp[i].est_actif = 0;
                            g->clients_tcp[i].socket = -1;

                            /* Retirer le joueur des parties */
                            for (int j = 0; j < g->nb_joueurs_connectes; j++) {
                                if (g->joueurs_connectes[j] &&
                                    g->joueurs_connectes[j]->socket_fd == g->clients_tcp[i].socket) {
                                    deconnecter_joueur(g, g->joueurs_connectes[j]->id);
                                    break;
                                }
                            }
                        }
                        /* bytes < 0 : erreur, on ignore */
                    }
                }
            }

            /* ========== 3. NETTOYAGE CLIENTS INACTIFS ========== */
            time_t maintenant = time(NULL);
            for (int i = 0; i < g->nb_clients_tcp; i++) {
                if (g->clients_tcp[i].est_actif && g->clients_tcp[i].socket != -1) {
                    /* Timeout après 60 secondes d'inactivité */
                    if (difftime(maintenant, g->clients_tcp[i].derniere_activite) > 500) {
                        printf("⏰ Timeout client: %s:%d (inactif)\n",
                               g->clients_tcp[i].adresse_ip,
                               g->clients_tcp[i].port);

                        char* msg = "⏰ Déconnecté pour inactivité\n";
                        send(g->clients_tcp[i].socket, msg, strlen(msg), 0);

                        close(g->clients_tcp[i].socket);
                        g->clients_tcp[i].est_actif = 0;
                        g->clients_tcp[i].socket = -1;
                    }
                }
            }
        }

        /* ========== GESTION DES PARTIES LOCALES ========== */
        /* Gérer les tours des parties en cours (pour robots locaux) */
        for (int i = 0; i < g->nb_parties; i++) {
            if (g->parties[i] && g->parties[i]->partie_terminee == 0) {
                gerer_tour_partie(g, g->parties[i]);
            }
        }

        /* Petite pause pour éviter la surcharge CPU */
        usleep(100000);  // 100ms
    }

    printf("✅ Boucle principale terminée\n");
}


/* Fonction pour lancer le gestionnaire (à appeler depuis main) */
void lancer_gestionnaire(int port) {
    printf("🚀 Lancement du gestionnaire de jeu...\n");

    /* Créer le gestionnaire */
    Gestionnaire* gestionnaire = creer_gestionnaire(NULL, port);
    if (!gestionnaire) {
        printf("❌ Impossible de créer le gestionnaire\n");
        return;
    }

    /* Lancer la boucle principale */
    boucle_principale(gestionnaire);

    /* Nettoyer */
    detruire_gestionnaire(gestionnaire);

    printf("🎮 Gestionnaire arrêté\n");
}

/* Trouver une partie disponible */
Partie* trouver_partie_disponible(Gestionnaire* g) {
    printf("🔍 Recherche d'une partie disponible...\n");

    /* Chercher une partie avec de la place */
    for (int i = 0; i < g->nb_parties; i++) {
        if (g->parties[i] && g->parties[i]->nb_joueurs < MAX_JOUEURS &&
            g->parties[i]->partie_terminee == 0) {
            printf("✅ Partie trouvée (ID: %d, joueurs: %d/%d)\n",
                   g->parties[i]->id_partie,
                   g->parties[i]->nb_joueurs,
                   MAX_JOUEURS);
            return g->parties[i];
        }
    }

    /* Si pas de partie disponible, en créer une nouvelle */
    if (g->nb_parties < MAX_PARTIES) {
        Partie* nouvelle = malloc(sizeof(Partie));
        if (nouvelle) {
            nouvelle->id_partie = g->nb_parties + 1;
            nouvelle->nb_joueurs = 0;
            nouvelle->joueurs_actifs = 0;
            nouvelle->manche_actuelle = 0;
            nouvelle->tour_actuel = 0;
            nouvelle->joueur_actif = 0;
            nouvelle->partie_terminee = 0;
            nouvelle->gagnant_id = -1;
            nouvelle->index_pioche = 0;

            /* Initialiser les rangées */
            for (int r = 0; r < NB_RANGEES; r++) {
                initialiser_rangee(&nouvelle->rangees[r]);
            }

            /* Ajouter à la liste des parties */
            g->parties[g->nb_parties++] = nouvelle;

            printf("✅ Nouvelle partie créée (ID: %d)\n", nouvelle->id_partie);
            return nouvelle;
        }
    }

    printf("❌ Aucune partie disponible\n");
    return NULL;
}

int ajouter_joueur_a_partie(Gestionnaire* g, int id_joueur, int id_partie) {
    printf("➕ Tentative d'ajout du joueur %d à la partie %d\n", id_joueur, id_partie);

    /* Trouver la partie */
    Partie* partie = NULL;
    for (int i = 0; i < g->nb_parties; i++) {
        if (g->parties[i] && g->parties[i]->id_partie == id_partie) {
            partie = g->parties[i];
            break;
        }
    }

    if (!partie) {
        printf("❌ Partie %d introuvable\n", id_partie);
        return 0;
    }

    if (partie->nb_joueurs >= MAX_JOUEURS) {
        printf("❌ Partie %d complète (%d/%d joueurs)\n",
               id_partie, partie->nb_joueurs, MAX_JOUEURS);
        return 0;
    }

    /* Trouver le joueur connecté */
    JoueurConnecte* jc = trouver_joueur_connecte(g, id_joueur);
    if (!jc || !jc->joueur) {
        printf("❌ Joueur %d introuvable\n", id_joueur);
        return 0;
    }

    /* Ajouter le joueur à la partie */
    partie->joueurs[partie->nb_joueurs] = jc->joueur;
    partie->nb_joueurs++;
    partie->joueurs_actifs++;

    printf("✅ Joueur %s ajouté à la partie %d (%d/%d joueurs)\n",
           jc->joueur->nom, id_partie, partie->nb_joueurs, MAX_JOUEURS);

    return 1;
}

void demarrer_partie(Gestionnaire* g, Partie* p) {
    if (!p) return;

    if (p->nb_joueurs < 2) {
        printf("❌ Impossible de démarrer: besoin d'au moins 2 joueurs\n");
        return;
    }

    printf("▶️ Démarrage de la partie %d avec %d joueurs\n",
           p->id_partie, p->nb_joueurs);

    /* Initialiser la partie */
    p->manche_actuelle = 1;
    p->tour_actuel = 1;
    p->joueur_actif = 0;  /* Premier joueur */
    p->partie_terminee = 0;

    /* Mélanger et distribuer les cartes */
    printf("🔀 Mélange et distribution des cartes...\n");
    /* Note: Vous aurez besoin d'une fonction melanger_pioche() dans jeu.c */

    /* Notifier tous les joueurs */
    for (int i = 0; i < p->nb_joueurs; i++) {
        if (p->joueurs[i]) {
            printf("📢 Notification à %s: partie démarrée\n", p->joueurs[i]->nom);
        }
    }
}

void gerer_tour_partie(Gestionnaire* g, Partie* p) {
    if (!p || p->partie_terminee) return;

    printf("🔄 Tour %d de la partie %d\n", p->tour_actuel, p->id_partie);

    /* Si c'est le début d'une nouvelle manche */
    if (p->joueur_actif == 0) {
        printf("📖 Début de la manche %d\n", p->manche_actuelle);

        /* Distribuer les cartes pour la nouvelle manche */
        if (p->manche_actuelle == 1) {
            printf("🃏 Première manche: distribution initiale\n");
        }
    }

    /* Joueur actuel */
    Joueur* joueur = p->joueurs[p->joueur_actif];
    if (!joueur) {
        printf("⚠️ Joueur actuel introuvable, passage au suivant\n");
        p->joueur_actif = (p->joueur_actif + 1) % p->nb_joueurs;
        return;
    }

    printf("🎮 C'est au tour de %s\n", joueur->nom);

    /* Simuler un coup */
    if (joueur->nb_cartes_main > 0) {
        /* Dans la vraie implémentation, attendre le choix du joueur */
        printf("🤔 %s réfléchit...\n", joueur->nom);

        /* Simuler une carte jouée */
        Carte carte_jouee = {0, 0};
        if (joueur->nb_cartes_main > 0) {
            carte_jouee = joueur->main[0];
        }

        /* Enregistrer le coup */
        enregistrer_coup(g, p, joueur->id, carte_jouee, 0);
    }

    /* Passer au joueur suivant */
    p->joueur_actif = (p->joueur_actif + 1) % p->nb_joueurs;

    /* Si tous les joueurs ont joué, fin du tour */
    if (p->joueur_actif == 0) {
        p->tour_actuel++;

        /* Vérifier si la manche est terminée */
        int main_vide = 1;
        for (int i = 0; i < p->nb_joueurs; i++) {
            if (p->joueurs[i] && p->joueurs[i]->nb_cartes_main > 0) {
                main_vide = 0;
                break;
            }
        }

        if (main_vide) {
            printf("🏁 Fin de la manche %d\n", p->manche_actuelle);
            p->manche_actuelle++;

            /* Vérifier si la partie est terminée */
            if (p->manche_actuelle > 10) {  /* Exemple: 10 manches max */
                terminer_partie(g, p);
            }
        }
    }
}

void terminer_partie(Gestionnaire* g, Partie* p) {
    if (!p) return;

    printf("🏁 Fin de la partie %d\n", p->id_partie);

    /* Calculer le gagnant */
    int meilleur_score = 1000;  /* Score le plus bas gagne */
    int gagnant_id = -1;

    for (int i = 0; i < p->nb_joueurs; i++) {
        if (p->joueurs[i] && p->joueurs[i]->score < meilleur_score) {
            meilleur_score = p->joueurs[i]->score;
            gagnant_id = p->joueurs[i]->id;
        }
    }

    p->gagnant_id = gagnant_id;
    p->partie_terminee = 1;

    /* Afficher les résultats */
    printf("\n🏆 RÉSULTATS DE LA PARTIE %d\n", p->id_partie);
    printf("   Gagnant: Joueur %d avec %d têtes\n", gagnant_id, meilleur_score);
    printf("   Scores finaux:\n");

    for (int i = 0; i < p->nb_joueurs; i++) {
        if (p->joueurs[i]) {
            printf("   - %s: %d têtes\n",
                   p->joueurs[i]->nom, p->joueurs[i]->score);
        }
    }

    /* Enregistrer dans le log */
    char message[256];
    snprintf(message, sizeof(message),
             "Partie %d terminée. Gagnant: Joueur %d (%d têtes)",
             p->id_partie, gagnant_id, meilleur_score);
    ecrire_log(g->fichier_log, message);

    /* Libérer la mémoire de la partie */
    /* Note: Les joueurs sont gérés séparément */
    free(p);
}

void enregistrer_coup(Gestionnaire* g, Partie* p, int id_joueur, Carte carte, int tetes_prises) {
    char message[256];
    snprintf(message, sizeof(message),
             "Partie %d - Joueur %d joue carte %d (%d têtes), prend %d têtes",
             p->id_partie, id_joueur, carte.valeur, carte.tetes_de_boeuf, tetes_prises);
    printf("📝 %s\n", message);

    /* Écrire dans le log */
    ecrire_log(g->fichier_log, message);
}

void afficher_etat_global(const Gestionnaire* g) {
    printf("\n📊 ÉTAT GLOBAL DU GESTIONNAIRE\n");
    printf("   Parties actives: %d/%d\n", g->nb_parties, MAX_PARTIES);
    printf("   Joueurs connectés: %d\n", g->nb_joueurs_connectes);
    printf("   Clients TCP: %d\n", g->nb_clients_tcp);
    printf("   Mode réseau: %s\n", g->socket_serveur != -1 ? "Actif" : "Inactif");

    /* Détails des parties */
    for (int i = 0; i < g->nb_parties; i++) {
        if (g->parties[i]) {
            Partie* p = g->parties[i];
            printf("   - Partie %d: %d/%d joueurs, %s\n",
                   p->id_partie, p->nb_joueurs, MAX_JOUEURS,
                   p->partie_terminee ? "Terminée" : "En cours");
        }
    }
}

void generer_statistiques(Gestionnaire* g) {
    printf("\n📈 STATISTIQUES\n");
    printf("   - Parties jouées: %d\n", g->nb_parties);
    printf("   - Joueurs connectés: %d\n", g->nb_joueurs_connectes);
    printf("   - Clients TCP actifs: %d\n", g->nb_clients_tcp);
    printf("   - Logs dans: %s\n", g->fichier_log);
}

void accepter_joueurs_reseau(Gestionnaire* g) {
    printf("🌐 Acceptation des joueurs réseau\n");
    /* Cette fonction est maintenant intégrée dans boucle_principale */
}

void gerer_message_reseau(Gestionnaire* g, int socket_client) {
    printf("📨 Gestion message réseau du socket %d\n", socket_client);
}

JoueurConnecte* ajouter_joueur(Gestionnaire* g, Joueur* joueur, int est_reseau) {
    printf("➕ Ajout du joueur %s (ID: %d)\n", joueur->nom, joueur->id);

    JoueurConnecte* jc = malloc(sizeof(JoueurConnecte));
    if (jc) {
        jc->id = joueur->id;
        jc->joueur = joueur;
        jc->socket_fd = est_reseau;
        jc->actif = 1;
        jc->fd_in = -1;
        jc->fd_out = -1;
        jc->pid = 0;

        /* Ajouter à la liste */
        if (g->nb_joueurs_connectes < MAX_JOUEURS_PAR_PARTIE * MAX_PARTIES) {
            g->joueurs_connectes[g->nb_joueurs_connectes++] = jc;
            printf("✅ Joueur ajouté (total: %d)\n", g->nb_joueurs_connectes);
        } else {
            printf("❌ Trop de joueurs connectés\n");
            free(jc);
            return NULL;
        }
    }
    return jc;
}

JoueurConnecte* trouver_joueur_connecte(Gestionnaire* g, int id_joueur) {
    for (int i = 0; i < g->nb_joueurs_connectes; i++) {
        if (g->joueurs_connectes[i] && g->joueurs_connectes[i]->id == id_joueur) {
            return g->joueurs_connectes[i];
        }
    }
    return NULL;
}

void deconnecter_joueur(Gestionnaire* g, int id_joueur) {
    for (int i = 0; i < g->nb_joueurs_connectes; i++) {
        if (g->joueurs_connectes[i] && g->joueurs_connectes[i]->id == id_joueur) {
            printf("➖ Déconnexion du joueur %d\n", id_joueur);
            free(g->joueurs_connectes[i]);
            g->joueurs_connectes[i] = NULL;
        }
    }
}

/**
 * Démarre une partie réseau
 */
void demarrer_partie_reseau(Gestionnaire* g) {
    printf("🎮 Démarrage partie réseau avec %d joueurs\n", g->nb_clients_tcp);

    /* Créer une nouvelle partie */
    Partie* partie = creer_partie(g->nb_parties + 1);
    if (!partie) {
        printf("❌ Erreur création partie\n");
        return;
    }

    /* Ajouter la partie à la liste */
    g->parties[g->nb_parties] = partie;
    g->nb_parties++;

    /* Ajouter tous les joueurs connectés */
    int joueurs_ajoutes = 0;
    for (int i = 0; i < g->nb_clients_tcp; i++) {
        if (g->clients_tcp[i].est_actif) {
            /* Trouver le joueur correspondant */
            for (int j = 0; j < g->nb_joueurs_connectes; j++) {
                if (g->joueurs_connectes[j] &&
                    g->joueurs_connectes[j]->socket_fd == g->clients_tcp[i].socket) {

                    /* Ajouter le joueur à la partie */
                    partie->joueurs[joueurs_ajoutes] = g->joueurs_connectes[j]->joueur;
                    joueurs_ajoutes++;
                    partie->nb_joueurs++;

                    /* Informer le joueur */
                    char message[200];
                    snprintf(message, sizeof(message),
                            "\n=== PARTIE %d DÉMARRÉE ===\n"
                            "Joueurs: %d\n"
                            "Distribution des cartes...\n\n",
                            partie->id_partie, partie->nb_joueurs);

                    send(g->clients_tcp[i].socket, message, strlen(message), 0);
                    break;
                }
            }
        }
    }

    /* Initialiser la partie */
    distribuer_cartes_partie(partie);
    initialiser_rangees_partie(partie);
    partie->joueur_actif = 0;  // Premier joueur commence

    /* Envoyer l'état initial à tous */
    for (int i = 0; i < g->nb_clients_tcp; i++) {
        if (g->clients_tcp[i].est_actif) {
            /* Envoyer l'état */
            char etat[1024];
            snprintf(etat, sizeof(etat),
                    "\n=== ÉTAT INITIAL ===\n"
                    "Manche: %d\n\n"
                    "Vos cartes (tapez 'cartes' pour voir):\n",
                    partie->manche_actuelle);

            /* Indiquer qui commence */
            char tour[100];
            snprintf(tour, sizeof(tour),
                    "\n🎮 TOUR DU JOUEUR %d\n"
                    "Tapez 'cartes' pour voir vos cartes\n"
                    "Puis 'jouer X' pour jouer\n\n",
                    partie->joueur_actif + 1);

            strcat(etat, tour);
            send(g->clients_tcp[i].socket, etat, strlen(etat), 0);
        }
    }
}

/**
 * Gère une commande réseau
 */
void gerer_commande_reseau(Gestionnaire* g, int socket, const char* commande) {
    /* Trouver le joueur */
    JoueurConnecte* jc = NULL;
    for (int i = 0; i < g->nb_joueurs_connectes; i++) {
        if (g->joueurs_connectes[i] && g->joueurs_connectes[i]->socket_fd == socket) {
            jc = g->joueurs_connectes[i];
            break;
        }
    }

    if (!jc || !jc->joueur) {
        send(socket, "❌ Erreur: joueur non trouvé\n", 28, 0);
        return;
    }

    /* Trouver la partie du joueur */
    Partie* partie = NULL;
    for (int i = 0; i < g->nb_parties; i++) {
        if (g->parties[i]) {
            for (int j = 0; j < g->parties[i]->nb_joueurs; j++) {
                if (g->parties[i]->joueurs[j] == jc->joueur) {
                    partie = g->parties[i];
                    break;
                }
            }
        }
        if (partie) break;
    }

    /* Traiter la commande */
    if (strcmp(commande, "cartes") == 0) {
        /* Afficher les cartes */
        if (jc->joueur->nb_cartes_main == 0) {
            send(socket, "❌ Vous n'avez plus de cartes\n", 30, 0);
            return;
        }

        char reponse[1024];
        strcpy(reponse, "\n=== VOS CARTES ===\n");

        for (int i = 0; i < jc->joueur->nb_cartes_main; i++) {
            char ligne[100];
            snprintf(ligne, sizeof(ligne), "%d. Carte %d (%d têtes)\n",
                    i + 1,
                    jc->joueur->main[i].valeur,
                    jc->joueur->main[i].tetes_de_boeuf);
            strcat(reponse, ligne);
        }

        strcat(reponse, "\nPour jouer: 'jouer X' (ex: jouer 1)\n");
        send(socket, reponse, strlen(reponse), 0);

    } else if (strncmp(commande, "jouer ", 6) == 0) {
        /* Jouer une carte */
        if (!partie) {
            send(socket, "❌ Vous n'êtes dans aucune partie\n", 34, 0);
            return;
        }

        /* Vérifier si c'est le tour du joueur */
        int index_joueur = -1;
        for (int i = 0; i < partie->nb_joueurs; i++) {
            if (partie->joueurs[i] == jc->joueur) {
                index_joueur = i;
                break;
            }
        }

        if (index_joueur != partie->joueur_actif) {
            char msg[100];
            snprintf(msg, sizeof(msg),
                    "⏳ Ce n'est pas votre tour! Tour du joueur %d\n",
                    partie->joueur_actif + 1);
            send(socket, msg, strlen(msg), 0);
            return;
        }

        /* Récupérer le numéro de carte */
        int carte_num = atoi(commande + 6);
        if (carte_num < 1 || carte_num > jc->joueur->nb_cartes_main) {
            send(socket, "❌ Numéro de carte invalide\n", 28, 0);
            return;
        }

        /* Jouer la carte */
        Carte carte_jouee = retirer_carte_main(jc->joueur, carte_num - 1);
        int tetes = jouer_carte_partie(partie, jc->joueur->id, carte_jouee);

        /* Informer le joueur */
        char resultat[200];
        snprintf(resultat, sizeof(resultat),
                "\n✅ Vous avez joué la carte %d\n"
                "Têtes prises: %d\n"
                "Votre score: %d têtes\n\n",
                carte_jouee.valeur, tetes, jc->joueur->score);

        send(socket, resultat, strlen(resultat), 0);

        /* Passer au joueur suivant */
        partie->joueur_actif = (partie->joueur_actif + 1) % partie->nb_joueurs;
        partie->tour_actuel++;

        /* Envoyer le nouvel état à tous */
        for (int i = 0; i < g->nb_clients_tcp; i++) {
            if (g->clients_tcp[i].est_actif && g->clients_tcp[i].socket != -1) {
                /* Qui doit jouer maintenant */
                char tour[100];
                snprintf(tour, sizeof(tour),
                        "\n🎮 TOUR DU JOUEUR %d\n",
                        partie->joueur_actif + 1);

                send(g->clients_tcp[i].socket, tour, strlen(tour), 0);

                /* Envoyer l'état */
                char etat[1024];
                snprintf(etat, sizeof(etat),
                        "\n=== ÉTAT PARTIE ===\n"
                        "Manche: %d, Tour: %d\n\n"
                        "Scores:\n",
                        partie->manche_actuelle, partie->tour_actuel);

                for (int j = 0; j < partie->nb_joueurs; j++) {
                    char score[50];
                    snprintf(score, sizeof(score), "%s: %d têtes\n",
                            partie->joueurs[j]->nom, partie->joueurs[j]->score);
                    strcat(etat, score);
                }

                send(g->clients_tcp[i].socket, etat, strlen(etat), 0);
            }
        }

    } else if (strcmp(commande, "etat") == 0) {
        /* Afficher l'état */
        if (partie) {
            char etat[1024];
            snprintf(etat, sizeof(etat),
                    "\n=== ÉTAT PARTIE ===\n"
                    "Manche: %d, Tour: %d\n"
                    "Tour du joueur: %d\n\n"
                    "Scores:\n",
                    partie->manche_actuelle, partie->tour_actuel,
                    partie->joueur_actif + 1);

            for (int i = 0; i < partie->nb_joueurs; i++) {
                char score[50];
                snprintf(score, sizeof(score), "%s: %d têtes\n",
                        partie->joueurs[i]->nom, partie->joueurs[i]->score);
                strcat(etat, score);
            }

            send(socket, etat, strlen(etat), 0);
        } else {
            send(socket, "❌ Aucune partie en cours\n", 26, 0);
        }

    } else if (strcmp(commande, "quit") == 0 || strcmp(commande, "exit") == 0) {
        /* Déconnexion */
        char* msg = "\n👋 À bientôt!\n";
        send(socket, msg, strlen(msg), 0);

    } else {
        /* Commande inconnue */
        char* help = "\nCommandes:\n"
                     "  cartes   - Voir vos cartes\n"
                     "  jouer X  - Jouer la carte X\n"
                     "  etat     - Voir l'état\n"
                     "  quit     - Quitter\n\n";
        send(socket, help, strlen(help), 0);
    }
}

/* Point d'entrée pour tester directement */
#ifdef TEST_GESTIONNAIRE
int main(int argc, char* argv[]) {
    printf("🧪 TEST DU GESTIONNAIRE\n");

    int port = 0;
    if (argc > 1 && strcmp(argv[1], "--reseau") == 0) {
        port = (argc > 2) ? atoi(argv[2]) : 8080;
        printf("Mode: Réseau (port: %d)\n", port);
    } else {
        printf("Mode: Local\n");
    }

    lancer_gestionnaire(port);
    return 0;
}
#endif
