/* ====================================================
 * fichier : joueurs.c
 * but : Gestion des joueurs humains et robots
 *       - Interface utilisateur
 *       - Stratégies des robots
 *       - Communication avec le gestionnaire
 * ==================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

/* Inclure les structures du jeu */
#include "jeu.h"
#include "joueurs.h"

/* ========== CONSTANTES ========== */
#define TAILLE_BUFFER 1024
#define TIMEOUT_COUP 30  /* 30 secondes pour jouer */
#define NOM_FIFO_BASE "/tmp/6quiprend_joueur_"

/* ========== STRUCTURES INTERNES ========== */

/* Variable globale pour le timeout */
volatile int timeout_atteint = 0;

/* ========== FONCTIONS D'INTERFACE HUMAINE ========== */

/**
 * Affiche l'interface d'accueil pour un joueur humain
 */
void afficher_accueil_joueur(Joueur* j) {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║         JOUEUR : %-20s  ║\n", j->nom);
    printf("║         ID    : %-20d  ║\n", j->id);
    printf("║         SCORE : %-20d  ║\n", j->score);
    printf("╚════════════════════════════════════════╝\n");
}

/**
 * Demande à un joueur humain de choisir une carte
 * Retourne l'index de la carte choisie (-1 si annulation)
 */
int demander_choix_carte_humain(Joueur* j) {
    if (j->nb_cartes_main == 0) {
        printf("Vous n'avez plus de cartes en main!\n");
        return -1;
    }

    printf("\n=== VOTRE MAIN ===\n");
    for (int i = 0; i < j->nb_cartes_main; i++) {
        printf("%2d. ", i + 1);
        afficher_carte(j->main[i]);
        printf("\n");
    }

    int choix = -1;
    while (choix < 1 || choix > j->nb_cartes_main) {
        printf("\nChoisissez une carte (1-%d) : ", j->nb_cartes_main);

        /* Lire l'entrée utilisateur */
        char input[10];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Erreur de lecture.\n");
            continue;
        }

        /* Vérifier si c'est un nombre */
        char* endptr;
        choix = strtol(input, &endptr, 10);

        if (endptr == input) {
            printf("Veuillez entrer un nombre.\n");
            choix = -1;
        } else if (choix < 1 || choix > j->nb_cartes_main) {
            printf("Choix invalide. Veuillez choisir entre 1 et %d.\n",
                   j->nb_cartes_main);
        }
    }

    return choix - 1;  /* Convertir en index 0-based */
}

/**
 * Affiche les rangées pour le joueur humain
 */
void afficher_rangees_joueur(const Rangee rangees[], int nb_rangees) {
    printf("\n=== RANGÉES SUR LA TABLE ===\n");
    for (int i = 0; i < nb_rangees; i++) {
        printf("Rangée %d: ", i + 1);
        afficher_rangee(&rangees[i]);
    }
}

/**
 * Affiche les scores de tous les joueurs
 */
void afficher_scores_joueurs(Joueur* joueurs[], int nb_joueurs) {
    printf("\n=== SCORES ===\n");
    for (int i = 0; i < nb_joueurs; i++) {
        if (joueurs[i]->est_actif) {
            printf("%-15s : %3d tête%s\n",
                   joueurs[i]->nom,
                   joueurs[i]->score,
                   joueurs[i]->score > 1 ? "s" : "");
        }
    }
}

/* ========== STRATÉGIES DES ROBOTS ========== */

/**
 * Stratégie simple : joue la plus petite carte
 */
Carte strategie_petite_carte(Joueur* robot, const Rangee rangees[], int nb_rangees) {
    if (robot->nb_cartes_main == 0) {
        Carte vide = {0, 0};
        return vide;
    }

    /* Trouver la plus petite carte */
    int index_min = 0;
    for (int i = 1; i < robot->nb_cartes_main; i++) {
        if (robot->main[i].valeur < robot->main[index_min].valeur) {
            index_min = i;
        }
    }

    Carte choix = robot->main[index_min];
    printf("Robot %s choisit la plus petite carte: ", robot->nom);
    afficher_carte(choix);
    printf("\n");

    return choix;
}

/**
 * Stratégie moyenne : essaie de ne pas prendre de rangée
 */
Carte strategie_eviter_rangees(Joueur* robot, const Rangee rangees[], int nb_rangees) {
    if (robot->nb_cartes_main == 0) {
        Carte vide = {0, 0};
        return vide;
    }

    /* Analyser les rangées */
    int valeurs_min_rangees[4];
    for (int i = 0; i < nb_rangees; i++) {
        if (rangees[i].nb_cartes > 0) {
            valeurs_min_rangees[i] = rangees[i].cartes[rangees[i].nb_cartes - 1].valeur;
        } else {
            valeurs_min_rangees[i] = 0;
        }
    }

    /* Trouver la carte qui risque le moins de faire ramasser une rangée */
    int meilleur_index = 0;
    int meilleur_risque = 1000;  /* Plus bas = mieux */

    for (int i = 0; i < robot->nb_cartes_main; i++) {
        int carte_val = robot->main[i].valeur;
        int risque = 0;

        /* Vérifier si la carte est trop petite pour toutes les rangées */
        int trop_petite = 1;
        for (int r = 0; r < nb_rangees; r++) {
            if (carte_val > valeurs_min_rangees[r]) {
                trop_petite = 0;

                /* Calculer le risque (distance à la 6ème carte) */
                int cartes_restantes = 6 - rangees[r].nb_cartes;
                if (cartes_restantes <= 1) {
                    risque += 10;  /* Risque élevé de ramasser */
                }
            }
        }

        if (trop_petite) {
            risque += 5;  /* Devra choisir une rangée à ramasser */
        }

        /* Préférer les cartes avec peu de têtes */
        risque += robot->main[i].tetes_de_boeuf;

        if (risque < meilleur_risque) {
            meilleur_risque = risque;
            meilleur_index = i;
        }
    }

    Carte choix = robot->main[meilleur_index];
    printf("Robot %s joue (risque calculé: %d): ", robot->nom, meilleur_risque);
    afficher_carte(choix);
    printf("\n");

    return choix;
}

/**
 * Stratégie agressive : essaie de faire ramasser des rangées aux autres
 */
Carte strategie_agressive(Joueur* robot, const Rangee rangees[], int nb_rangees) {
    if (robot->nb_cartes_main == 0) {
        Carte vide = {0, 0};
        return vide;
    }

    /* Trouver les rangées presque pleines (5 cartes) */
    int rangees_presque_pleines[4] = {0};
    for (int i = 0; i < nb_rangees; i++) {
        if (rangees[i].nb_cartes == 5) {
            rangees_presque_pleines[i] = 1;
        }
    }

    /* Chercher une carte qui peut être placée sur une rangée presque pleine */
    for (int i = 0; i < robot->nb_cartes_main; i++) {
        Carte carte = robot->main[i];

        for (int r = 0; r < nb_rangees; r++) {
            if (rangees[r].nb_cartes > 0 &&
                carte.valeur > rangees[r].cartes[rangees[r].nb_cartes - 1].valeur &&
                rangees_presque_pleines[r]) {

                printf("Robot %s joue agressivement sur rangée %d presque pleine: ",
                       robot->nom, r + 1);
                afficher_carte(carte);
                printf("\n");

                return carte;
            }
        }
    }

    /* Sinon, jouer la plus grande carte */
    int index_max = 0;
    for (int i = 1; i < robot->nb_cartes_main; i++) {
        if (robot->main[i].valeur > robot->main[index_max].valeur) {
            index_max = i;
        }
    }

    Carte choix = robot->main[index_max];
    printf("Robot %s joue la plus grande carte: ", robot->nom);
    afficher_carte(choix);
    printf("\n");

    return choix;
}

/**
 * Choisit une stratégie pour un robot selon son niveau
 */
Carte choisir_carte_robot(Joueur* robot, const Rangee rangees[], int nb_rangees, int niveau) {
    if (robot->nb_cartes_main == 0) {
        Carte vide = {0, 0};
        return vide;
    }

    switch (niveau) {
        case 0:  /* Débutant */
            return strategie_petite_carte(robot, rangees, nb_rangees);

        case 1:  /* Intermédiaire */
            return strategie_eviter_rangees(robot, rangees, nb_rangees);

        case 2:  /* Avancé/Agressif */
            return strategie_agressive(robot, rangees, nb_rangees);

        default:
            return strategie_petite_carte(robot, rangees, nb_rangees);
    }
}

/**
 * Retire la carte choisie de la main du robot
 */
void retirer_carte_robot(Joueur* robot, Carte carte) {
    for (int i = 0; i < robot->nb_cartes_main; i++) {
        if (robot->main[i].valeur == carte.valeur &&
            robot->main[i].tetes_de_boeuf == carte.tetes_de_boeuf) {

            /* Décaler les cartes */
            for (int j = i; j < robot->nb_cartes_main - 1; j++) {
                robot->main[j] = robot->main[j + 1];
            }
            robot->nb_cartes_main--;
            return;
        }
    }

    printf("ERREUR: carte non trouvée dans la main du robot %s\n", robot->nom);
}

/* ========== GESTION DU TEMPS (TIMEOUT) ========== */

/**
 * Gestionnaire de signal pour le timeout
 */
void gestionnaire_timeout(int sig) {
    if (sig == SIGALRM) {
        timeout_atteint = 1;
        printf("⏰ Timeout! Le joueur a mis trop de temps.\n");
    }
}

/**
 * Configure un timeout pour le choix d'une carte
 * Retourne 1 si timeout atteint, 0 sinon
 */
int configurer_timeout(int secondes) {
    timeout_atteint = 0;

    /* Configurer le gestionnaire de signal */
    signal(SIGALRM, gestionnaire_timeout);

    /* Configurer l'alarme */
    alarm(secondes);

    return 0;
}

/**
 * Désactive le timeout
 */
void desactiver_timeout(void) {
    alarm(0);  /* Désactiver l'alarme */
    signal(SIGALRM, SIG_DFL);  /* Rétablir le comportement par défaut */
}

/* ========== COMMUNICATION PAR FIFOs ========== */

/**
 * Crée le nom du FIFO pour un joueur
 */
void creer_nom_fifo_joueur(char* buffer, int taille, int id_joueur, const char* suffixe) {
    snprintf(buffer, taille, "%s%d_%s", NOM_FIFO_BASE, id_joueur, suffixe);
}

/**
 * Crée les FIFOs pour un joueur
 * Retourne 1 en cas de succès, 0 sinon
 */
int creer_fifos_joueur(int id_joueur) {
    char nom_fifo[256];

    /* FIFO pour envoyer au joueur */
    creer_nom_fifo_joueur(nom_fifo, sizeof(nom_fifo), id_joueur, "in");
    if (mkfifo(nom_fifo, 0666) == -1 && errno != EEXIST) {
        perror("Erreur création FIFO in");
        return 0;
    }

    /* FIFO pour recevoir du joueur */
    creer_nom_fifo_joueur(nom_fifo, sizeof(nom_fifo), id_joueur, "out");
    if (mkfifo(nom_fifo, 0666) == -1 && errno != EEXIST) {
        perror("Erreur création FIFO out");
        return 0;
    }

    return 1;
}

/**
 * Ouvre les FIFOs d'un joueur
 */
int ouvrir_fifos_joueur(int id_joueur, int* fd_in, int* fd_out) {
    char nom_fifo[256];

    /* Ouvrir FIFO d'entrée (gestionnaire -> joueur) en écriture */
    creer_nom_fifo_joueur(nom_fifo, sizeof(nom_fifo), id_joueur, "in");
    *fd_in = open(nom_fifo, O_WRONLY);
    if (*fd_in == -1) {
        perror("Erreur ouverture FIFO in");
        return 0;
    }

    /* Ouvrir FIFO de sortie (joueur -> gestionnaire) en lecture */
    creer_nom_fifo_joueur(nom_fifo, sizeof(nom_fifo), id_joueur, "out");
    *fd_out = open(nom_fifo, O_RDONLY);
    if (*fd_out == -1) {
        perror("Erreur ouverture FIFO out");
        close(*fd_in);
        return 0;
    }

    return 1;
}

/**
 * Ferme les FIFOs d'un joueur
 */
void fermer_fifos_joueur(int fd_in, int fd_out) {
    if (fd_in != -1) close(fd_in);
    if (fd_out != -1) close(fd_out);
}

/**
 * Envoie un message à un joueur via FIFO
 */
int envoyer_message_fifo(int fd_fifo, Message* msg) {
    int bytes_ecrits = write(fd_fifo, msg, sizeof(Message));

    if (bytes_ecrits != sizeof(Message)) {
        if (bytes_ecrits == -1) {
            perror("Erreur écriture FIFO");
        } else {
            printf("Message incomplet envoyé (%d bytes au lieu de %ld)\n",
                   bytes_ecrits, sizeof(Message));
        }
        return 0;
    }

    return 1;
}

/**
 * Reçoit un message d'un joueur via FIFO
 */
int recevoir_message_fifo(int fd_fifo, Message* msg, int timeout_sec) {
    if (timeout_sec > 0) {
        configurer_timeout(timeout_sec);
    }

    int bytes_lus = read(fd_fifo, msg, sizeof(Message));

    if (timeout_sec > 0) {
        desactiver_timeout();
    }

    if (timeout_atteint) {
        printf("Timeout lors de la réception du message\n");
        return 0;
    }

    if (bytes_lus != sizeof(Message)) {
        if (bytes_lus == -1) {
            perror("Erreur lecture FIFO");
        } else {
            printf("Message incomplet reçu (%d bytes au lieu de %ld)\n",
                   bytes_lus, sizeof(Message));
        }
        return 0;
    }

    return 1;
}

/* ========== FONCTIONS PRINCIPALES POUR LES JOUEURS ========== */

/**
 * Boucle principale pour un joueur humain (à exécuter dans un thread/processus)
 */
void boucle_joueur_humain(Joueur* joueur, int fd_in, int fd_out) {
    printf("=== Processus joueur %s démarré ===\n", joueur->nom);

    Message msg;
    int en_jeu = 1;

    while (en_jeu) {
        /* Attendre un message du gestionnaire */
        if (!recevoir_message_fifo(fd_out, &msg, -1)) {
            printf("Erreur de réception, sortie...\n");
            break;
        }

        switch (msg.type) {
            case MSG_DEMANDE_CARTE:
                printf("\n--- C'EST À VOTRE TOUR ---\n");

                /* Afficher les informations */
                if (msg.donnees.message[0] != '\0') {
                    printf("%s\n", msg.donnees.message);
                }

                /* Demander le choix */
                int choix = demander_choix_carte_humain(joueur);

                if (choix >= 0) {
                    /* Envoyer la carte choisie */
                    Message reponse;
                    reponse.type = MSG_CARTE_JOUE;
                    reponse.id_joueur = joueur->id;
                    reponse.donnees.carte = joueur->main[choix];

                    if (!envoyer_message_fifo(fd_in, &reponse)) {
                        printf("Erreur envoi réponse\n");
                    }
                }
                break;

            case MSG_INFO_PARTIE:
                printf("\n=== INFORMATION ===\n");
                printf("%s\n", msg.donnees.message);
                break;

            case MSG_SCORE:
                joueur->score = msg.donnees.score;
                printf("Votre score mis à jour: %d\n", joueur->score);
                break;

            case MSG_FIN_MANCHE:
                printf("\n=== FIN DE LA MANCHE ===\n");
                printf("%s\n", msg.donnees.message);
                break;

            case MSG_FIN_PARTIE:
                printf("\n=== FIN DE LA PARTIE ===\n");
                printf("%s\n", msg.donnees.message);
                en_jeu = 0;
                break;

            case MSG_PING:
                /* Répondre au ping */
                msg.type = MSG_CONFIRMATION;
                envoyer_message_fifo(fd_in, &msg);
                break;

            default:
                printf("Message inconnu reçu (type: %d)\n", msg.type);
        }
    }

    printf("=== Processus joueur %s terminé ===\n", joueur->nom);
}

/**
 * Boucle principale pour un joueur robot
 */
void boucle_joueur_robot(Joueur* robot, int niveau_strategie, int fd_in, int fd_out) {
    printf("=== Processus robot %s démarré (niveau %d) ===\n",
           robot->nom, niveau_strategie);

    Message msg;
    int en_jeu = 1;

    /* Le robot garde une copie des rangées */
    Rangee rangees_copie[4];
    for (int i = 0; i < 4; i++) {
        initialiser_rangee(&rangees_copie[i]);
    }

    while (en_jeu) {
        /* Attendre un message du gestionnaire */
        if (!recevoir_message_fifo(fd_out, &msg, -1)) {
            printf("Erreur de réception, sortie...\n");
            break;
        }

        switch (msg.type) {
            case MSG_DEMANDE_CARTE:
                /* Le robot choisit automatiquement */
                printf("Robot %s doit jouer...\n", robot->nom);

                /* Mettre à jour les rangées si fournies */
                if (msg.donnees.message[0] != '\0') {
                    /* Dans une vraie implémentation, parser les rangées */
                    printf("Info rangées: %s\n", msg.donnees.message);
                }

                /* Simuler un temps de réflexion */
                sleep(1 + (rand() % 2));

                /* Choisir une carte */
                Carte choix = choisir_carte_robot(robot, rangees_copie, 4, niveau_strategie);

                /* Retirer la carte de la main */
                retirer_carte_robot(robot, choix);

                /* Envoyer la réponse */
                Message reponse;
                reponse.type = MSG_CARTE_JOUE;
                reponse.id_joueur = robot->id;
                reponse.donnees.carte = choix;

                if (!envoyer_message_fifo(fd_in, &reponse)) {
                    printf("Erreur envoi réponse robot\n");
                }
                break;

            case MSG_INFO_PARTIE:
                /* Le robot enregistre les infos */
                printf("Robot %s reçoit info: %s\n", robot->nom, msg.donnees.message);
                /* Ici, on devrait parser le message pour mettre à jour rangees_copie */
                break;

            case MSG_SCORE:
                robot->score = msg.donnees.score;
                printf("Robot %s score: %d\n", robot->nom, robot->score);
                break;

            case MSG_FIN_PARTIE:
                printf("Robot %s: fin de partie\n", robot->nom);
                en_jeu = 0;
                break;

            case MSG_PING:
                msg.type = MSG_CONFIRMATION;
                envoyer_message_fifo(fd_in, &msg);
                break;

            default:
                printf("Robot %s: message inconnu %d\n", robot->nom, msg.type);
        }
    }

    printf("=== Processus robot %s terminé ===\n", robot->nom);
}

/* ========== FONCTIONS DE TEST ========== */

/**
 * Test de l'interface joueur
 */
void tester_interface_joueur(void) {
    printf("\n=== TEST INTERFACE JOUEUR ===\n");

    /* Créer un joueur test */
    Joueur* joueur_test = creer_joueur(99, "Testeur", 0);

    /* Lui donner quelques cartes */
    Carte cartes_test[5] = {
        {15, 2},  /* multiple de 5 -> 2 têtes */
        {23, 1},  /* normal -> 1 tête */
        {55, 7},  /* multiple de 55 -> 7 têtes */
        {77, 5},  /* multiple de 11 -> 5 têtes */
        {100, 3}  /* multiple de 10 -> 3 têtes */
    };

    for (int i = 0; i < 5; i++) {
        ajouter_carte_main(joueur_test, cartes_test[i]);
    }

    /* Afficher l'accueil */
    afficher_accueil_joueur(joueur_test);

    /* Tester l'affichage des cartes */
    printf("\nCartes en main:\n");
    for (int i = 0; i < joueur_test->nb_cartes_main; i++) {
        printf("%d. ", i + 1);
        afficher_carte(joueur_test->main[i]);
        printf("\n");
    }

    /* Nettoyer */
    detruire_joueur(joueur_test);

    printf("\n=== FIN TEST INTERFACE ===\n");
}

/**
 * Test des stratégies robot
 */
void tester_strategies_robot(void) {
    printf("\n=== TEST STRATÉGIES ROBOT ===\n");

    /* Créer un robot test */
    Joueur* robot_test = creer_joueur(100, "RobotTest", 1);

    /* Lui donner des cartes */
    Carte cartes_test[5] = {
        {10, 3}, {25, 2}, {50, 2}, {80, 1}, {104, 1}
    };

    for (int i = 0; i < 5; i++) {
        ajouter_carte_main(robot_test, cartes_test[i]);
    }

    /* Créer des rangées test */
    Rangee rangees_test[4];
    for (int i = 0; i < 4; i++) {
        initialiser_rangee(&rangees_test[i]);
    }

    /* Ajouter quelques cartes aux rangées */
    Carte r1 = {15, 2}; ajouter_carte_a_rangee(&rangees_test[0], r1);
    Carte r2 = {30, 3}; ajouter_carte_a_rangee(&rangees_test[1], r2);
    Carte r3 = {45, 2}; ajouter_carte_a_rangee(&rangees_test[2], r3);
    Carte r4 = {60, 3}; ajouter_carte_a_rangee(&rangees_test[3], r4);

    /* Tester différentes stratégies */
    printf("\n1. Stratégie débutant (petite carte):\n");
    Carte choix1 = choisir_carte_robot(robot_test, rangees_test, 4, 0);
    afficher_carte(choix1);
    printf("\n");

    printf("\n2. Stratégie intermédiaire (éviter rangées):\n");
    Carte choix2 = choisir_carte_robot(robot_test, rangees_test, 4, 1);
    afficher_carte(choix2);
    printf("\n");

    printf("\n3. Stratégie avancée (agressive):\n");
    Carte choix3 = choisir_carte_robot(robot_test, rangees_test, 4, 2);
    afficher_carte(choix3);
    printf("\n");

    /* Nettoyer */
    detruire_joueur(robot_test);

    printf("\n=== FIN TEST STRATÉGIES ===\n");
}

/* ========== POINT D'ENTRÉE POUR TEST ========== */
#ifdef TEST_JOUEURS
int main(int argc, char* argv[]) {
    printf("Compilation de joueurs.c en mode test\n");

    /* Initialiser l'aléatoire */
    srand(time(NULL));

    /* Exécuter les tests */
    tester_interface_joueur();
    tester_strategies_robot();

    /* Tester les FIFOs si demandé */
    if (argc > 1 && strcmp(argv[1], "--fifo") == 0) {
        printf("\nTest création FIFOs...\n");
        if (creer_fifos_joueur(999)) {
            printf("FIFOs créés avec succès\n");
        }
    }

    return 0;
}
#endif
