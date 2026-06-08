#ifndef PIKACHU_H
#define PIKACHU_H

#include <GL/glut.h>

class Pikachu {
private:
    // Identifiants des textures OpenGL
    GLuint textureID;           // Texture principale pour la peau (ex: calimero.jpg)
    GLuint textureID2;          // Texture secondaire pour les accessoires (ex: bat.jpg)
    GLuint backgroundTextureID; // Texture pour l'arrière-plan de la scène

    // Variables d'animation
    float animationAngle;       // Angle courant pour les animations

    // Dimensions des textures chargées
    int larging1, hauting1;     // Dimensions de la première texture
    int larging2, hauting2;     // Dimensions de la deuxième texture

public:
    // Constructeur et destructeur
    Pikachu();
    ~Pikachu();

    // Méthodes de gestion des textures
    bool chargerImageJpeg(const char* filename, GLuint &textureID, int &larging, int &hauting);
    bool chargerTextures();
    void creerTextureDamier();  // Crée une texture damier comme alternative

    // Méthodes d'affichage principales
    void dessiner();            // Dessine Pikachu complet
    void dessinerArrierePlan(); // Dessine le fond de la scène
    void mettreAJourAnimation(); // Met à jour l'animation

private:
    // Méthodes de dessin des primitives géométriques
    void dessinerSphere(float radius, int segments);
    void dessinerCylindreFacettes(float radius, float height, int segments); // Version avec facettes
    void dessinerCylindreTexture(float radius, float height, int segments);  // Version texturée
    void dessinerCubeTexture(); // Cube texturé comme dans l'exercice du TP

    // Méthodes de dessin des parties du corps
    void dessinerCorps();       // Dessine le corps principal
    void dessinerTete();        // Dessine la tête
    void dessinerOreilles();    // Dessine les oreilles
    void dessinerPattes();      // Dessine les quatre pattes
    void dessinerQueue();       // Dessine la queue caractéristique
    void dessinerYeux();        // Dessine les yeux et les reflets
    void dessinerJoues();       // Dessine les joues roses
    void dessinerBouche();      // Dessine la bouche
    void dessinerNez();         // Dessine le nez triangulaire
    void dessinerAccessoire();  // Dessine l'accessoire (ceinture)
};

#endif
