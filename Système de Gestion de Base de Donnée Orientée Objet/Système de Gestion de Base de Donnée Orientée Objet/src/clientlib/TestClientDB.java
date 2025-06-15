package clientlib;

import donnees.ObjetBDD;
import donnees.Utilisateur;
import java.util.List;

public class TestClientDB {
    public static void main(String[] args) {
        // Adresse et port du serveur
        String host = "localhost";
        int port = 8080;

        // Créer une instance du client de base de données
        try (ClientDB client = new ClientDB(host, port)) {
            // Connexion au serveur
            client.connect();
            System.out.println("Connexion au serveur réussie.");

            // Authentification
            try {
                client.login("test", "0000");  // Remplacer par des identifiants valides
                System.out.println("Authentification réussie.");
            } catch (AuthException e) {
                System.err.println("Erreur d'authentification : " + e.getMessage());
                return;
            } catch (DBException e) {
                System.err.println("Erreur de base de données : " + e.getMessage());
                return;
            }

            // Créer une nouvelle collection
            try {
                client.createCollection("test");
                System.out.println("Collection 'test' créée.");
            } catch (DBException e) {
                System.err.println("Erreur lors de la création de la collection : " + e.getMessage());
                return;
            }

            // Ajouter un objet à la collection "utilisateurs"
            try {
                // Créer un utilisateur
                Utilisateur utilisateur = new Utilisateur("test1", "pass123");

                // Ajouter l'objet utilisateur à la collection
                client.addObject("utilisateurs", utilisateur);
                System.out.println("Objet 'test1' ajouté à la collection 'utilisateurs'.");
            } catch (DBException e) {
                System.err.println("Erreur lors de l'ajout de l'objet : " + e.getMessage());
                return;
            }

            // Rechercher un objet dans la collection "utilisateurs"
            try {
                List<ObjetBDD> results = client.search("utilisateurs", "test1");

                // Vérification si les résultats sont null ou vides
                if (results != null && !results.isEmpty()) {
                    System.out.println("Résultats de la recherche :");
                    for (ObjetBDD obj : results) {
                        System.out.println(obj);  // Afficher l'objet trouvé
                    }
                } else {
                    System.out.println("Aucun résultat trouvé.");
                }
            } catch (DBException e) {
                System.err.println("Erreur lors de la recherche : " + e.getMessage());
            }

            // Déconnexion
            try {
                client.logout();
                System.out.println("Déconnexion réussie.");
            } catch (DBException e) {
                System.err.println("Erreur lors de la déconnexion : " + e.getMessage());
            }
        } catch (DBException e) {
            System.err.println("Erreur lors de la connexion au serveur : " + e.getMessage());
        }
    }
}