package serveur;

import donnees.Collection;
import donnees.ObjetBDD;

import java.io.*;
import java.net.ServerSocket;
import java.nio.file.*;
import java.util.concurrent.ConcurrentHashMap;

public class Serveur {
    // Carte concurrente pour stocker toutes les collections en mémoire
    public static final ConcurrentHashMap<String, Collection<ObjetBDD>> collections = new ConcurrentHashMap<>();
    // Gestionnaire des collections, utilisé pour la gestion des collections persistantes
    private static GestionnaireCollections gestionnaire = new GestionnaireCollections();

    public static void main(String[] args) {
        // Crée le dossier "collections" s'il n'existe pas
        creerDossierCollections();

        // Assure que la collection "utilisateurs" existe, sinon elle est créée
        gestionnaire.getCollection("utilisateurs");

        // Charge toutes les collections existantes depuis les fichiers
        chargerCollectionsDepuisDossier();

        // Démarre le serveur
        demarrerServeur();
    }

    /**
     * Démarre le serveur et écoute les connexions des clients sur le port 8080.
     * À chaque nouvelle connexion, un nouveau thread est lancé pour gérer le client.
     */
    private static void demarrerServeur() {
        try (ServerSocket ss = new ServerSocket(8080)) {
            // Message de confirmation lorsque le serveur démarre
            System.out.println("Serveur démarré sur le port 8080");

            // Attente et gestion des connexions des clients en boucle infinie
            while (true) {
                new GestionnaireClient(ss.accept()).start();  // Lance un nouveau thread pour chaque client
            }
        }
        catch (IOException e) {
            // En cas d'erreur lors du démarrage du serveur
            System.err.println("Erreur démarrage serveur : " + e.getMessage());
        }
    }

    /**
     * Charge une collection depuis le fichier sérialisé correspondant dans le dossier "collections/".
     * @param nomCollection Le nom de la collection à charger.
     * @return La collection chargée, ou une nouvelle collection vide en cas d'échec.
     */
    public static Collection<ObjetBDD> chargerCollection(String nomCollection) {
        String chemin = "collections/" + nomCollection + ".ser";
        try (ObjectInputStream ois = new ObjectInputStream(new FileInputStream(chemin))) {
            // Désérialisation de la collection depuis le fichier
            Collection<ObjetBDD> collection = (Collection<ObjetBDD>) ois.readObject();
            System.out.println("[DEBUG] Chargement réussi : " + collection.getTous().size() + " utilisateurs");
            return collection;  // Retourne la collection chargée
        } catch (Exception e) {
            // En cas d'échec de la désérialisation, on crée une nouvelle collection vide
            System.err.println("[DEBUG] Échec chargement : " + e.getMessage());
            return new Collection<>(nomCollection);  // Nouvelle collection vide
        }
    }

    /**
     * Charge toutes les collections présentes dans le dossier "collections/" au démarrage du serveur.
     * Cette méthode est appelée pour charger toutes les collections existantes et les ajouter en mémoire.
     */
    private static void chargerCollectionsDepuisDossier() {
        File dossier = new File("collections"); // Dossier relatif où les collections sont stockées
        if (!dossier.exists()) return;  // Si le dossier n'existe pas, on quitte la méthode

        // Liste des fichiers se terminant par ".ser" dans le dossier
        File[] fichiers = dossier.listFiles((dir, name) -> name.endsWith(".ser"));
        if (fichiers == null) return;  // Si aucun fichier n'est trouvé, on quitte la méthode

        // Pour chaque fichier de collection, on charge la collection correspondante
        for (File fichier : fichiers) {
            String nomCollection = fichier.getName().replace(".ser", "");
            // On charge la collection via le gestionnaire et on l'ajoute à la carte des collections en mémoire
            Collection<ObjetBDD> collection = gestionnaire.getCollection(nomCollection);
            Serveur.collections.put(nomCollection, collection);
        }
    }

    /**
     * Crée le dossier "collections" s'il n'existe pas déjà.
     */
    private static void creerDossierCollections() {
        try {
            // Création du répertoire "collections"
            Files.createDirectories(Paths.get("collections/"));
        }
        catch (IOException e) {
            // Gestion des erreurs lors de la création du dossier
            System.err.println("Erreur création dossier : " + e.getMessage());
        }
    }

    /**
     * Sauvegarde une collection dans un fichier sérialisé dans le dossier "collections/".
     * @param nomCollection Le nom de la collection à sauvegarder.
     */
    public static void sauvegarderCollection(String nomCollection) {
        try (ObjectOutputStream oos = new ObjectOutputStream(
                new FileOutputStream("collections/" + nomCollection + ".ser"))
        ) {
            // Sérialisation de la collection dans un fichier
            oos.writeObject(collections.get(nomCollection));
            System.out.println("[SAVE] Collection sauvegardée : " + nomCollection);
        }
        catch (IOException e) {
            // En cas d'erreur d'entrée/sortie lors de la sauvegarde
            System.err.println("Erreur sauvegarde : " + e.getMessage());
        }
    }
}
