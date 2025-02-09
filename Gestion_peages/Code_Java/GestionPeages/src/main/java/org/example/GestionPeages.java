package org.example;

import java.sql.*;
import java.util.Scanner;

public class GestionPeages {

    private static final String URL = "jdbc:postgresql://kafka.iem/rl646187";
    private static final String USER = "rl646187";
    private static final String PASSWORD = "rl646187";

    public static void main(String[] args) {
        try (Connection connection = DriverManager.getConnection(URL, USER, PASSWORD)) {
            System.out.println("Connexion réussie à la base de données.");

            Scanner scanner = new Scanner(System.in);
            boolean continuer = true;

            while (continuer) {
                System.out.println("\nOptions disponibles :");
                System.out.println("1. Afficher les péages les plus empruntés");
                System.out.println("2. Liste des trajets d'un utilisateur abonné");
                System.out.println("3. Automobilistes non abonnés avec trajets répétés");
                System.out.println("4. Portions d’autoroute les plus empruntées");
                System.out.println("5. Automobilistes avec badge et abonnement expiré");
                System.out.println("6. Quitter");
                System.out.print("Votre choix : ");

                int choix = scanner.nextInt();
                scanner.nextLine(); // Consommer le retour à la ligne

                switch (choix) {
                    case 1:
                        afficherPeagesLesPlusEmpruntes(connection);
                        break;
                    case 2:
                        System.out.print("Entrez l'ID de l'utilisateur abonné : ");
                        int idUtilisateur = scanner.nextInt();
                        afficherTrajetsUtilisateurAbonne(connection, idUtilisateur);
                        break;
                    case 3:
                        detecterAutomobilistesNonAbonnes(connection);
                        break;
                    case 4:
                        afficherPortionsLesPlusEmpruntees(connection);
                        break;
                    case 5:
                        afficherAutomobilistesAvecBadgeEtAbonnementExpire(connection);
                        break;
                    case 6:
                        continuer = false;
                        System.out.println("Fermeture du programme.");
                        break;
                    default:
                        System.out.println("Choix invalide, veuillez réessayer.");
                }
            }
        } catch (SQLException e) {
            System.err.println("Erreur lors de la connexion ou de l'exécution : " + e.getMessage());
        }
    }

    private static void afficherPeagesLesPlusEmpruntes(Connection connection) {
        String query = """
            SELECT localisation, nombre_passages
            FROM Gestion_peages.Peage
            ORDER BY nombre_passages DESC
            LIMIT 10;
        """;
        try (Statement statement = connection.createStatement();
             ResultSet resultSet = statement.executeQuery(query)) {

            System.out.println("\nPéages les plus empruntés :");
            while (resultSet.next()) {
                System.out.println("Localisation : " + resultSet.getString("localisation") +
                        ", Nombre de passages : " + resultSet.getInt("nombre_passages"));
            }
        } catch (SQLException e) {
            System.err.println("Erreur lors de la requête : " + e.getMessage());
        }
    }

    private static void afficherTrajetsUtilisateurAbonne(Connection connection, int idUtilisateur) {
        String query = """
        SELECT t.date_entree, p1.localisation AS peage_entree, p2.localisation AS peage_sortie, t.montant
        FROM Gestion_peages.Ticket t
        JOIN Gestion_peages.Peage p1 ON t.peage_entree = p1.id
        JOIN Gestion_peages.Peage p2 ON t.peage_sortie = p2.id
        WHERE t.id_automobiliste = ?;
    """;
        try (PreparedStatement preparedStatement = connection.prepareStatement(query)) {
            preparedStatement.setInt(1, idUtilisateur);

            try (ResultSet resultSet = preparedStatement.executeQuery()) {
                System.out.println("\nTrajets de l'utilisateur abonné :");
                if (!resultSet.isBeforeFirst()) { // Si aucun résultat
                    System.out.println("Aucun trajet trouvé pour cet utilisateur.");
                } else {
                    while (resultSet.next()) {
                        System.out.println("Date d'entrée : " + resultSet.getTimestamp("date_entree") +
                                ", Péage d'entrée : " + resultSet.getString("peage_entree") +
                                ", Péage de sortie : " + resultSet.getString("peage_sortie") +
                                ", Montant : " + resultSet.getDouble("montant"));
                    }
                }
            }
        } catch (SQLException e) {
            System.err.println("Erreur lors de la requête : " + e.getMessage());
        }
    }


    private static void detecterAutomobilistesNonAbonnes(Connection connection) {
        String query = """
            SELECT a.nom, a.prenom, COUNT(*) AS nb_trajets
            FROM Gestion_peages.Ticket t
            JOIN Gestion_peages.Automobiliste a ON t.id_automobiliste = a.id
            WHERE a.statut_abo = 'non-abonne'
            GROUP BY a.id, a.nom, a.prenom
            HAVING COUNT(*) > 1;
        """;
        try (Statement statement = connection.createStatement();
             ResultSet resultSet = statement.executeQuery(query)) {

            System.out.println("\nAutomobilistes non abonnés avec trajets répétés :");
            while (resultSet.next()) {
                System.out.println("Nom : " + resultSet.getString("nom") +
                        ", Prénom : " + resultSet.getString("prenom") +
                        ", Nombre de trajets : " + resultSet.getInt("nb_trajets"));
            }
        } catch (SQLException e) {
            System.err.println("Erreur lors de la requête : " + e.getMessage());
        }
    }

    private static void afficherPortionsLesPlusEmpruntees(Connection connection) {
        String query = """
            SELECT p1.localisation AS peage_entree, p2.localisation AS peage_sortie, COUNT(*) AS nb_passages
            FROM Gestion_peages.Ticket t
            JOIN Gestion_peages.Peage p1 ON t.peage_entree = p1.id
            JOIN Gestion_peages.Peage p2 ON t.peage_sortie = p2.id
            GROUP BY p1.localisation, p2.localisation
            ORDER BY nb_passages DESC
            LIMIT 10;
        """;
        try (Statement statement = connection.createStatement();
             ResultSet resultSet = statement.executeQuery(query)) {

            System.out.println("\nPortions d’autoroute les plus empruntées :");
            while (resultSet.next()) {
                System.out.println("Péage d'entrée : " + resultSet.getString("peage_entree") +
                        ", Péage de sortie : " + resultSet.getString("peage_sortie") +
                        ", Nombre de passages : " + resultSet.getInt("nb_passages"));
            }
        } catch (SQLException e) {
            System.err.println("Erreur lors de la requête : " + e.getMessage());
        }
    }

    private static void afficherAutomobilistesAvecBadgeEtAbonnementExpire(Connection connection) {
        String query = """
            SELECT a.nom, a.prenom, b.id AS badge_id
            FROM Gestion_peages.Abonnement ab
            JOIN Gestion_peages.Automobiliste a ON ab.id = a.id
            JOIN Gestion_peages.Badge b ON b.id_automobiliste = a.id
            WHERE ab.date_fin < CURRENT_DATE AND b.actif = TRUE;
        """;
        try (Statement statement = connection.createStatement();
             ResultSet resultSet = statement.executeQuery(query)) {

            System.out.println("\nAutomobilistes avec abonnement expiré et badge actif :");
            while (resultSet.next()) {
                System.out.println("Nom : " + resultSet.getString("nom") +
                        ", Prénom : " + resultSet.getString("prenom") +
                        ", ID Badge : " + resultSet.getInt("badge_id"));
            }
        } catch (SQLException e) {
            System.err.println("Erreur lors de la requête : " + e.getMessage());
        }
    }
}