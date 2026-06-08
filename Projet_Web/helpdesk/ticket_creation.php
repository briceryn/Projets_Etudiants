<?php
declare(strict_types=1);
require_once 'includes/authentification.php';
requireConnexion();

if (estTuteur()) {
    header('Location: tableau_bord.php');
    exit();
}

$erreurs = [];
$donnees = [
    'titre' => '',
    'categorie' => '',
    'priorite' => '',
    'description' => ''
];

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Récupération et nettoyage
    $donnees['titre'] = trim($_POST['titre'] ?? '');
    $donnees['description'] = trim($_POST['description'] ?? '');
    $donnees['categorie'] = $_POST['categorie'] ?? '';
    $donnees['priorite'] = $_POST['priorite'] ?? '';
    
    // Validation
    if (empty($donnees['titre'])) {
        $erreurs['titre'] = 'Le titre est requis';
    } elseif (strlen($donnees['titre']) < 3) {
        $erreurs['titre'] = 'Le titre doit contenir au moins 3 caractères';
    }
    
    if (empty($donnees['description'])) {
        $erreurs['description'] = 'La description est requise';
    } elseif (strlen($donnees['description']) < 10) {
        $erreurs['description'] = 'La description doit contenir au moins 10 caractères';
    }
    
    if (!in_array($donnees['categorie'], ['Cours', 'TD', 'TP'])) {
        $erreurs['categorie'] = 'Catégorie invalide';
    }
    
    if (!in_array($donnees['priorite'], ['Basse', 'Moyenne', 'Haute'])) {
        $erreurs['priorite'] = 'Priorité invalide';
    }
    
    if (empty($erreurs)) {
        $ticket = [
            'id' => getNextTicketId(),
            'auteur_id' => $_SESSION['user_id'],
            'auteur_nom' => $_SESSION['prenom'] . ' ' . $_SESSION['nom'],
            'titre' => $donnees['titre'],
            'description' => $donnees['description'],
            'categorie' => $donnees['categorie'],
            'priorite' => $donnees['priorite'],
            'statut' => 'Ouvert',
            'date_creation' => date('Y-m-d H:i:s')
        ];
        
        if (saveTicket($ticket)) {
            $_SESSION['success'] = 'Ticket créé avec succès !';
            header('Location: tableau_bord.php');
            exit();
        } else {
            $erreurs['general'] = 'Erreur lors de la création du ticket';
        }
    }
}

$pageTitle = 'Nouveau ticket';
include 'includes/en_tete.php';
?>

<h1> Créer un nouveau ticket</h1>

<?php if (!empty($erreurs['general'])): ?>
    <div class="error-message"><?php echo nettoyer($erreurs['general']); ?></div>
<?php endif; ?>

<form method="POST" action="" class="ticket-form" id="ticket-form">
    <div class="form-group <?php echo isset($erreurs['titre']) ? 'has-error' : ''; ?>">
        <label for="titre">Titre:</label>
        <input type="text" 
               id="titre" 
               name="titre" 
               value="<?php echo nettoyer($donnees['titre']); ?>" 
               required
               data-minlength="3">
        <?php if (isset($erreurs['titre'])): ?>
            <div class="error-feedback"><?php echo nettoyer($erreurs['titre']); ?></div>
        <?php endif; ?>
        <div class="character-counter" id="titre-counter">0/50</div>
    </div>
    
    <div class="form-group <?php echo isset($erreurs['categorie']) ? 'has-error' : ''; ?>">
        <label for="categorie">Catégorie:</label>
        <select id="categorie" name="categorie" required>
            <option value="">Sélectionnez une catégorie</option>
            <option value="Cours" <?php echo $donnees['categorie'] === 'Cours' ? 'selected' : ''; ?>> Cours</option>
            <option value="TD" <?php echo $donnees['categorie'] === 'TD' ? 'selected' : ''; ?>> TD</option>
            <option value="TP" <?php echo $donnees['categorie'] === 'TP' ? 'selected' : ''; ?>> TP</option>
        </select>
        <?php if (isset($erreurs['categorie'])): ?>
            <div class="error-feedback"><?php echo nettoyer($erreurs['categorie']); ?></div>
        <?php endif; ?>
    </div>
    
    <div class="form-group <?php echo isset($erreurs['priorite']) ? 'has-error' : ''; ?>">
        <label for="priorite">Priorité:</label>
        <select id="priorite" name="priorite" required>
            <option value="">Sélectionnez une priorité</option>
            <option value="Basse" <?php echo $donnees['priorite'] === 'Basse' ? 'selected' : ''; ?>> Basse</option>
            <option value="Moyenne" <?php echo $donnees['priorite'] === 'Moyenne' ? 'selected' : ''; ?>> Moyenne</option>
            <option value="Haute" <?php echo $donnees['priorite'] === 'Haute' ? 'selected' : ''; ?>> Haute</option>
        </select>
        <?php if (isset($erreurs['priorite'])): ?>
            <div class="error-feedback"><?php echo nettoyer($erreurs['priorite']); ?></div>
        <?php endif; ?>
    </div>
    
    <div class="form-group <?php echo isset($erreurs['description']) ? 'has-error' : ''; ?>">
        <label for="description">Description:</label>
        <textarea id="description" 
                  name="description" 
                  rows="6" 
                  required
                  data-minlength="10"><?php echo nettoyer($donnees['description']); ?></textarea>
        <?php if (isset($erreurs['description'])): ?>
            <div class="error-feedback"><?php echo nettoyer($erreurs['description']); ?></div>
        <?php endif; ?>
        <div class="character-counter" id="description-counter">0/500</div>
    </div>
    
    <div class="form-actions">
        <button type="submit" class="btn-primary">Créer le ticket</button>
        <a href="tableau_bord.php" class="btn-secondary">Annuler</a>
    </div>
</form>

<?php include 'includes/pied_page.php'; ?>