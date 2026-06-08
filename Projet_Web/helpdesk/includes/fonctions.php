<?php
declare(strict_types=1);

/**
 * Lit un fichier JSON et retourne un tableau
 * @param string $file Chemin du fichier
 * @return array
 */
function lireJSON(string $file): array {
    if (!file_exists($file)) {
        return [];
    }
    $content = file_get_contents($file);
    return json_decode($content, true) ?: [];
}

/**
 * Écrit des données dans un fichier JSON
 * @param string $file Chemin du fichier
 * @param array $data Données à écrire
 * @return bool
 */
function ecrireJSON(string $file, array $data): bool {
    return file_put_contents($file, json_encode($data, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE)) !== false;
}

/**
 * Récupère tous les tickets
 * @return array
 */
function getTickets(): array {
    return lireJSON(TICKETS_FILE);
}

/**
 * Récupère un ticket par son ID
 * @param int $id
 * @return array|null
 */
function getTicketById(int $id): ?array {
    $tickets = getTickets();
    foreach ($tickets as $ticket) {
        if ($ticket['id'] === $id) {
            return $ticket;
        }
    }
    return null;
}

/**
 * Sauvegarde un ticket
 * @param array $ticket
 * @return bool
 */
function saveTicket(array $ticket): bool {
    $tickets = getTickets();
    $found = false;
    
    foreach ($tickets as &$t) {
        if ($t['id'] === $ticket['id']) {
            $t = $ticket;
            $found = true;
            break;
        }
    }
    
    if (!$found) {
        $tickets[] = $ticket;
    }
    
    return ecrireJSON(TICKETS_FILE, $tickets);
}

/**
 * Génère le prochain ID de ticket
 * @return int
 */
function getNextTicketId(): int {
    $tickets = getTickets();
    if (empty($tickets)) {
        return 1;
    }
    $ids = array_column($tickets, 'id');
    return max($ids) + 1;
}

/**
 * Récupère les commentaires d'un ticket
 * @param int $ticketId
 * @return array
 */
function getCommentairesByTicketId(int $ticketId): array {
    $comments = lireJSON(COMMENTS_FILE);
    return array_filter($comments, function($comment) use ($ticketId) {
        return $comment['ticket_id'] === $ticketId;
    });
}

/**
 * Ajoute un commentaire
 * @param array $comment
 * @return bool
 */
function addCommentaire(array $comment): bool {
    $comments = lireJSON(COMMENTS_FILE);
    $comments[] = $comment;
    return ecrireJSON(COMMENTS_FILE, $comments);
}

/**
 * Récupère tous les utilisateurs
 * @return array
 */
function getUtilisateurs(): array {
    return lireJSON(USERS_FILE);
}

/**
 * Récupère un utilisateur par son nom d'utilisateur
 * @param string $username
 * @return array|null
 */
function getUtilisateurByUsername(string $username): ?array {
    $users = getUtilisateurs();
    foreach ($users as $user) {
        if ($user['username'] === $username) {
            return $user;
        }
    }
    return null;
}

/**
 * Valide une adresse email
 * @param string $email
 * @return bool
 */
function validerEmail(string $email): bool {
    return filter_var($email, FILTER_VALIDATE_EMAIL) !== false;
}

/**
 * Nettoie une chaîne pour l'affichage HTML
 * @param string $str
 * @return string
 */
function nettoyer(string $str): string {
    return htmlspecialchars($str, ENT_QUOTES, 'UTF-8');
}
?>