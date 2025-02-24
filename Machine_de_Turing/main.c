#include <stdio.h>
#include <stdint.h>

// Définition des valeurs possibles sur la bande
#define BLANK 0b01000000  // Symbole '#' (case vide)
#define ZERO  0b00000000  // Symbole '0'
#define ONE   0b00100000  // Symbole '1'

// Définition des directions de déplacement
#define LEFT  0b00000000  // Déplacement à gauche
#define RIGHT 0b10000000  // Déplacement à droite

// Code d'arrêt de la machine
#define STOP 31

// Déclaration des variables globales
int Head = 0;      // Position de la tête de lecture
int State = 0;     // État courant de la machine

// Définition de la bande initiale (longueur max = 32 cases)
// Ici, la bande commence par : # 0 1 1 0 #
uint8_t Tape[32] = {2, 0, 1, 1, 0, 2};

// Programme de la machine de Turing sous forme de table de transitions
uint8_t Prog[] = {
    //  État 0
    ZERO|RIGHT|STOP,  // Si '0' -> écrire '0', aller à droite, arrêter
    ONE|RIGHT|STOP,   // Si '1' -> écrire '1', aller à droite, arrêter
    BLANK|RIGHT|1,    // Si '#' -> écrire '#', aller à droite, passer à l'état 1

    //  État 1
    ZERO|RIGHT|1,     // Si '0' -> écrire '0', aller à droite, rester à l'état 1
    ONE|RIGHT|1,      // Si '1' -> écrire '1', aller à droite, rester à l'état 1
    BLANK|LEFT|2,     // Si '#' -> écrire '#', aller à gauche, passer à l'état 2

    //  État 2
    ONE|LEFT|3,       // Si '0' -> écrire '1', aller à gauche, passer à l'état 3
    ZERO|LEFT|2,      // Si '1' -> écrire '0', aller à gauche, rester à l'état 2
    BLANK|LEFT|STOP,  // Si '#' -> écrire '#', aller à gauche, arrêter

    //  État 3
    ZERO|LEFT|3,      // Si '0' -> écrire '0', aller à gauche, rester à l'état 3
    ONE|LEFT|3,       // Si '1' -> écrire '1', aller à gauche, rester à l'état 3
    BLANK|LEFT|STOP   // Si '#' -> écrire '#', aller à gauche, arrêter
};

// Fonction pour lire la valeur sur la bande à une position donnée
int read(int i) {
    return Tape[i];
}

// Fonction pour écrire une valeur sur la bande à une position donnée
void write(int i, int val) {
    Tape[i] = val;
}

// Fonction d'affichage de la bande et de la position de la tête
void print() {
    for (int i = 0; i < 32; i++) {
        if (Tape[i] == 2) printf("# ");  // Affiche '#' pour les cases vides
        else printf("%d ", Tape[i]);    // Affiche les valeurs 0 ou 1
    }
    printf("[%d]\n", State); // Affiche l'état actuel de la machine

    // Affiche un curseur sous la tête de lecture
    for (int i = 0; i < Head; i++) printf("  ");
    printf("T\n");
}

// Fonction pour récupérer l'action suivante à effectuer selon l'état et la lecture actuelle
uint8_t nextAction() {
    return Prog[(3 * State) + read(Head)]; // Chaque état a 3 transitions possibles (0, 1, #)
}

// Fonction qui effectue une opération en fonction de l'action courante
void operate(uint8_t action) {
    // Extraction et écriture de la nouvelle valeur (les bits 5 et 6 contiennent la valeur)
    write(Head, (action & 0b01100000) >> 5);

    // Gestion du déplacement de la tête de lecture
    if ((action & 0b10000000) == RIGHT && Head < 31)
        Head++;  // Se déplace à droite si possible
    else if ((action & 0b10000000) == LEFT && Head > 0)
        Head--;  // Se déplace à gauche si possible

    // Mise à jour de l'état (bits 0 à 4 contiennent le nouvel état)
    State = action & 0b00011111;
}

// Fonction principale qui exécute la machine de Turing
void run() {
    print(); // Affiche l'état initial
    State = 0; // Initialise l'état

    // Boucle d'exécution jusqu'à l'état STOP
    while (State != STOP) {
        uint8_t action = nextAction(); // Récupère l'action à effectuer
        operate(action);               // Exécute l'action
        print();                        // Affiche la bande mise à jour
        getchar();  // Attente de l'entrée utilisateur pour observer l'évolution
    }

    printf("Exécution terminée\n");
}

// Fonction principale qui lance l'exécution de la machine de Turing
int main() {
    run();
    return 0;
}
