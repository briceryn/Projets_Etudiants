#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <cmath>
#include "pikachu.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Variables globales pour la gestion de la scène 3D
Pikachu pikachu;
float angle = 0.0f;
float distanceCamera = 6.0f;
float angleCameraX = 10.0f;
float angleCameraY = 0.0f;
int derniereSourisX = 0, derniereSourisY = 0;
bool sourisGaucheEnfoncee = false;
bool rotationAutomatique = true;
bool animationPikachuActive = true;

// Fonction d'affichage principale
void afficherScene() {
    // Nettoyage des buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Dessin de l'arrière-plan en premier
    pikachu.dessinerArrierePlan();

    // Calcul de la position de la caméra (orbite autour de la scène)
    float posCamX = distanceCamera * std::sin(angleCameraY * M_PI / 180.0f) * std::cos(angleCameraX * M_PI / 180.0f);
    float posCamY = distanceCamera * std::sin(angleCameraX * M_PI / 180.0f);
    float posCamZ = distanceCamera * std::cos(angleCameraY * M_PI / 180.0f) * std::cos(angleCameraX * M_PI / 180.0f);

    // Configuration du point de vue de la caméra
    gluLookAt(posCamX, posCamY + 1.0f, posCamZ,   // Position de l'œil
              0.0f, 0.9f, 0.0f,                  // Point visé
              0.0f, 1.0f, 0.0f);                 // Vecteur up

    // Application de la rotation automatique de la scène
    glPushMatrix();
    if (rotationAutomatique) {
        glRotatef(angle, 0.0f, 1.0f, 0.0f);
    }

    // Mise à jour de l'animation de Pikachu si active
    if (animationPikachuActive) {
        pikachu.mettreAJourAnimation();
    }

    // Activation de l'éclairage et dessin de Pikachu
    glEnable(GL_LIGHTING);
    pikachu.dessiner();
    glDisable(GL_LIGHTING);

    glPopMatrix();

    // Échange des buffers pour l'affichage
    glutSwapBuffers();
}

// Fonction de redimensionnement de la fenêtre
void redimensionner(int largeur, int hauteur) {
    if (hauteur == 0) hauteur = 1;

    // Configuration du viewport
    glViewport(0, 0, largeur, hauteur);

    // Passage en mode projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Définition de la perspective
    gluPerspective(45.0, (double)largeur / (double)hauteur, 0.1, 100.0);

    // Retour en mode modèle-vue
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Fonction appelée en idle (rafraîchissement continu)
void animationTempsReel() {
    // Mise à jour de l'angle de rotation automatique
    angle += 0.5f;
    if (angle > 360.0f) angle -= 360.0f;

    // Demande de rafraîchissement de l'affichage
    glutPostRedisplay();
}

// Gestion des touches clavier standard
void gererClavier(unsigned char touche, int x, int y) {
    switch (touche) {
        case 27: // Touche ÉCHAP : quitter l'application
            exit(0);
            break;

        case ' ': // ESPACE : activer/désactiver l'animation de Pikachu
            animationPikachuActive = !animationPikachuActive;
            std::cout << "Animation Pikachu: " << (animationPikachuActive ? "ACTIVEE" : "DESACTIVEE") << std::endl;
            break;

        case 'z': // 'z' minuscule : zoom arrière
            distanceCamera += 0.5f;
            if (distanceCamera > 20.0f) distanceCamera = 20.0f;
            std::cout << "Zoom: " << distanceCamera << std::endl;
            break;

        case 'Z': // 'Z' majuscule : zoom avant
            distanceCamera -= 0.5f;
            if (distanceCamera < 2.0f) distanceCamera = 2.0f;
            std::cout << "Zoom: " << distanceCamera << std::endl;
            break;

        case 'r': // 'r' : activer/désactiver la rotation automatique
        case 'R':
            rotationAutomatique = !rotationAutomatique;
            std::cout << "Rotation automatique: " << (rotationAutomatique ? "ACTIVEE" : "DESACTIVEE") << std::endl;
            break;

        case 'c': // 'c' : afficher les informations de la caméra
        case 'C':
            std::cout << "Camera - Distance: " << distanceCamera
                      << ", AngleX: " << angleCameraX
                      << ", AngleY: " << angleCameraY << std::endl;
            break;
    }
    glutPostRedisplay();
}

// Gestion des touches spéciales (flèches directionnelles)
void gererTouchesSpeciales(int touche, int x, int y) {
    switch (touche) {
        case GLUT_KEY_UP: // Flèche haut : incliner la caméra vers le haut
            angleCameraX += 5.0f;
            if (angleCameraX > 89.0f) angleCameraX = 89.0f;
            break;

        case GLUT_KEY_DOWN: // Flèche bas : incliner la caméra vers le bas
            angleCameraX -= 5.0f;
            if (angleCameraX < -89.0f) angleCameraX = -89.0f;
            break;

        case GLUT_KEY_LEFT: // Flèche gauche : rotation vers la gauche
            angleCameraY += 5.0f;
            break;

        case GLUT_KEY_RIGHT: // Flèche droite : rotation vers la droite
            angleCameraY -= 5.0f;
            break;
    }
    glutPostRedisplay();
}

// Gestion des événements de la souris
void gererSouris(int bouton, int etat, int x, int y) {
    if (bouton == GLUT_LEFT_BUTTON) {
        if (etat == GLUT_DOWN) {
            sourisGaucheEnfoncee = true;
            derniereSourisX = x;
            derniereSourisY = y;
        } else if (etat == GLUT_UP) {
            sourisGaucheEnfoncee = false;
        }
    }
}

// Gestion du déplacement de la souris
void gererDeplacementSouris(int x, int y) {
    if (sourisGaucheEnfoncee) {
        // Calcul des déplacements angulaires
        angleCameraY += (x - derniereSourisX) * 0.5f;
        angleCameraX += (y - derniereSourisY) * 0.5f;

        // Limitation de l'angle vertical
        if (angleCameraX > 89.0f) angleCameraX = 89.0f;
        if (angleCameraX < -89.0f) angleCameraX = -89.0f;

        // Mise à jour de la position précédente
        derniereSourisX = x;
        derniereSourisY = y;

        glutPostRedisplay();
    }
}

// Configuration de l'éclairage de la scène
void configurerEclairage() {
    // Activation du système d'éclairage
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Configuration de la lumière principale (LIGHT0)
    GLfloat positionLumiere0[] = { 3.0f, 5.0f, 3.0f, 1.0f };
    GLfloat diffuseLumiere0[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat ambianteLumiere0[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat speculaireLumiere0[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, positionLumiere0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLumiere0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambianteLumiere0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, speculaireLumiere0);

    // Configuration de la deuxième lumière (LIGHT1) - lumière d'ambiance colorée
    glEnable(GL_LIGHT1);
    GLfloat positionLumiere1[] = { -3.0f, 3.0f, -3.0f, 1.0f };
    GLfloat diffuseLumiere1[] = { 0.5f, 0.5f, 0.7f, 1.0f };
    GLfloat ambianteLumiere1[] = { 0.1f, 0.1f, 0.2f, 1.0f };

    glLightfv(GL_LIGHT1, GL_POSITION, positionLumiere1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLumiere1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambianteLumiere1);
}

// Configuration des propriétés des matériaux
void configurerMateriaux() {
    // Configuration des propriétés spéculaires
    GLfloat speculaireMateriau[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat brillanceMateriau[] = { 50.0f };

    glMaterialfv(GL_FRONT, GL_SPECULAR, speculaireMateriau);
    glMaterialfv(GL_FRONT, GL_SHININESS, brillanceMateriau);

    // Activation de la gestion automatique des couleurs des matériaux
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

// Affichage des instructions de contrôle
void afficherInstructions() {
    std::cout << "==========================================" << std::endl;
    std::cout << "          PIKACHU 3D - TP TEXTURES" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "CONTROLES :" << std::endl;
    std::cout << "- ESPACE     : Animation Pikachu ON/OFF" << std::endl;
    std::cout << "- R          : Rotation auto ON/OFF" << std::endl;
    std::cout << "- z / Z      : Zoomer / Dezoomer" << std::endl;
    std::cout << "- Fleches    : Rotation camera" << std::endl;
    std::cout << "- Souris     : Rotation camera (clic gauche)" << std::endl;
    std::cout << "- C          : Info camera" << std::endl;
    std::cout << "- ESC        : Quitter" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "FONCTIONNALITES IMPLEMENTEES :" << std::endl;
    std::cout << "- 2 textures differentes" << std::endl;
    std::cout << "- Texture enroulee autour d'un cylindre" << std::endl;
    std::cout << "- 2 types de lumieres" << std::endl;
    std::cout << "- Controles camera (z/Z, fleches)" << std::endl;
    std::cout << "- 2 animations (auto + clavier)" << std::endl;
    std::cout << "==========================================" << std::endl;
}

// Fonction principale
int main(int argc, char** argv) {
    // Initialisation de GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 800);
    glutCreateWindow("Pikachu 3D");

    // Configuration OpenGL de base
    glEnable(GL_DEPTH_TEST);   // Activation du test de profondeur
    glEnable(GL_NORMALIZE);    // Normalisation automatique des normales

    // Configuration de l'éclairage et des matériaux
    configurerEclairage();
    configurerMateriaux();

    // Définition de la couleur de fond (bleu ciel)
    glClearColor(0.4f, 0.6f, 0.8f, 1.0f);

    // Chargement des textures
    std::cout << "Chargement des textures..." << std::endl;
    if (!pikachu.chargerTextures()) {
        std::cout << "Attention: certaines textures n'ont pas pu etre chargees." << std::endl;
        std::cout << "Des textures alternatives ont ete creees." << std::endl;
    }
    std::cout << "Textures chargees avec succes." << std::endl;

    // Enregistrement des fonctions de callback
    glutDisplayFunc(afficherScene);
    glutReshapeFunc(redimensionner);
    glutKeyboardFunc(gererClavier);
    glutSpecialFunc(gererTouchesSpeciales);
    glutMouseFunc(gererSouris);
    glutMotionFunc(gererDeplacementSouris);
    glutIdleFunc(animationTempsReel);

    // Affichage des instructions
    afficherInstructions();

    // Informations de démarrage
    std::cout << std::endl;
    std::cout << "Demarrage de l'application..." << std::endl;
    std::cout << "Placez les fichiers de texture (peau.jpg, rayures.jpg, scene.jpg)" << std::endl;
    std::cout << "dans le meme repertoire que l'executable." << std::endl;
    std::cout << std::endl;

    // Lancement de la boucle principale GLUT
    glutMainLoop();

    return 0;
}
