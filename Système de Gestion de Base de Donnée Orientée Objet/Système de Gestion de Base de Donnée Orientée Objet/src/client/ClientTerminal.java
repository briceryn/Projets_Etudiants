package client;

import donnees.ObjetBDD;
import reseaux.Message;
import donnees.Utilisateur;

import java.io.*;
import java.net.Socket;
import java.util.List;
import java.util.Map;
import java.util.Scanner;

/**
 * ClientTerminal est une interface en ligne de commande permettant
 * d'interagir avec le serveur via des messages sérialisés.
 *
 * <p>Il permet de se connecter au serveur, d'envoyer des requêtes,
 * de recevoir les réponses, et de gérer des collections et utilisateurs.</p>
 */
public class ClientTerminal {
    private static Socket socket;
    private static ObjectOutputStream oos;
    private static String sessionId = null;

    /**
     * Point d'entrée principal du client terminal.
     */
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        System.out.println("Client Terminal - ? pour l'aide");

        while (true) {
            System.out.print("> ");
            String[] cmd = sc.nextLine().split(" ");

            try {
                switch (cmd[0].toUpperCase()) {
                    case "CONNECT":
                        connecter(cmd[1], Integer.parseInt(cmd[2]));
                        break;
                    case "CREATE_COLLECTION":
                        envoyerRequete(new Message("CREATE_COLLECTION", Map.of("nom", cmd[1])));
                        break;
                    case "DELETE_COLLECTION":
                        if (cmd.length < 2) {
                            System.out.println("Usage: DELETE_COLLECTION <nom_collection>");
                            break;
                        }
                        envoyerRequete(new Message("DELETE_COLLECTION", Map.of("nom", cmd[1])));
                        break;
                    case "LIST":
                        envoyerRequete(new Message("LIST_COLLECTIONS", Map.of()));
                        break;
                    case "CREATE_USER":
                        envoyerRequete(new Message("ADD_OBJECT", Map.of(
                                "collection", "utilisateurs",
                                "objet", new Utilisateur(cmd[1], cmd[2])
                        )));
                        break;
                    case "LOGIN":
                        if (cmd.length < 3) {
                            System.out.println("Usage: LOGIN <username> <password>");
                            break;
                        }
                        envoyerRequete(new Message("LOGIN", Map.of(
                                "username", cmd[1],
                                "password", cmd[2]
                        )));
                        break;
                    case "SEARCH":
                        if (cmd.length < 3) {
                            System.out.println("Usage: SEARCH <collection> <critere>");
                            break;
                        }
                        envoyerRequete(new Message("SEARCH", Map.of(
                                "collection", cmd[1],
                                "critere", cmd[2]
                        )));
                        break;
                    case "LOGOUT":
                        if (sessionId == null) {
                            System.out.println("Non authentifié");
                            break;
                        }
                        envoyerRequete(new Message("LOGOUT", Map.of()));
                        sessionId = null;
                        System.out.println("Déconnecté");
                        break;
                    case "EXIT":
                        deconnecter();
                        System.exit(0);
                    default:
                        System.out.println("Commande inconnue");
                }
            } catch (Exception e) {
                System.out.println("Erreur: " + e.getMessage());
            }
        }
    }

    /**
     * Établit la connexion avec le serveur.
     *
     * @param hote adresse IP ou nom de domaine du serveur
     * @param port port d'écoute du serveur
     * @throws IOException si la connexion échoue
     */
    private static void connecter(String hote, int port) throws IOException {
        if (socket != null && !socket.isClosed()) {
            System.out.println("Déjà connecté");
            return;
        }
        socket = new Socket(hote, port);
        oos = new ObjectOutputStream(socket.getOutputStream());
        new Thread(ClientTerminal::ecouterReponses).start();
        System.out.println("Connecté à " + hote + ":" + port);
    }

    /**
     * Envoie une requête au serveur, ajoutant l'ID de session si nécessaire.
     *
     * @param requete message à envoyer
     * @throws IOException si l'envoi échoue
     */
    private static void envoyerRequete(Message requete) throws IOException {
        if (oos == null) throw new IOException("Non connecté");
        if (!requete.getType().equals("LOGIN") && sessionId != null) {
            requete.setSessionId(sessionId);
        }
        oos.writeObject(requete);
        oos.flush();
    }

    /**
     * Écoute et affiche les réponses du serveur.
     */
    private static void ecouterReponses() {
        try (ObjectInputStream ois = new ObjectInputStream(socket.getInputStream())) {
            while (true) {
                Message reponse = (Message) ois.readObject();

                if (reponse.getType().equals("OK") && reponse.getContenu().containsKey("resultats")) {
                    String message = (String) reponse.getContenu().getOrDefault("message", "");
                    List<ObjetBDD> resultats = (List<ObjetBDD>) reponse.getContenu().get("resultats");

                    System.out.println("[SERVER] " + message);
                    if (!resultats.isEmpty()) {
                        System.out.println("Résultats :");
                        resultats.forEach(obj -> System.out.println(" - " + obj));
                    }
                } else if (reponse.getType().equals("OK") && reponse.getContenu().containsKey("sessionId")) {
                    sessionId = (String) reponse.getContenu().get("sessionId");
                    System.out.println("Authentifié. Session ID : " + sessionId);
                } else if (reponse.getType().equals("OK") && reponse.getContenu().containsKey("collections")) {
                    List<String> collections = (List<String>) reponse.getContenu().get("collections");
                    int count = (int) reponse.getContenu().get("count");

                    System.out.println("\nCollections disponibles (" + count + ") :");
                    collections.forEach(col -> System.out.println(" - " + col));
                } else {
                    System.out.println("[SERVER] " + reponse);
                }
            }
        } catch (IOException | ClassNotFoundException e) {
            System.err.println("Déconnecté: " + e.getMessage());
        }
    }

    /**
     * Ferme proprement la connexion avec le serveur.
     *
     * @throws IOException si une erreur survient lors de la fermeture
     */
    private static void deconnecter() throws IOException {
        if (oos != null) oos.close();
        if (socket != null) socket.close();
    }
}