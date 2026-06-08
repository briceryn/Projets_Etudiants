<?php
declare(strict_types=1);

session_start();
require_once __DIR__ . '/../config/base_donnees.php';
require_once 'fonctions.php';

/**
 * Vérifie si l'utilisateur est connecté
 * @return bool
 */
function estConnecte(): bool {
    return isset($_SESSION['user_id']);
}

/**
 * Vérifie si l'utilisateur est un tuteur
 * @return bool
 */
function estTuteur(): bool {
    return isset($_SESSION['role']) && $_SESSION['role'] === 'tuteur';
}

/**
 * Vérifie si l'utilisateur est un étudiant
 * @return bool
 */
function estEtudiant(): bool {
    return isset($_SESSION['role']) && $_SESSION['role'] === 'etudiant';
}

/**
 * Redirige vers la page de connexion si non connecté
 * @return void
 */
function requireConnexion(): void {
    if (!estConnecte()) {
        header('Location: connexion.php');
        exit();
    }
}

/**
 * Redirige si l'utilisateur n'est pas tuteur
 * @return void
 */
function requireTuteur(): void {
    requireConnexion();
    if (!estTuteur()) {
        header('Location: tableau_bord.php');
        exit();
    }
}

/**
 * Initialise les utilisateurs par défaut
 * @return void
 */
function initUtilisateursParDefaut(): void {
    $users = getUtilisateurs();
    if (empty($users)) {
        $defaultUsers = [
            [
                'id' => 1,
                'username' => 'etudiant',
                'password' => password_hash('etudiant123', PASSWORD_DEFAULT),
                'role' => 'etudiant',
                'nom' => 'Dupont',
                'prenom' => 'Jean',
                'email' => 'jean.dupont@etudiant.fr'
            ],
            [
                'id' => 2,
                'username' => 'tuteur',
                'password' => password_hash('tuteur123', PASSWORD_DEFAULT),
                'role' => 'tuteur',
                'nom' => 'Martin',
                'prenom' => 'Pierre',
                'email' => 'pierre.martin@tuteur.fr'
            ]
        ];
        ecrireJSON(USERS_FILE, $defaultUsers);
    }
}

// Initialiser les utilisateurs par défaut
initUtilisateursParDefaut();
?>