#include "pikachu.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <jpeglib.h>
#include <setjmp.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Constructeur : initialise les textures et les variables d'animation
Pikachu::Pikachu()
    : textureID(0), textureID2(0), backgroundTextureID(0),
      animationAngle(0.0f), larging1(0), hauting1(0), larging2(0), hauting2(0) {
}

// Destructeur : nettoie les textures OpenGL
Pikachu::~Pikachu() {
    if (textureID) glDeleteTextures(1, &textureID);
    if (textureID2) glDeleteTextures(1, &textureID2);
    if (backgroundTextureID) glDeleteTextures(1, &backgroundTextureID);
}

// Charge une image JPEG et crée une texture OpenGL
bool Pikachu::chargerImageJpeg(const char* filename, GLuint &textureID, int &larging, int &hauting) {
    FILE* infile = fopen(filename, "rb");
    if (!infile) {
        std::cerr << "Erreur : impossible d'ouvrir " << filename << std::endl;
        return false;
    }

    // Initialisation de la décompression JPEG
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);

    // Lecture de l'en-tête JPEG
    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
        std::cerr << "Erreur : entête JPEG invalide\n";
        fclose(infile);
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    jpeg_start_decompress(&cinfo);

    // Récupération des dimensions de l'image
    larging = cinfo.output_width;
    hauting = cinfo.output_height;
    int channels = cinfo.output_components;

    if (channels != 3 && channels != 4) {
        std::cerr << "Warning: image channels = " << channels << " (expected 3 or 4). Forcing 3 channels.\n";
    }

    // Allocation mémoire pour les données de l'image
    size_t row_stride = larging * channels;
    unsigned char* data = (unsigned char*)malloc((size_t)larging * hauting * channels);
    if (!data) {
        std::cerr << "Erreur allocation memoire pour l'image\n";
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return false;
    }

    // Lecture des lignes de l'image
    while (cinfo.output_scanline < cinfo.output_height) {
        unsigned char* rowptr[1];
        rowptr[0] = data + (size_t)cinfo.output_scanline * row_stride;
        jpeg_read_scanlines(&cinfo, rowptr, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    // Création de la texture OpenGL
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Configuration des paramètres de texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, larging, hauting, 0, format, GL_UNSIGNED_BYTE, data);

    free(data);
    std::cout << "Texture '" << filename << "' chargee : " << larging << "x" << hauting << std::endl;
    return true;
}

// Crée une texture damier en alternative
void Pikachu::creerTextureDamier() {
    const int size = 64;
    GLubyte checkerboard[size][size][3];

    // Génération du motif damier
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            GLubyte c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) ? 255 : 0;
            checkerboard[i][j][0] = c;
            checkerboard[i][j][1] = c;
            checkerboard[i][j][2] = c;
        }
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, checkerboard);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    std::cout << "Texture damier creee : " << size << "x" << size << std::endl;
}

// Charge toutes les textures nécessaires
bool Pikachu::chargerTextures() {
    // Texture principale pour la peau
    if (!chargerImageJpeg("peau.jpg", textureID, larging1, hauting1)) {
        std::cerr << "Creation texture damier en alternative\n";
        creerTextureDamier();
    }

    // Deuxieme texture pour accessoire
    if (!chargerImageJpeg("rayures.jpg", textureID2, larging2, hauting2)) {
        std::cerr << "Texture rayures non chargee\n";
    }

    // Texture de fond
    int larging3, hauting3;
    if (!chargerImageJpeg("scene.jpg", backgroundTextureID, larging3, hauting3)) {
        std::cerr << "Texture de fond non chargee\n";
    }

    return true;
}

// Met à jour l'animation de Pikachu
void Pikachu::mettreAJourAnimation() {
    animationAngle += 2.0f;
    if (animationAngle > 360.0f) animationAngle -= 360.0f;
}

// Dessine l'arrière-plan avec la texture de fond
void Pikachu::dessinerArrierePlan() {
    if (!backgroundTextureID) return;

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTextureID);
    glDisable(GL_DEPTH_TEST);

    // Dessin d'un quad texturé pour le fond
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-15.0f, -10.0f, -20.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(15.0f, -10.0f, -20.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(15.0f, 10.0f, -20.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-15.0f, 10.0f, -20.0f);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// Dessine une sphère avec un nombre donné de segments
void Pikachu::dessinerSphere(float radius, int segments) {
    for (int i = 0; i < segments; ++i) {
        float lat0 = M_PI * (-0.5f + (float)i / segments);
        float lat1 = M_PI * (-0.5f + (float)(i + 1) / segments);
        float z0 = sinf(lat0);
        float zr0 = cosf(lat0);
        float z1 = sinf(lat1);
        float zr1 = cosf(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= segments; ++j) {
            float lng = 2.0f * M_PI * (float)j / segments;
            float x = cosf(lng);
            float y = sinf(lng);

            float u = (float)j / segments;
            float v0 = (float)i / segments;
            glNormal3f(x * zr0, y * zr0, z0);
            glTexCoord2f(u, v0);
            glVertex3f(radius * x * zr0, radius * y * zr0, radius * z0);

            float v1 = (float)(i + 1) / segments;
            glNormal3f(x * zr1, y * zr1, z1);
            glTexCoord2f(u, v1);
            glVertex3f(radius * x * zr1, radius * y * zr1, radius * z1);
        }
        glEnd();
    }
}

// Dessine un cylindre avec des facettes
void Pikachu::dessinerCylindreFacettes(float radius, float height, int segments) {
    float angleStep = 2.0f * M_PI / segments;

    // Surface latérale avec texture
    for (int i = 0; i < segments; i++) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        float x1 = cosf(angle1) * radius;
        float z1 = sinf(angle1) * radius;
        float x2 = cosf(angle2) * radius;
        float z2 = sinf(angle2) * radius;

        glBegin(GL_QUADS);
        glNormal3f(cosf(angle1), 0.0f, sinf(angle1));
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, -height/2, z1);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x2, -height/2, z2);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x2, height/2, z2);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x1, height/2, z1);
        glEnd();
    }

    // Couvercles bleus
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.0f, 0.0f, 1.0f);

    // Base inférieure
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, -height/2, 0.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep;
        glVertex3f(cosf(angle) * radius, -height/2, sinf(angle) * radius);
    }
    glEnd();

    // Base supérieure
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, height/2, 0.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep;
        glVertex3f(cosf(angle) * radius, height/2, sinf(angle) * radius);
    }
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
}

// Dessine un cylindre avec texture enroulée
void Pikachu::dessinerCylindreTexture(float radius, float height, int segments) {
    float angleStep = 2.0f * M_PI / segments;

    // Texture enroulée autour du cylindre
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep;
        float x = cosf(angle);
        float z = sinf(angle);

        float u = (float)i / segments;

        glNormal3f(x, 0.0f, z);
        glTexCoord2f(u, 0.0f); glVertex3f(x * radius, -height/2, z * radius);
        glTexCoord2f(u, 1.0f); glVertex3f(x * radius, height/2, z * radius);
    }
    glEnd();

    // Couvercles
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, -height/2, 0.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep;
        glVertex3f(cosf(angle) * radius, -height/2, sinf(angle) * radius);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, height/2, 0.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep;
        glVertex3f(cosf(angle) * radius, height/2, sinf(angle) * radius);
    }
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
}

// Dessine un cube texturé
void Pikachu::dessinerCubeTexture() {
    float size = 0.5f;

    glBegin(GL_QUADS);

    // Face avant
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, -size, size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, size, size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, size);

    // Face arrière
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(size, -size, -size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(size, size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, size, -size);

    // Face droite
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, -size, size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, size, size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(size, size, -size);

    // Face gauche
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, size, -size);

    // Face supérieure
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, size, size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, size);

    // Face inférieure
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, -size, size);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, size);

    glEnd();
}

// Dessine l'accessoire (ceinture) de Pikachu
void Pikachu::dessinerAccessoire() {
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

    if (textureID2) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
        glColor3f(0.9f, 0.2f, 0.1f); // Rouge fallback
    }

    // Ceinture fine avec texture enroulée
    dessinerCylindreTexture(0.92f, 0.1f, 32);

    if (textureID2) glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// Fonction principale pour dessiner Pikachu
void Pikachu::dessiner() {
    glPushMatrix();

    // Animation de balancement
    glRotatef(sin(animationAngle * M_PI / 180.0f) * 5.0f, 0.0f, 0.0f, 1.0f);

    // Application de la texture principale
    if (textureID) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    } else {
        glColor3f(1.0f, 0.85f, 0.0f);
    }

    dessinerCorps();
    dessinerTete();
    dessinerOreilles();

    if (textureID) glDisable(GL_TEXTURE_2D);

    dessinerPattes();
    dessinerQueue();
    dessinerYeux();
    dessinerJoues();
    dessinerNez();
    dessinerBouche();

    // Accessoire avec deuxième texture
    dessinerAccessoire();

    glPopMatrix();
}

// Dessine le corps de Pikachu
void Pikachu::dessinerCorps() {
    glPushMatrix();
    glScalef(1.0f, 1.0f, 0.8f);
    dessinerSphere(1.0f, 24);
    glPopMatrix();
}

// Dessine la tête de Pikachu
void Pikachu::dessinerTete() {
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, 0.0f);
    glScalef(0.85f, 0.85f, 0.85f);
    dessinerSphere(0.9f, 24);
    glPopMatrix();
}

// Dessine les oreilles de Pikachu
void Pikachu::dessinerOreilles() {
    // Oreille droite
    glPushMatrix();
    glTranslatef(0.45f, 2.35f, 0.0f);
    glRotatef(-15.0f, 0.0f, 0.0f, 1.0f);

    // Partie intérieure avec texture rayures
    if (textureID2) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID2);
    } else {
        glColor3f(0.1f, 0.1f, 0.1f);
    }
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f);
    glScalef(0.1f, 0.3f, 0.1f);
    dessinerSphere(0.5f, 12);
    glPopMatrix();

    // Partie extérieure avec texture peau
    if (textureID) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    } else {
        glColor3f(1.0f, 0.85f, 0.0f);
    }
    glPushMatrix();
    glScalef(0.14f, 0.45f, 0.14f);
    dessinerSphere(0.9f, 16);
    glPopMatrix();

    glPopMatrix();

    // Oreille gauche
    glPushMatrix();
    glTranslatef(-0.45f, 2.35f, 0.0f);
    glRotatef(15.0f, 0.0f, 0.0f, 1.0f);

    // Partie intérieure avec texture rayures
    if (textureID2) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID2);
    } else {
        glColor3f(0.1f, 0.1f, 0.1f);
    }
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f);
    glScalef(0.1f, 0.3f, 0.1f);
    dessinerSphere(0.5f, 12);
    glPopMatrix();

    // Partie extérieure avec texture peau
    if (textureID) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    } else {
        glColor3f(1.0f, 0.85f, 0.0f);
    }
    glPushMatrix();
    glScalef(0.14f, 0.45f, 0.14f);
    dessinerSphere(0.9f, 16);
    glPopMatrix();

    glPopMatrix();

    // Rétablissement de la texture principale
    if (textureID) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
    glColor3f(1.0f, 0.85f, 0.0f);
}

// Dessine les pattes de Pikachu
void Pikachu::dessinerPattes() {
    glColor3f(1.0f, 0.85f, 0.0f);

    // Patte avant droite
    glPushMatrix();
    glTranslatef(0.5f, 0.45f, 0.6f);
    glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
    dessinerSphere(0.18f, 12);
    glPopMatrix();

    // Patte avant gauche
    glPushMatrix();
    glTranslatef(-0.5f, 0.45f, 0.6f);
    glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
    dessinerSphere(0.18f, 12);
    glPopMatrix();

    // Patte arrière droite
    glPushMatrix();
    glTranslatef(0.45f, -0.75f, 0.3f);
    glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
    dessinerSphere(0.24f, 12);
    glPopMatrix();

    // Patte arrière gauche
    glPushMatrix();
    glTranslatef(-0.45f, -0.75f, 0.3f);
    glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
    dessinerSphere(0.24f, 12);
    glPopMatrix();
}

// Dessine la queue de Pikachu
void Pikachu::dessinerQueue() {
    glColor3f(1.0f, 0.85f, 0.0f);

    glPushMatrix();
    glTranslatef(0.3f, 0.0f, -0.5f);
    glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(60.0f, 0.0f, 1.0f, 0.0f);

    // Segments de la queue
    float segments[][3] = {
        {0.0f, 0.0f, 0.0f},
        {0.12f, 0.25f, 0.0f},
        {-0.08f, 0.5f, 0.0f},
        {0.06f, 0.75f, 0.0f},
        {-0.04f, 1.0f, 0.0f},
        {0.02f, 1.2f, 0.0f}
    };

    float thickness = 0.08f;

    // Construction de la queue segment par segment
    for (int i = 0; i < 5; i++) {
        glBegin(GL_QUADS);
        // Face avant
        glVertex3f(segments[i][0] - thickness, segments[i][1], thickness);
        glVertex3f(segments[i][0] + thickness, segments[i][1], thickness);
        glVertex3f(segments[i+1][0] + thickness, segments[i+1][1], thickness);
        glVertex3f(segments[i+1][0] - thickness, segments[i+1][1], thickness);
        // Face arrière
        glVertex3f(segments[i][0] - thickness, segments[i][1], -thickness);
        glVertex3f(segments[i+1][0] - thickness, segments[i+1][1], -thickness);
        glVertex3f(segments[i+1][0] + thickness, segments[i+1][1], -thickness);
        glVertex3f(segments[i][0] + thickness, segments[i][1], -thickness);
        // Côté gauche
        glVertex3f(segments[i][0] - thickness, segments[i][1], -thickness);
        glVertex3f(segments[i][0] - thickness, segments[i][1], thickness);
        glVertex3f(segments[i+1][0] - thickness, segments[i+1][1], thickness);
        glVertex3f(segments[i+1][0] - thickness, segments[i+1][1], -thickness);
        // Côté droit
        glVertex3f(segments[i][0] + thickness, segments[i][1], -thickness);
        glVertex3f(segments[i+1][0] + thickness, segments[i+1][1], -thickness);
        glVertex3f(segments[i+1][0] + thickness, segments[i+1][1], thickness);
        glVertex3f(segments[i][0] + thickness, segments[i][1], thickness);
        glEnd();
    }

    // Bout noir de la queue
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(segments[5][0], segments[5][1], 0.0f);
    glBegin(GL_TRIANGLES);
    // Face avant
    glVertex3f(-0.06f, 0.0f, thickness);
    glVertex3f(0.06f, 0.0f, thickness);
    glVertex3f(0.0f, 0.1f, thickness);
    // Face arrière
    glVertex3f(-0.06f, 0.0f, -thickness);
    glVertex3f(0.0f, 0.1f, -thickness);
    glVertex3f(0.06f, 0.0f, -thickness);
    glEnd();
    glPopMatrix();
    glPopMatrix();
}

// Dessine les yeux de Pikachu
void Pikachu::dessinerYeux() {
    // Pupilles noires
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(0.25f, 1.9f, 0.6f);
    glScalef(0.6f, 0.6f, 0.15f);
    dessinerSphere(0.12f, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.25f, 1.9f, 0.6f);
    glScalef(0.6f, 0.6f, 0.15f);
    dessinerSphere(0.12f, 12);
    glPopMatrix();

    // Reflets blancs
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.28f, 1.92f, 0.62f);
    glScalef(0.4f, 0.4f, 0.2f);
    dessinerSphere(0.05f, 8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.22f, 1.92f, 0.62f);
    glScalef(0.4f, 0.4f, 0.2f);
    dessinerSphere(0.05f, 8);
    glPopMatrix();
}

// Dessine les joues roses de Pikachu
void Pikachu::dessinerJoues() {
    glColor3f(0.9f, 0.3f, 0.2f);
    glPushMatrix();
    glTranslatef(0.45f, 1.6f, 0.65f);
    glScalef(0.9f, 0.9f, 0.4f);
    dessinerSphere(0.16f, 16);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.45f, 1.6f, 0.65f);
    glScalef(0.9f, 0.9f, 0.4f);
    dessinerSphere(0.16f, 16);
    glPopMatrix();
}

// Dessine la bouche de Pikachu
void Pikachu::dessinerBouche() {
    glColor3f(0.25f, 0.25f, 0.25f);
    glPushMatrix();
    glTranslatef(0.0f, 1.62f, 0.75f);
    float thickness = 0.02f;

    // Construction de la bouche segment par segment
    glBegin(GL_QUADS);
    // Segment gauche
    glVertex3f(-0.12f, 0.02f, thickness);
    glVertex3f(-0.06f, -0.04f, thickness);
    glVertex3f(-0.06f, -0.06f, thickness);
    glVertex3f(-0.12f, 0.0f, thickness);

    glVertex3f(-0.12f, 0.02f, -thickness);
    glVertex3f(-0.12f, 0.0f, -thickness);
    glVertex3f(-0.06f, -0.06f, -thickness);
    glVertex3f(-0.06f, -0.04f, -thickness);

    // Segment central gauche
    glVertex3f(-0.06f, -0.04f, thickness);
    glVertex3f(0.0f, 0.02f, thickness);
    glVertex3f(0.0f, 0.0f, thickness);
    glVertex3f(-0.06f, -0.06f, thickness);

    glVertex3f(-0.06f, -0.04f, -thickness);
    glVertex3f(-0.06f, -0.06f, -thickness);
    glVertex3f(0.0f, 0.0f, -thickness);
    glVertex3f(0.0f, 0.02f, -thickness);

    // Segment central droit
    glVertex3f(0.0f, 0.02f, thickness);
    glVertex3f(0.06f, -0.04f, thickness);
    glVertex3f(0.06f, -0.06f, thickness);
    glVertex3f(0.0f, 0.0f, thickness);

    glVertex3f(0.0f, 0.02f, -thickness);
    glVertex3f(0.0f, 0.0f, -thickness);
    glVertex3f(0.06f, -0.06f, -thickness);
    glVertex3f(0.06f, -0.04f, -thickness);

    // Segment droit
    glVertex3f(0.06f, -0.04f, thickness);
    glVertex3f(0.12f, 0.02f, thickness);
    glVertex3f(0.12f, 0.0f, thickness);
    glVertex3f(0.06f, -0.06f, thickness);

    glVertex3f(0.06f, -0.04f, -thickness);
    glVertex3f(0.06f, -0.06f, -thickness);
    glVertex3f(0.12f, 0.0f, -thickness);
    glVertex3f(0.12f, 0.02f, -thickness);
    glEnd();

    glPopMatrix();
}

// Dessine le nez triangulaire de Pikachu
void Pikachu::dessinerNez() {
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, 1.8f, 0.69f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.05f, 0.02f);
    glVertex3f(-0.04f, -0.03f, 0.02f);
    glVertex3f(0.04f, -0.03f, 0.02f);
    glEnd();
    glPopMatrix();
}
