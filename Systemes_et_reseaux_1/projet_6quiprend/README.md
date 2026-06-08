PRÉSENTATION
===============================================================================

6 Qui Prend est un jeu de cartes stratégique où les joueurs tentent de marquer
le moins de points possible. Chaque carte a une valeur et un nombre de "têtes
de bœuf". Le but est d'éviter de ramasser des rangées de cartes.

===============================================================================
FONCTIONNALITÉS
===============================================================================

✓ Jeu complet avec règles officielles
✓ Mode réseau client/serveur TCP
✓ Mode local avec interface FIFO
✓ 3 niveaux d'IA pour les robots
✓ Gestion de jusqu'à 10 joueurs par partie
✓ Support multi-parties simultanées
✓ Interface texte complète
✓ Système de logs et statistiques
✓ Tests unitaires intégrés

===============================================================================
FICHIERS DU PROJET
===============================================================================

main.c - Point d'entrée principal, gestion des modes
jeu.c / jeu.h - Logique métier du jeu (cartes, rangées, parties)
joueurs.c / joueurs.h - Joueurs humains et robots, stratégies IA
gestionnaire.c / gestionnaire.h - Serveur et gestion des parties
communication.c / communication.h - TCP, FIFOs, sémaphores
Makefile - Script de compilation

===============================================================================
COMPILATION
===============================================================================

Pour compiler le projet :

make

Pour compiler en mode debug :

make debug

Pour nettoyer :

make clean

Compilation manuelle :

gcc -o 6quiprend *.c -pthread -Wall -Wextra -std=c99

===============================================================================
UTILISATION
===============================================================================

LANCER UN SERVEUR :

./6quiprend --serveur [port]
(port par défaut : 8080)

SE CONNECTER EN CLIENT :

./6quiprend --client IP PORT
Exemple : ./6quiprend --client 127.0.0.1 8080

MODE LOCAL :

./6quiprend --client-local

DÉMONSTRATION :

./6quiprend --demo
./6quiprend --demo-2v2 (2 humains vs 2 robots)

TESTS :

./6quiprend --test (tous les tests)
./6quiprend --test-jeu (test logique jeu)
./6quiprend --test-joueurs (test joueurs et IA)
./6quiprend --test-comm (test communications)

AIDE ET VERSION :

./6quiprend --help
./6quiprend --version

===============================================================================
RÈGLES DU JEU (RÉSUMÉ)
===============================================================================

• 104 cartes numérotées de 1 à 104
• Chaque carte a un nombre de "têtes de bœuf" :

Multiple de 55 : 7 têtes

Multiple de 11 : 5 têtes

Multiple de 10 : 3 têtes

Multiple de 5 : 2 têtes

Autre : 1 tête

• 4 rangées maximum 6 cartes
• 10 cartes par joueur par manche
• Le joueur avec le score le plus bas gagne
• Élimination à 66 têtes

===============================================================================
STRATÉGIES DES ROBOTS
===============================================================================

NIVEAU 0 - DÉBUTANT :
Joue toujours la plus petite carte disponible.

NIVEAU 1 - INTERMÉDIAIRE :
Évalue les risques et essaie d'éviter de ramasser des rangées.

NIVEAU 2 - AVANCÉ :
Tente de faire ramasser des rangées aux adversaires.

===============================================================================
COMMANDES RÉSEAU
===============================================================================

Une fois connecté au serveur, vous pouvez utiliser :

cartes - Afficher vos cartes
jouer X - Jouer la carte numéro X (ex: jouer 1)
etat - Voir l'état de la partie
quit - Quitter le jeu

===============================================================================
ARCHITECTURE RÉSEAU
===============================================================================

PROTOCOLE TCP :
• Port par défaut : 8080
• Jusqu'à 100 clients simultanés
• Timeout d'inactivité : 60 secondes
• Messages structurés avec taille

COMMUNICATION LOCALE :
• FIFOs dans /tmp/6quiprend_joueur_*
• Sémaphores System V pour synchronisation
• Communication inter-processus sécurisée

===============================================================================
JOURNALISATION
===============================================================================

Le système crée automatiquement :
• logs/parties.log - Historique des parties
• FIFOs dans /tmp/ - Communication locale
• Sémaphores System V - Synchronisation

===============================================================================
EXEMPLE COMPLET
===============================================================================

Démarrer le serveur :

./6quiprend --serveur 8080

Connecter un client (dans un autre terminal) :

./6quiprend --client 127.0.0.1 8080

Dans le client, taper :

cartes (pour voir vos cartes)
jouer 1 (pour jouer la première carte)
etat (pour voir l'état de la partie)

Pour une démonstration rapide :

./6quiprend --demo-2v2

===============================================================================
CONFIGURATION AVANCÉE
===============================================================================

Variables d'environnement :
• Aucune requise

Fichiers de configuration :
• Générés automatiquement

Ports utilisés :
• TCP : 8080 par défaut (configurable)
• FIFOs : /tmp/6quiprend_*

===============================================================================
DÉPANNAGE
===============================================================================

PROBLÈME : "Port déjà utilisé"
SOLUTION : Changer le port ou attendre qu'il se libère

PROBLÈME : "Impossible de créer les FIFOs"
SOLUTION : Vérifier les permissions sur /tmp/

PROBLÈME : "Client ne se connecte pas"
SOLUTION : Vérifier le firewall et l'adresse IP

PROBLÈME : "Segmentation fault"
SOLUTION : Recompiler avec make debug et vérifier les logs

===============================================================================
CRÉDITS
===============================================================================

Développeurs : Ryan LAWSON & Thomas MORAUX
Année : 2024
Version : 1.0.0
Système : Linux/Unix
Licence : Usage académique

===============================================================================
