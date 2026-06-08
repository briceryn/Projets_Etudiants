<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Helpdesk - <?php echo $pageTitle ?? 'Accueil'; ?></title>
    <!-- jQuery CDN -->
    <script src="https://code.jquery.com/jquery-3.7.1.min.js"></script>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <header>
        <nav>
            <div class="nav-container">
                <a href="tableau_bord.php" class="logo"> Helpdesk</a>
                <div class="nav-links">
                    <?php if (estConnecte()): ?>
                        <span> <?php echo nettoyer($_SESSION['prenom'] . ' ' . $_SESSION['nom']); ?></span>
                        <a href="tableau_bord.php"> Tableau de bord</a>
                        <?php if (estEtudiant()): ?>
                            <a href="ticket_creation.php"> Nouveau ticket</a>
                        <?php endif; ?>
                        <a href="deconnexion.php"> Déconnexion</a>
                    <?php endif; ?>
                </div>
            </div>
        </nav>
    </header>
    <main>