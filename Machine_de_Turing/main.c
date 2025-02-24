#include <stdio.h>
#include <stdint.h>

// D�finition des valeurs possibles sur la bande
#define BLANK 0b01000000  // Symbole '#' (case vide)
#define ZERO  0b00000000  // Symbole '0'
#define ONE   0b00100000  // Symbole '1'

// D�finition des directions de d�placement
#define LEFT  0b00000000  // D�placement � gauche
#define RIGHT 0b10000000  // D�placement � droite

// Code d'arr�t de la machine
#define STOP 31

// D�claration des variables globales
int Head = 0;      // Position de la t�te de lecture
int State = 0;     // �tat courant de la machine

// D�finition de la bande initiale (longueur max = 32 cases)
// Ici, la bande commence par : # 0 1 1 0 #
uint8_t Tape[32] = {2, 0, 1, 1, 0, 2};

// Programme de la machine de Turing sous forme de table de transitions
uint8_t Prog[] = {
    //  �tat 0
    ZERO|RIGHT|STOP,  // Si '0' -> �crire '0', aller � droite, arr�ter
    ONE|RIGHT|STOP,   // Si '1' -> �crire '1', aller � droite, arr�ter
    BLANK|RIGHT|1,    // Si '#' -> �crire '#', aller � droite, passer � l'�tat 1

    //  �tat 1
    ZERO|RIGHT|1,     // Si '0' -> �crire '0', aller � droite, rester � l'�tat 1
    ONE|RIGHT|1,      // Si '1' -> �crire '1', aller � droite, rester � l'�tat 1
    BLANK|LEFT|2,     // Si '#' -> �crire '#', aller � gauche, passer � l'�tat 2

    //  �tat 2
    ONE|LEFT|3,       // Si '0' -> �crire '1', aller � gauche, passer � l'�tat 3
    ZERO|LEFT|2,      // Si '1' -> �crire '0', aller � gauche, rester � l'�tat 2
    BLANK|LEFT|STOP,  // Si '#' -> �crire '#', aller � gauche, arr�ter

    //  �tat 3
    ZERO|LEFT|3,      // Si '0' -> �crire '0', aller � gauche, rester � l'�tat 3
    ONE|LEFT|3,       // Si '1' -> �crire '1', aller � gauche, rester � l'�tat 3
    BLANK|LEFT|STOP   // Si '#' -> �crire '#', aller � gauche, arr�ter
};

// Fonction pour lire la valeur sur la bande � une position donn�e
int read(int i) {
    return Tape[i];
}

// Fonction pour �crire une valeur sur la bande � une position donn�e
void write(int i, int val) {
    Tape[i] = val;
}

// Fonction d'affichage de la bande et de la position de la t�te
void print() {
    for (int i = 0; i < 32; i++) {
        if (Tape[i] == 2) printf("# ");  // Affiche '#' pour les cases vides
        else printf("%d ", Tape[i]);    // Affiche les valeurs 0 ou 1
    }
    printf("[%d]\n", State); // Affiche l'�tat actuel de la machine

    // Affiche un curseur sous la t�te de lecture
    for (int i = 0; i < Head; i++) printf("  ");
    printf("T\n");
}

// Fonction pour r�cup�rer l'action suivante � effectuer selon l'�tat et la lecture actuelle
uint8_t nextAction() {
    return Prog[(3 * State) + read(Head)]; // Chaque �tat a 3 transitions possibles (0, 1, #)
}

// Fonction qui effectue une op�ration en fonction de l'action courante
void operate(uint8_t action) {
    // Extraction et �criture de la nouvelle valeur (les bits 5 et 6 contiennent la valeur)
    write(Head, (action & 0b01100000) >> 5);

    // Gestion du d�placement de la t�te de lecture
    if ((action & 0b10000000) == RIGHT && Head < 31)
        Head++;  // Se d�place � droite si possible
    else if ((action & 0b10000000) == LEFT && Head > 0)
        Head--;  // Se d�place � gauche si possible

    // Mise � jour de l'�tat (bits 0 � 4 contiennent le nouvel �tat)
    State = action & 0b00011111;
}

// Fonction principale qui ex�cute la machine de Turing
void run() {
    print(); // Affiche l'�tat initial
    State = 0; // Initialise l'�tat

    // Boucle d'ex�cution jusqu'� l'�tat STOP
    while (State != STOP) {
        uint8_t action = nextAction(); // R�cup�re l'action � effectuer
        operate(action);               // Ex�cute l'action
        print();                        // Affiche la bande mise � jour
        getchar();  // Attente de l'entr�e utilisateur pour observer l'�volution
    }

    printf("Ex�cution termin�e\n");
}

// Fonction principale qui lance l'ex�cution de la machine de Turing
int main() {
    run();
    return 0;
}
