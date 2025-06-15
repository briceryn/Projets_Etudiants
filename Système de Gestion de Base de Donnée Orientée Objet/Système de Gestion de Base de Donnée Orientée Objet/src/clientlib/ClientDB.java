package clientlib;

import reseaux.Message;
import donnees.ObjetBDD;

import java.io.*;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Client réutilisable pour interagir avec le serveur de base de données.
 * Exemple d'utilisation :
 * <pre>
 * ClientDB client = new ClientDB("localhost", 8080);
 * client.login("admin", "password");
 * client.createCollection("utilisateurs");
 * client.addObject("utilisateurs", new Utilisateur("alice", "pass123"));
 * List<ObjetBDD> results = client.search("utilisateurs", "nomUtilisateur", "alice");
 * </pre>
 */
public class ClientDB implements AutoCloseable {
    private final String host;
    private final int port;
    private Socket socket;
    private ObjectOutputStream oos;
    private ObjectInputStream ois;
    private String sessionId;

    public ClientDB(String host, int port) {
        this.host = host;
        this.port = port;
    }

    // --- Connexion/Déconnexion ---
    public void connect() throws DBException {
        try {
            this.socket = new Socket(host, port);
            this.oos = new ObjectOutputStream(socket.getOutputStream());
            this.ois = new ObjectInputStream(socket.getInputStream());
        } catch (IOException e) {
            throw new DBException("Échec de la connexion au serveur", e);
        }
    }

    @Override
    public void close() throws DBException {
        try {
            if (sessionId != null) {
                logout();
            }
            if (ois != null) ois.close();
            if (oos != null) oos.close();
            if (socket != null) socket.close();
        } catch (IOException e) {
            throw new DBException("Erreur lors de la fermeture", e);
        }
    }

    // --- Authentification ---
    public void login(String username, String password) throws AuthException, DBException {
        try {
            Map<String, Object> content = new HashMap<>();
            content.put("username", username);
            content.put("password", password);

            Message response = sendRequest("LOGIN", content);
            if (!response.getType().equals("OK")) {
                throw new AuthException((String) response.getContenu().get("message"));
            }

            this.sessionId = (String) response.getContenu().get("sessionId");
        } catch (IOException e) {
            throw new DBException("Erreur réseau lors de l'authentification", e);
        }
    }

    public void logout() throws DBException {
        try {
            if (sessionId != null) {
                sendRequest("LOGOUT", new HashMap<>());
                this.sessionId = null;
            }
        } catch (IOException e) {
            throw new DBException("Erreur lors de la déconnexion", e);
        }
    }

    // --- Opérations CRUD ---
    public void createCollection(String name) throws DBException {
        try {
            Map<String, Object> content = new HashMap<>();
            content.put("nom", name);
            sendRequest("CREATE_COLLECTION", content);
        } catch (IOException e) {
            throw new DBException("Échec de la création de collection", e);
        }
    }

    public void addObject(String collectionName, ObjetBDD object) throws DBException {
        try {
            Map<String, Object> content = new HashMap<>();
            content.put("collection", collectionName);
            content.put("objet", object);
            sendRequest("ADD_OBJECT", content);
        } catch (IOException e) {
            throw new DBException("Échec de l'ajout d'objet", e);
        }
    }

    // --- Recherche d'un objet dans une collection ---
    @SuppressWarnings("unchecked")
    public List<ObjetBDD> search(String collectionName, String critere) throws DBException {
        try {
            // Préparer le contenu de la requête
            Map<String, Object> content = new HashMap<>();
            content.put("collection", collectionName);  // Nom de la collection
            content.put("critere", critere);  // Critère de recherche

            // Envoi de la requête au serveur
            Message response = sendRequest("SEARCH", content);

            // Récupération des résultats du serveur
            String message = (String) response.getContenu().get("message");
            List<ObjetBDD> resultats = (List<ObjetBDD>) response.getContenu().get("resultats");

            // Affichage du message de résultat de recherche
            System.out.println(message);  // Afficher le message retourné par le serveur

            // Si des résultats sont trouvés, on les retourne, sinon on renvoie une liste vide
            return resultats != null ? resultats : new ArrayList<>();
        } catch (IOException e) {
            throw new DBException("Échec de la recherche", e);
        }
    }

    // --- Méthode interne pour envoyer des requêtes ---
    private Message sendRequest(String type, Map<String, Object> content) throws IOException {
        Message request = new Message(type, content, sessionId);
        oos.writeObject(request);
        oos.flush();

        try {
            return (Message) ois.readObject();
        } catch (ClassNotFoundException e) {
            throw new IOException("Format de réponse invalide", e);
        }
    }
}