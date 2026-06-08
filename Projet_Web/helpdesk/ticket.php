<?php
declare(strict_types=1);
require_once 'includes/authentification.php';
requireConnexion();

// Vérifier que l'ID est présent
if (!isset($_GET['id']) || !is_numeric($_GET['id'])) {
    header('Location: tableau_bord.php');
    exit();
}

$ticketId = (int)$_GET['id'];
$ticket = getTicketById($ticketId);

// Vérifier que le ticket existe
if (!$ticket) {
    header('Location: tableau_bord.php');
    exit();
}

// Vérifier les droits d'accès
if (estEtudiant() && $ticket['auteur_id'] !== $_SESSION['user_id']) {
    header('Location: tableau_bord.php');
    exit();
}

// Traitement des commentaires
if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['commentaire'])) {
    $message = trim($_POST['commentaire']);
    
    if (!empty($message)) {
        $comment = [
            'id' => uniqid(),
            'ticket_id' => $ticketId,
            'auteur_id' => $_SESSION['user_id'],
            'auteur_nom' => $_SESSION['prenom'] . ' ' . $_SESSION['nom'],
            'message' => $message,
            'date' => date('Y-m-d H:i:s')
        ];
        
        addCommentaire($comment);
        
        header('Location: ticket.php?id=' . $ticketId . '#comments');
        exit();
    }
}

// Traitement de la mise à jour du statut (tuteur seulement)
if (estTuteur() && isset($_POST['update_statut'])) {
    $nouveauStatut = $_POST['statut'] ?? '';
    
    if (in_array($nouveauStatut, ['Ouvert', 'En cours', 'Résolu'])) {
        $ticket['statut'] = $nouveauStatut;
        saveTicket($ticket);
        
        header('Location: ticket.php?id=' . $ticketId);
        exit();
    }
}

$commentaires = getCommentairesByTicketId($ticketId);

$pageTitle = 'Ticket #' . $ticketId;
include 'includes/en_tete.php';
?>

<div class="ticket-detail">
    <div class="ticket-header">
        <h1>Ticket #<?php echo $ticketId; ?>: <?php echo nettoyer($ticket['titre']); ?></h1>
        <span class="statut-badge statut-<?php echo strtolower(str_replace(' ', '', $ticket['statut'])); ?>">
            <?php echo nettoyer($ticket['statut']); ?>
        </span>
    </div>
    
    <div class="ticket-meta">
        <div class="meta-item">
            <span class="meta-label">👤 Auteur:</span>
            <span class="meta-value"><?php echo nettoyer($ticket['auteur_nom']); ?></span>
        </div>
        <div class="meta-item">
            <span class="meta-label">📚 Catégorie:</span>
            <span class="meta-value"><?php echo nettoyer($ticket['categorie']); ?></span>
        </div>
        <div class="meta-item">
            <span class="meta-label">⚡ Priorité:</span>
            <span class="meta-value priorite-<?php echo strtolower($ticket['priorite']); ?>">
                <?php echo nettoyer($ticket['priorite']); ?>
            </span>
        </div>
        <div class="meta-item">
            <span class="meta-label">📅 Créé le:</span>
            <span class="meta-value"><?php echo date('d/m/Y à H:i', strtotime($ticket['date_creation'])); ?></span>
        </div>
    </div>
    
    <div class="ticket-description">
        <h3>Description</h3>
        <div class="description-content">
            <?php echo nl2br(nettoyer($ticket['description'])); ?>
        </div>
    </div>
    
    <?php if (estTuteur()): ?>
        <div class="update-statut-section">
            <h3>Mettre à jour le statut</h3>
            <form method="POST" action="" class="update-statut-form" id="update-statut-form">
                <select name="statut" id="statut-select">
                    <option value="Ouvert" <?php echo $ticket['statut'] === 'Ouvert' ? 'selected' : ''; ?>>📌 Ouvert</option>
                    <option value="En cours" <?php echo $ticket['statut'] === 'En cours' ? 'selected' : ''; ?>>⚙️ En cours</option>
                    <option value="Résolu" <?php echo $ticket['statut'] === 'Résolu' ? 'selected' : ''; ?>>✅ Résolu</option>
                </select>
                <button type="submit" name="update_statut" class="btn-primary">Mettre à jour</button>
            </form>
        </div>
    <?php endif; ?>
    
    <div class="comments-section" id="comments">
        <h3>💬 Commentaires (<?php echo count($commentaires); ?>)</h3>
        
        <div class="comments-list" id="comments-list">
            <?php if (empty($commentaires)): ?>
                <p class="no-comments">Aucun commentaire pour le moment.</p>
            <?php else: ?>
                <?php foreach ($commentaires as $comment): ?>
                    <div class="comment">
                        <div class="comment-header">
                            <strong><?php echo nettoyer($comment['auteur_nom']); ?></strong>
                            <span class="comment-date"><?php echo date('d/m/Y H:i', strtotime($comment['date'])); ?></span>
                        </div>
                        <div class="comment-content">
                            <?php echo nl2br(nettoyer($comment['message'])); ?>
                        </div>
                    </div>
                <?php endforeach; ?>
            <?php endif; ?>
        </div>
        
        <div class="add-comment">
            <h4>Ajouter un commentaire</h4>
            <form method="POST" action="" id="comment-form">
                <div class="form-group">
                    <textarea name="commentaire" 
                              id="commentaire" 
                              rows="4" 
                              placeholder="Votre commentaire..."
                              required></textarea>
                    <div class="character-counter" id="comment-counter">0/500</div>
                </div>
                <button type="submit" class="btn-primary">Envoyer</button>
            </form>
        </div>
    </div>
    
    <div class="actions">
        <a href="tableau_bord.php" class="btn-secondary">← Retour à la liste</a>
    </div>
</div>

<?php include 'includes/pied_page.php'; ?>