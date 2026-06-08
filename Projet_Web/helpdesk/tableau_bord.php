<?php
declare(strict_types=1);
require_once 'includes/authentification.php';
requireConnexion();

$tickets = getTickets();

// Filtrer les tickets pour les étudiants
if (estEtudiant()) {
    $tickets = array_filter($tickets, function($ticket) {
        return $ticket['auteur_id'] === $_SESSION['user_id'];
    });
}

// Trier par date de création (plus récent d'abord)
usort($tickets, function($a, $b) {
    return strtotime($b['date_creation']) - strtotime($a['date_creation']);
});

$statistiques = [
    'total' => count($tickets),
    'ouvert' => count(array_filter($tickets, fn($t) => $t['statut'] === 'Ouvert')),
    'encours' => count(array_filter($tickets, fn($t) => $t['statut'] === 'En cours')),
    'resolu' => count(array_filter($tickets, fn($t) => $t['statut'] === 'Résolu'))
];

$pageTitle = 'Tableau de bord';
include 'includes/en_tete.php';
?>

<h1> Tableau de bord</h1>

<!-- Statistiques -->
<div class="stats-container">
    <div class="stat-card total">
        <span class="stat-value"><?php echo $statistiques['total']; ?></span>
        <span class="stat-label">Total tickets</span>
    </div>
    <div class="stat-card ouvert">
        <span class="stat-value"><?php echo $statistiques['ouvert']; ?></span>
        <span class="stat-label">Ouverts</span>
    </div>
    <div class="stat-card encours">
        <span class="stat-value"><?php echo $statistiques['encours']; ?></span>
        <span class="stat-label">En cours</span>
    </div>
    <div class="stat-card resolu">
        <span class="stat-value"><?php echo $statistiques['resolu']; ?></span>
        <span class="stat-label">Résolus</span>
    </div>
</div>

<!-- Filtres -->
<div class="filters-container">
    <input type="text" id="search-tickets" placeholder=" Rechercher un ticket...">
    <select id="filter-statut">
        <option value="tous">Tous les statuts</option>
        <option value="Ouvert">Ouvert</option>
        <option value="En cours">En cours</option>
        <option value="Résolu">Résolu</option>
    </select>
    <select id="filter-priorite">
        <option value="tous">Toutes les priorités</option>
        <option value="Haute">Haute</option>
        <option value="Moyenne">Moyenne</option>
        <option value="Basse">Basse</option>
    </select>
</div>

<div class="tickets-list">
    <h2>Liste des tickets</h2>
    
    <?php if (empty($tickets)): ?>
        <p class="no-data">Aucun ticket trouvé.</p>
    <?php else: ?>
        <table id="tickets-table">
            <thead>
                <tr>
                    <th>ID</th>
                    <th>Titre</th>
                    <th>Catégorie</th>
                    <th>Priorité</th>
                    <th>Statut</th>
                    <th>Date</th>
                    <th>Actions</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($tickets as $ticket): 
                    $statutClass = strtolower(str_replace(' ', '', $ticket['statut']));
                    $prioriteClass = strtolower($ticket['priorite']);
                ?>
                    <tr class="ticket-row <?php echo $statutClass; ?>" 
                        data-statut="<?php echo nettoyer($ticket['statut']); ?>"
                        data-priorite="<?php echo nettoyer($ticket['priorite']); ?>"
                        data-titre="<?php echo nettoyer($ticket['titre']); ?>">
                        <td>#<?php echo $ticket['id']; ?></td>
                        <td><?php echo nettoyer($ticket['titre']); ?></td>
                        <td><?php echo nettoyer($ticket['categorie']); ?></td>
                        <td class="priorite-<?php echo $prioriteClass; ?>">
                            <?php echo nettoyer($ticket['priorite']); ?>
                        </td>
                        <td class="statut-<?php echo $statutClass; ?>">
                            <?php echo nettoyer($ticket['statut']); ?>
                        </td>
                        <td><?php echo date('d/m/Y H:i', strtotime($ticket['date_creation'])); ?></td>
                        <td>
                            <a href="ticket.php?id=<?php echo $ticket['id']; ?>" class="btn-view"> Voir</a>
                        </td>
                    </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
    <?php endif; ?>
</div>

<?php include 'includes/pied_page.php'; ?>