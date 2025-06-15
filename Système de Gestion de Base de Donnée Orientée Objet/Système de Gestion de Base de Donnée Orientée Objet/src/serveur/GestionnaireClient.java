package serveur;

import donnees.Collection;
import donnees.ObjetBDD;
import donnees.Utilisateur;
import reseaux.Message;
import transactions.GestionnaireAuth;

import java.io.*;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;

import serveur.Serveur;

/**
 * Cette classe gère la communication avec un client. Elle hérite de la classe Thread
 * et permet d'exécuter un processus pour chaque client connecté via un socket.
 */
public class GestionnaireClient extends Thread {
    private final Socket socket;  // Socket de communication avec le client
    private ObjectOutputStream oos;  // Flux de sortie pour envoyer des objets au client

    /**
     * Constructeur de la classe, initialisant le socket de communication.
     * @param socket Le socket permettant la communication avec le client
     */
    public GestionnaireClient(Socket socket) {
        this.socket = socket;
    }

    /**
     * Méthode run() qui sera exécutée dans un thread, permettant la gestion de la communication avec le client.
     */
    @Override
    public void run() {
        try (
                ObjectInputStream ois = new ObjectInputStream(socket.getInputStream())  // Flux pour recevoir les objets
        ) {
            this.oos = new ObjectOutputStream(socket.getOutputStream());  // Flux pour envoyer des objets au client

            // Boucle pour traiter plusieurs requêtes
            while (!socket.isClosed()) {
                Message requete = (Message) ois.readObject();  // Lecture de la requête envoyée par le client
                Message reponse = traiterRequete(requete);  // Traitement de la requête
                envoyerReponse(reponse);  // Envoi de la réponse au client
            }
        }
        catch (IOException | ClassNotFoundException e) {
            System.err.println("Client déconnecté : " + e.getMessage());  // Gestion des exceptions liées à la communication
        }
        finally {
            fermerConnexion();  // Fermeture de la connexion après traitement
        }
    }

    /**
     * Méthode pour traiter les requêtes reçues du client.
     * @param requete La requête envoyée par le client
     * @return La réponse à envoyer au client
     */
    private Message traiterRequete(Message requete) {
        try {
            // Vérification de la session pour toutes les commandes sauf LOGIN, CONNECT, et ADD_OBJECT
            if (!requete.getType().equals("LOGIN") && !requete.getType().equals("CONNECT") &&
                    !requete.getType().equals("ADD_OBJECT")) {
                String sessionId = requete.getSessionId();  // Récupération de l'identifiant de session
                if (sessionId == null || !GestionnaireAuth.validerSession(sessionId)) {
                    return new Message("ERREUR", Map.of("message", "Session invalide. Authentifiez-vous avec LOGIN."));  // Retourner un message d'erreur si la session est invalide
                }
            }

            // Traitement des différentes commandes en fonction de leur type
            switch (requete.getType().toUpperCase()) {
                case "CREATE_COLLECTION":
                    return creerCollection(requete.getContenu());  // Création d'une nouvelle collection

                case "DELETE_COLLECTION": {
                    // Suppression d'une collection
                    String nomCollection = (String) requete.getContenu().get("nom");

                    // Vérification pour interdire la suppression de la collection "utilisateurs"
                    if ("utilisateurs".equalsIgnoreCase(nomCollection)) {
                        return new Message("ERREUR", Map.of("message", "Suppression de 'utilisateurs' interdite"));
                    }

                    // Suppression de la collection en mémoire et du fichier correspondant
                    Serveur.collections.remove(nomCollection);
                    String chemin = "collections/" + nomCollection + ".ser";  // Chemin du fichier à supprimer
                    try {
                        Files.deleteIfExists(Paths.get(chemin));  // Supprimer le fichier de la collection
                        return new Message("OK", Map.of("message", "Collection '" + nomCollection + "' supprimée définitivement"));
                    } catch (IOException e) {
                        return new Message("ERREUR", Map.of("message", "Erreur fichier : " + e.getMessage()));  // En cas d'erreur de suppression du fichier
                    }
                }
                case "LIST_COLLECTIONS": {
                    // Lister toutes les collections
                    Set<String> collections = Serveur.collections.keySet();  // Récupérer toutes les clés des collections
                    return new Message("OK", Map.of(
                            "collections", new ArrayList<>(collections),
                            "count", collections.size()  // Nombre de collections
                    ));
                }

                case "ADD_OBJECT":
                    return ajouterObjet(requete.getContenu());  // Ajouter un objet à une collection

                case "LOGIN":
                    // Traitement de la connexion d'un utilisateur
                    Collection<ObjetBDD> collectionUtilisateurs = Serveur.chargerCollection("utilisateurs");
                    if (collectionUtilisateurs == null) {
                        return new Message("ERREUR", Map.of("message", "Collection introuvable"));  // Si la collection "utilisateurs" n'existe pas
                    }

                    String username = (String) requete.getContenu().get("username");  // Récupération du nom d'utilisateur
                    String password = (String) requete.getContenu().get("password");  // Récupération du mot de passe

                    // Vérification des identifiants de l'utilisateur
                    for (ObjetBDD obj : collectionUtilisateurs.getTous()) {
                        Utilisateur user = (Utilisateur) obj;
                        if (user.getNomUtilisateur().equals(username)) {
                            if (user.getMotDePasseHash().equals(String.valueOf(password.hashCode()))) {  // Comparaison du mot de passe
                                String sessionId = GestionnaireAuth.creerSession(username);  // Création de la session
                                Message success = new Message("OK", Map.of("sessionId", sessionId));  // Message de succès avec l'ID de session
                                success.setSessionId(sessionId);  // Définir explicitement l'ID de session
                                return success;
                            }
                        }
                    }
                    return new Message("ERREUR", Map.of("message", "Identifiants incorrects"));  // Retourner une erreur si les identifiants sont incorrects

                case "LOGOUT":
                    // Traitement de la déconnexion de l'utilisateur
                    GestionnaireAuth.fermerSession(requete.getSessionId());  // Fermer la session
                    return new Message("OK", Map.of("message", "Déconnexion réussie"));

                case "SEARCH":
                    // Recherche d'un objet dans une collection
                    String nomCollection = (String) requete.getContenu().get("collection");
                    String critere = (String) requete.getContenu().get("critere");

                    Collection<ObjetBDD> collectionRecherche = Serveur.collections.get(nomCollection);
                    List<ObjetBDD> resultats = collectionRecherche.rechercher(critere);  // Rechercher les objets correspondant au critère

                    String message = resultats.isEmpty()
                            ? "Aucun résultat trouvé pour '" + critere + "'"
                            : resultats.size() + " résultat(s) trouvé(s)";

                    return new Message("OK", Map.of(
                            "message", message,
                            "resultats", resultats  // Retourner les résultats de la recherche
                    ));

                case "UPDATE_OBJECT":
                    return mettreAJourObjet(requete.getContenu());  // Mise à jour d'un objet (méthode à implémenter)

                case "DELETE_OBJECT":
                    return supprimerObjet(requete.getContenu());  // Suppression d'un objet (méthode à implémenter)

                default:
                    return new Message("ERREUR", Map.of("message", "Commande non reconnue : " + requete.getType()));  // Retourner une erreur si la commande est inconnue
            }
        } catch (Exception e) {
            return new Message("ERREUR", Map.of("message", "Erreur interne : " + e.getMessage()));  // Gestion des exceptions internes
        }
    }

    // --- Méthodes de traitement des requêtes ---

    /**
     * Crée une nouvelle collection.
     * @param contenu Le contenu de la requête contenant le nom de la collection
     * @return La réponse à envoyer au client
     */
    private Message creerCollection(Map<String, Object> contenu) {
        String nomCollection = (String) contenu.get("nom");
        Serveur.collections.put(nomCollection, new Collection<>(nomCollection));  // Ajouter la collection en mémoire
        Serveur.sauvegarderCollection(nomCollection);  // Sauvegarder la collection dans un fichier
        return new Message("OK", Map.of("message", "Collection '" + nomCollection + "' créée"));
    }

    /**
     * Ajoute un objet à une collection existante.
     * @param contenu Le contenu de la requête contenant la collection et l'objet
     * @return La réponse à envoyer au client
     */
    private Message ajouterObjet(Map<String, Object> contenu) {
        String nomCollection = (String) contenu.get("collection");
        ObjetBDD objet = (ObjetBDD) contenu.get("objet");

        // Créer la collection si elle n'existe pas
        Collection<ObjetBDD> collection = Serveur.collections.computeIfAbsent(
                nomCollection,
                k -> new Collection<>(nomCollection)
        );

        collection.ajouter(objet);  // Ajouter l'objet à la collection
        Serveur.sauvegarderCollection(nomCollection);  // Sauvegarder la collection
        return new Message("OK", Map.of("message", "Utilisateur ajouté à '" + nomCollection + "'"));
    }

    /**
     * Supprime un objet d'une collection.
     * @param contenu Le contenu de la requête contenant la collection et l'ID de l'objet à supprimer
     * @return La réponse à envoyer au client
     */
    private Message supprimerObjet(Map<String, Object> contenu) {
        String nomCollection = (String) contenu.get("collection");
        String idObjet = (String) contenu.get("id");
        Collection<ObjetBDD> collection = Serveur.collections.get(nomCollection);

        if (collection != null && collection.supprimer(idObjet)) {  // Suppression de l'objet de la collection
            Serveur.sauvegarderCollection(nomCollection);  // Sauvegarder la collection après suppression
            return new Message("OK", Map.of("message", "Objet supprimé"));
        } else {
            return new Message("ERREUR", Map.of("message", "Échec de la suppression"));
        }
    }

    /**
     * Met à jour un objet dans une collection.
     * @param contenu Le contenu de la requête contenant la collection et l'objet à mettre à jour
     * @return La réponse à envoyer au client
     */
    private Message mettreAJourObjet(Map<String, Object> contenu) {
        String nomCollection = (String) contenu.get("collection");
        ObjetBDD objet = (ObjetBDD) contenu.get("objet");
        Collection<ObjetBDD> collection = Serveur.collections.get(nomCollection);

        if (collection != null && collection.mettreAJour(objet)) {  // Mise à jour de l'objet dans la collection
            Serveur.sauvegarderCollection(nomCollection);  // Sauvegarder la collection après mise à jour
            return new Message("OK", Map.of("message", "Objet mis à jour"));
        } else {
            return new Message("ERREUR", Map.of("message", "Échec de la mise à jour"));
        }
    }

    // --- Méthodes utilitaires ---

    /**
     * Envoie une réponse au client.
     * @param reponse La réponse à envoyer au client
     * @throws IOException En cas d'erreur lors de l'envoi
     */
    private void envoyerReponse(Message reponse) throws IOException {
        oos.writeObject(reponse);  // Envoi de la réponse sous forme d'objet
        oos.flush();  // S'assurer que tout a été envoyé
    }

    /**
     * Ferme la connexion avec le client et les flux associés.
     */
    private void fermerConnexion() {
        try {
            if (oos != null) oos.close();  // Fermeture du flux de sortie
            if (socket != null) socket.close();  // Fermeture du socket de communication
        }
        catch (IOException e) {
            System.err.println("Erreur fermeture : " + e.getMessage());  // Gestion des erreurs lors de la fermeture
        }
    }
}
