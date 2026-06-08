<?php
declare(strict_types=1);
require_once 'includes/authentification.php';

// Rediriger si déjà connecté
if (estConnecte()) {
    header('Location: tableau_bord.php');
    exit();
}

$erreur = '';
$username = '';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = $_POST['username'] ?? '';
    $password = $_POST['password'] ?? '';
    
    // Validation
    if (empty($username) || empty($password)) {
        $erreur = 'Veuillez remplir tous les champs';
    } else {
        $user = getUtilisateurByUsername($username);
        
        if ($user && password_verify($password, $user['password'])) {
            $_SESSION['user_id'] = $user['id'];
            $_SESSION['username'] = $user['username'];
            $_SESSION['role'] = $user['role'];
            $_SESSION['nom'] = $user['nom'];
            $_SESSION['prenom'] = $user['prenom'];
            
            header('Location: tableau_bord.php');
            exit();
        } else {
            $erreur = 'Identifiants incorrects';
        }
    }
}

$pageTitle = 'Connexion';
include 'includes/en_tete.php';
?>

<div class="login-container">
    <h1> Connexion Helpdesk</h1>
    
    <?php if ($erreur): ?>
        <div class="error-message"><?php echo nettoyer($erreur); ?></div>
    <?php endif; ?>
    
    <form method="POST" action="" id="login-form">
        <div class="form-group">
            <label for="username">Nom d'utilisateur:</label>
            <input type="text" 
                   id="username" 
                   name="username" 
                   value="<?php echo nettoyer($username); ?>" 
                   required>
            <div class="error-feedback" id="username-error"></div>
        </div>
        
        <div class="form-group">
            <label for="password">Mot de passe:</label>
            <input type="password" id="password" name="password" required>
            <div class="error-feedback" id="password-error"></div>
        </div>
        
        <button type="submit" class="btn-primary">Se connecter</button>
    </form>
    
    <div class="login-info">
        <h3>Comptes de démonstration:</h3>
        <p> Étudiant: <strong>etudiant</strong> / <strong>etudiant123</strong></p>
        <p> Tuteur: <strong>tuteur</strong> / <strong>tuteur123</strong></p>
    </div>
</div>

<?php include 'includes/pied_page.php'; ?>