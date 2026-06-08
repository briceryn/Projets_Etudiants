<?php
declare(strict_types=1);

// Configuration pour le stockage JSON
define('DATA_DIR', __DIR__ . '/../donnees/');
define('TICKETS_FILE', DATA_DIR . 'tickets.json');
define('COMMENTS_FILE', DATA_DIR . 'commentaires.json');
define('USERS_FILE', DATA_DIR . 'utilisateurs.json');

// Créer le dossier donnees s'il n'existe pas
if (!file_exists(DATA_DIR)) {
    mkdir(DATA_DIR, 0777, true);
}

// Initialiser les fichiers JSON s'ils n'existent pas
$files = [TICKETS_FILE, COMMENTS_FILE, USERS_FILE];
foreach ($files as $file) {
    if (!file_exists($file)) {
        file_put_contents($file, json_encode([]));
    }
}
?>