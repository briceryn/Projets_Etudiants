/*
 * fichier : main.c
 * but : Point d'entrée principal du jeu 6 qui prend
 *       - Lancement du serveur
 *       - Lancement des clients
 *       - Mode test et démo réels
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include "jeu.h"
#include "joueurs.h"
#include "communication.h"
#include "gestionnaire.h"

/* ========== CONSTANTES ========== */
#define VERSION "1.0.0"
#define AUTEUR "Équipe 6 qui prend"
#define ANNEE "2024"
#define NB_ROBOTS_DEMO 2
#define NB_HUMAINS_DEMO 2

/* ========== FONCTIONS D'AFFICHAGE ========== */

/**
 * Affiche le logo du jeu
 */
void afficher_logo(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║                                                      ║\n");
    printf("║                      6                               ║\n");
    printf("║                   6 QUI PREND                        ║\n");
    printf("║                                                      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * Affiche l'aide
 */
void afficher_aide(const char* nom_programme) {
    printf("Usage: %s [OPTIONS]\n\n", nom_programme);
    printf("Options:\n");
    printf("  --serveur [PORT]    Lance le serveur (port par défaut: 8080)\n");
    printf("  --client IP PORT    Lance un client et se connecte au serveur\n");
    printf("  --client-local      Lance un client local avec interface FIFO\n");
    printf("  --test              Lance tous les tests\n");
    printf("  --test-jeu          Teste la logique du jeu\n");
    printf("  --test-joueurs      Teste les joueurs et robots\n");
    printf("  --test-comm         Teste les communications\n");
    printf("  --demo              Lance une partie complète avec robots\n");
    printf("  --demo-2v2          Lance une partie 2 humains vs 2 robots\n");
    printf("  --help              Affiche cette aide\n");
    printf("  --version           Affiche la version\n");
    printf("\nExemples:\n");
    printf("  %s --serveur 8080\n", nom_programme);
    printf("  %s --client 127.0.0.1 8080\n", nom_programme);
    printf("  %s --demo-2v2\n", nom_programme);
    printf("  %s --test\n", nom_programme);
}

/**
 * Affiche la version
 */
void afficher_version(void) {
    printf("6 qui prend - Version %s\n", VERSION);
    printf("Développé par %s - %s\n", AUTEUR, ANNEE);
    printf("Un jeu de cartes stratégique pour 2 à 10 joueurs\n");
}

/* ========== FONCTIONS DE MODE CLIENT ========== */

/**
 * Mode client TCP
 */
/**
 * Mode client TCP - Version corrigée
 */
void lancer_client_tcp(const char* adresse, int port) {
    printf("🎮 Lancement du client...\n");

    /* Résoudre localhost si nécessaire */
    const char* adresse_ip = adresse;
    if (strcmp(adresse, "localhost") == 0) {
        adresse_ip = "127.0.0.1";
    }

    printf("Connexion à %s:%d\n", adresse_ip, port);

    int socket_client = connecter_client_tcp(adresse_ip, port);
    if (socket_client == -1) {
        printf("❌ Impossible de se connecter au serveur\n");
        return;
    }

    printf("✅ Connecté au serveur\n");

    /* Recevoir le message de bienvenue - VERSION SIMPLE */
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    /* Désactiver le timeout pour la réception */
    struct timeval tv;
    tv.tv_sec = 2;  /* 2 secondes de timeout */
    tv.tv_usec = 0;
    setsockopt(socket_client, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    /* Lire directement format taille+données */
    char taille_buffer[4];
    int bytes = recv(socket_client, taille_buffer, 4, 0);
    if (bytes == 4) {
        uint32_t taille = ntohl(*(uint32_t*)taille_buffer);
        bytes = recv(socket_client, buffer, taille, 0);
        buffer[bytes] = '\0';
    }
    /* Restaurer le timeout par défaut */
    tv.tv_sec = 0;
    setsockopt(socket_client, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("📨 Serveur: %s", buffer);
    } else if (bytes == 0) {
        printf("🔌 Serveur déconnecté\n");
        close(socket_client);
        return;
    } else {
        printf("⚠️  Pas de message de bienvenue\n");
    }

    /* Boucle principale du client */
    printf("\n=== MODE CLIENT ===\n");
    printf("Tapez vos messages (ou 'quit' pour quitter):\n\n");

    fd_set readfds;
    int en_jeu = 1;

    while (en_jeu) {
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(socket_client, &readfds);

    struct timeval timeout = {1, 0};
    int activite = select(socket_client + 1, &readfds, NULL, NULL, &timeout);

    if (activite > 0) {
        /* Message de l'utilisateur */
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            printf("> ");
            fflush(stdout);  // <-- IMPORTANT

            char message[256];
            if (fgets(message, sizeof(message), stdin)) {
                message[strcspn(message, "\n")] = 0;

                if (strcmp(message, "quit") == 0 || strcmp(message, "exit") == 0) {
                    en_jeu = 0;
                    printf("Déconnexion...\n");
                    send(socket_client, "quit", 4, 0);
                } else {
                    send(socket_client, message, strlen(message), 0);
                }
            }
        }

        /* Message du serveur */
        if (FD_ISSET(socket_client, &readfds)) {
            char simple_buffer[1024];
            memset(simple_buffer, 0, sizeof(simple_buffer));
            int bytes = recv(socket_client, simple_buffer, sizeof(simple_buffer) - 1, 0);

            if (bytes > 0) {
                simple_buffer[bytes] = '\0';
                printf("\n📨 Serveur: %s\n", simple_buffer);
            } else if (bytes == 0) {
                printf("\n🔌 Serveur déconnecté\n");
                en_jeu = 0;
            }
        }
    }
}

    close(socket_client);
    printf("Client arrêté\n");
}

/**
 * Mode client local (FIFO) - Interface réelle
 */
void lancer_client_local(void) {
    printf("🎮 Lancement du client local...\n");
    printf("Mode local (FIFOs)\n");

    /* Créer un vrai joueur */
    Joueur* joueur = creer_joueur(1, "JoueurLocal", 0);
    if (!joueur) {
        printf("❌ Impossible de créer le joueur\n");
        return;
    }

    printf("Joueur créé: %s (ID: %d)\n", joueur->nom, joueur->id);

    /* Afficher l'accueil */
    afficher_accueil_joueur(joueur);

    /* Créer une vraie partie */
    Partie* partie = creer_partie(1);
    if (!partie) {
        printf("❌ Impossible de créer la partie\n");
        detruire_joueur(joueur);
        return;
    }

    /* Ajouter le joueur humain */
    if (!ajouter_joueur_partie(partie, joueur)) {
        printf("❌ Impossible d'ajouter le joueur à la partie\n");
        detruire_partie(partie);
        detruire_joueur(joueur);
        return;
    }

    /* Ajouter un robot */
    Joueur* robot = creer_joueur(2, "Robot1", 1);
    if (robot) {
        ajouter_joueur_partie(partie, robot);
        printf("✅ Robot ajouté: %s\n", robot->nom);
    }

    /* Distribuer les cartes */
    distribuer_cartes_partie(partie);
    initialiser_rangees_partie(partie);

    printf("\n=== PARTIE DÉMARRÉE ===\n");
    printf("Vous avez %d cartes en main\n", joueur->nb_cartes_main);

    /* Jouer la partie */
    int tour = 1;
    while (!partie->partie_terminee) {
        printf("\n--- TOUR %d ---\n", tour++);

        /* Afficher l'état */
        afficher_rangees_joueur(partie->rangees, NB_RANGEES);
        afficher_scores_joueurs(partie->joueurs, partie->nb_joueurs);

        /* Pour chaque joueur */
        for (int i = 0; i < partie->nb_joueurs && !partie->partie_terminee; i++) {
            Joueur* j = partie->joueurs[i];

            if (j->nb_cartes_main == 0) {
                continue;
            }

            printf("\n🎮 Tour de %s:\n", j->nom);

            Carte carte_jouee;
            if (j->est_robot) {
                /* Robot joue */
                carte_jouee = strategie_petite_carte(j, partie->rangees, NB_RANGEES);
                retirer_carte_robot(j, carte_jouee);
            } else {
                /* Humain joue */
                printf("Votre main: ");
                afficher_cartes(j->main, j->nb_cartes_main);

                int choix = demander_choix_carte_humain(j);
                if (choix >= 0) {
                    carte_jouee = retirer_carte_main(j, choix);
                } else {
                    printf("❌ Choix invalide, carte par défaut\n");
                    carte_jouee = retirer_carte_main(j, 0);
                }
            }

            /* Jouer la carte */
            int tetes = jouer_carte_partie(partie, j->id, carte_jouee);
            printf("%s joue carte %d", j->nom, carte_jouee.valeur);
            if (tetes > 0) {
                printf(" et ramasse %d tête%s\n", tetes, tetes > 1 ? "s" : "");
            } else {
                printf("\n");
            }

            /* Vérifier si la partie est terminée */
            verifier_partie_terminee(partie);

            /* Pause pour lisibilité */
            sleep(1);
        }
    }

    /* Afficher les résultats */
    printf("\n🎯 PARTIE TERMINÉE !\n");
    printf("Gagnant: Joueur %d\n", partie->gagnant_id);
    printf("\nScores finaux:\n");
    for (int i = 0; i < partie->nb_joueurs; i++) {
        printf("  %s: %d têtes\n", partie->joueurs[i]->nom, partie->joueurs[i]->score);
    }

    /* Nettoyer */
    detruire_partie(partie);  /* Détruit aussi les joueurs */
    printf("\nClient local arrêté\n");
}

/* ========== FONCTION DE DÉMONSTRATION RÉELLE ========== */

/**
 * Lance une démonstration avec des vrais joueurs et robots
 */
void lancer_demo_reelle(void) {
    printf("\n🎮 DÉMONSTRATION 6 QUI PREND (VRAIE PARTIE) 🎮\n\n");

    /* Initialiser l'aléatoire */
    srand(time(NULL) ^ getpid());

    /* Créer une partie */
    printf("1. Création d'une partie...\n");
    Partie* partie = creer_partie(1);
    if (!partie) {
        printf("❌ Impossible de créer la partie\n");
        return;
    }

    /* Créer des vrais joueurs */
    printf("2. Création des joueurs...\n");
    Joueur* joueurs[NB_HUMAINS_DEMO + NB_ROBOTS_DEMO];
    const char* noms[] = {"Alice", "Bob", "Robot1", "Robot2"};

    for (int i = 0; i < NB_HUMAINS_DEMO + NB_ROBOTS_DEMO; i++) {
        joueurs[i] = creer_joueur(i + 1, noms[i], i >= NB_HUMAINS_DEMO);
        if (joueurs[i]) {
            ajouter_joueur_partie(partie, joueurs[i]);
            printf("   ✅ %s (%s)\n", noms[i], i >= NB_HUMAINS_DEMO ? "robot" : "humain");
        }
    }

    /* Distribuer les cartes */
    printf("\n3. Distribution des cartes...\n");
    distribuer_cartes_partie(partie);

    /* Initialiser les rangées */
    printf("4. Initialisation des rangées...\n");
    initialiser_rangees_partie(partie);

    /* Afficher l'état initial */
    printf("\n5. État initial de la partie:\n");
    afficher_etat_partie(partie);

    /* Jouer la partie complète */
    printf("6. Début de la partie !\n\n");
    int manche = 1;

    while (!partie->partie_terminee) {
        printf("\n=== MANCHE %d ===\n", manche++);

        /* Réinitialiser pour nouvelle manche si nécessaire */
        if (manche > 1) {
            distribuer_cartes_partie(partie);
        }

        /* 10 tours par manche */
        for (int tour = 1; tour <= 10 && !partie->partie_terminee; tour++) {
            printf("\n--- TOUR %d ---\n", tour);

            /* Chaque joueur joue une carte */
            for (int i = 0; i < partie->nb_joueurs && !partie->partie_terminee; i++) {
                Joueur* joueur = partie->joueurs[i];

                if (joueur->nb_cartes_main == 0) {
                    continue;
                }

                /* Le joueur choisit une carte */
                Carte carte_jouee;

                if (joueur->est_robot) {
                    /* Robot : choisir une stratégie selon le niveau */
                    int niveau = (i - NB_HUMAINS_DEMO) % 3;  /* 3 niveaux différents */
                    carte_jouee = choisir_carte_robot(joueur, partie->rangees, NB_RANGEES, niveau);
                    retirer_carte_robot(joueur, carte_jouee);
                } else {
                    /* Humain : stratégie simple pour la démo */
                    printf("  %s choisit sa carte...\n", joueur->nom);

                    /* Trouver la carte avec le moins de têtes */
                    int index_meilleur = 0;
                    for (int j = 1; j < joueur->nb_cartes_main; j++) {
                        if (joueur->main[j].tetes_de_boeuf < joueur->main[index_meilleur].tetes_de_boeuf) {
                            index_meilleur = j;
                        }
                    }
                    carte_jouee = retirer_carte_main(joueur, index_meilleur);
                }

                /* Jouer la carte */
                int tetes = jouer_carte_partie(partie, joueur->id, carte_jouee);

                printf("  %s joue carte %d", joueur->nom, carte_jouee.valeur);
                if (tetes > 0) {
                    printf(" et ramasse %d tête%s\n", tetes, tetes > 1 ? "s" : "");
                } else {
                    printf("\n");
                }

                /* Vérifier si la partie est terminée */
                verifier_partie_terminee(partie);

                /* Pause pour la démo */
                usleep(500000);  /* 0.5 seconde */
            }

            /* Afficher l'état après le tour */
            if (tour % 3 == 0) {  /* Tous les 3 tours */
                printf("\nÉtat après tour %d:\n", tour);
                afficher_etat_partie(partie);
            }
        }

        /* Vérifier si on dépasse le nombre maximum de manches */
        if (manche > 10) {  /* 10 manches maximum */
            printf("\n⏱️  Nombre maximum de manches atteint\n");
            break;
        }
    }

    /* Afficher les résultats */
    printf("\n🎯 RÉSULTATS FINAUX 🎯\n");

    if (partie->partie_terminee) {
        printf("Partie terminée !\n");
        printf("🏆 Gagnant: ");
        for (int i = 0; i < partie->nb_joueurs; i++) {
            if (partie->joueurs[i]->id == partie->gagnant_id) {
                printf("%s", partie->joueurs[i]->nom);
                break;
            }
        }
        printf(" (Joueur %d)\n", partie->gagnant_id);
    } else {
        printf("Partie arrêtée après %d manches\n", manche - 1);
    }

    printf("\nScores finaux:\n");
    for (int i = 0; i < partie->nb_joueurs; i++) {
        printf("  %-10s: %3d têtes\n",
               partie->joueurs[i]->nom,
               partie->joueurs[i]->score);
    }

    /* Nettoyer */
    printf("\n7. Nettoyage...\n");
    detruire_partie(partie);  /* Libère aussi les joueurs */

    printf("\n✅ Démonstration terminée !\n");
    printf("   Pour jouer une vraie partie réseau, utilisez:\n");
    printf("   ./6quiprend --serveur 8080\n");
    printf("   ./6quiprend --client 127.0.0.1 8080\n");
}

/* ========== FONCTIONS DE TEST ========== */

/**
 * Lance tous les tests
 */
void lancer_tous_tests(void) {
    printf("🧪 LANCEMENT DE TOUS LES TESTS 🧪\n\n");

    /* Test de la logique du jeu */
    printf("=== TEST LOGIQUE JEU ===\n");
    tester_logique_jeu();

    /* Test des joueurs */
    printf("\n=== TEST JOUEURS ===\n");
    tester_interface_joueur();
    tester_strategies_robot();

    /* Test des communications */
    printf("\n=== TEST COMMUNICATIONS ===\n");
    tester_communications();

    printf("\n✅ Tous les tests sont terminés !\n");
}

/* ========== FONCTION PRINCIPALE ========== */

int main(int argc, char* argv[]) {
    /* Afficher le logo */
    afficher_logo();

    /* Vérifier les arguments */
    if (argc < 2) {
        printf("❌ Argument manquant\n");
        afficher_aide(argv[0]);
        return 1;
    }

    /* Traiter les options */
    if (strcmp(argv[1], "--serveur") == 0) {
        int port = 8080;  /* Port par défaut */

        if (argc > 2) {
            port = atoi(argv[2]);
            if (port <= 0 || port > 65535) {
                printf("❌ Port invalide: %s\n", argv[2]);
                printf("Le port doit être entre 1 et 65535\n");
                return 1;
            }
        }

        printf("🚀 Lancement du serveur sur le port %d...\n", port);
        lancer_gestionnaire(port);

    } else if (strcmp(argv[1], "--client") == 0) {
        if (argc < 4) {
            printf("❌ Arguments manquants pour --client\n");
            printf("Usage: %s --client IP PORT\n", argv[0]);
            return 1;
        }

        const char* adresse = argv[2];
        int port = atoi(argv[3]);

        if (port <= 0 || port > 65535) {
            printf("❌ Port invalide: %s\n", argv[3]);
            return 1;
        }

        lancer_client_tcp(adresse, port);

    } else if (strcmp(argv[1], "--client-local") == 0) {
        lancer_client_local();

    } else if (strcmp(argv[1], "--demo") == 0 || strcmp(argv[1], "--demo-2v2") == 0) {
        lancer_demo_reelle();

    } else if (strcmp(argv[1], "--test") == 0) {
        lancer_tous_tests();

    } else if (strcmp(argv[1], "--test-jeu") == 0) {
        printf("🧪 Test de la logique du jeu\n");
        tester_logique_jeu();

    } else if (strcmp(argv[1], "--test-joueurs") == 0) {
        printf("🧪 Test des joueurs\n");
        tester_interface_joueur();
        tester_strategies_robot();

    } else if (strcmp(argv[1], "--test-comm") == 0) {
        printf("🧪 Test des communications\n");
        tester_communications();

    } else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        afficher_aide(argv[0]);

    } else if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        afficher_version();

    } else {
        printf("❌ Option non reconnue: %s\n", argv[1]);
        printf("Utilisez --help pour voir les options disponibles\n");
        return 1;
    }

    return 0;
}
