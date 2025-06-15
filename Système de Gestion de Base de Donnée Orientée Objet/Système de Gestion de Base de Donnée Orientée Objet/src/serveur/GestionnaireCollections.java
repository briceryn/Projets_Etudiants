package serveur;

import donnees.Collection;
import donnees.ObjetBDD;
import java.io.*;
import java.nio.file.*;
import java.util.concurrent.ConcurrentHashMap;

import static serveur.Serveur.sauvegarderCollection;

public class GestionnaireCollections {
    // Dossier où les collections seront stockées sous forme de fichiers sérialisés
    private static final String DOSSIER_COLLECTIONS = "collections/";

    // Carte concurrente pour gérer les collections en mémoire
    private final ConcurrentHashMap<String, Collection<ObjetBDD>> collections = new ConcurrentHashMap<>();

    // Constructeur de la classe, initialise le dossier et charge les collections au démarrage
    public GestionnaireCollections() {
        creerDossierSiInexistant();  // Crée le dossier si il n'existe pas déjà
        chargerCollectionsAuDemarrage();  // Charge toutes les collections existantes au démarrage
    }

    /**
     * Crée le dossier de stockage des collections si il n'existe pas.
     */
    private void creerDossierSiInexistant() {
        try {
            // Création des répertoires nécessaires
            Files.createDirectories(Paths.get(DOSSIER_COLLECTIONS));
        } catch (IOException e) {
            System.err.println("Erreur création dossier: " + e.getMessage());
        }
    }

    /**
     * Charge une collection depuis le fichier sérialisé correspondant dans le dossier "collections/".
     * @param nomCollection Le nom de la collection à charger.
     */
    private void chargerCollection(String nomCollection) {
        String cheminFichier = DOSSIER_COLLECTIONS + nomCollection + ".ser";
        try (ObjectInputStream ois = new ObjectInputStream(new FileInputStream(cheminFichier))) {
            // Désérialisation de la collection
            Collection<ObjetBDD> collection = (Collection<ObjetBDD>) ois.readObject();
            if (collection != null) {
                // Ajout de la collection chargée à la carte en mémoire
                collections.put(nomCollection, collection);
                System.out.println("[OK] Collection chargée : " + nomCollection);
            } else {
                System.err.println("La collection lue est nulle pour " + cheminFichier);
            }
        } catch (FileNotFoundException e) {
            System.err.println("Fichier non trouvé : " + cheminFichier);
        } catch (IOException | ClassNotFoundException e) {
            System.err.println("Erreur chargement : " + e.getMessage());
        }
    }

    /**
     * Charge toutes les collections présentes dans le dossier "collections/" au démarrage du serveur.
     */
    private void chargerCollectionsAuDemarrage() {
        File dossier = new File(DOSSIER_COLLECTIONS);
        // Si le dossier n'existe pas, rien à charger
        if (!dossier.exists()) return;

        // Liste des fichiers dans le dossier qui se terminent par ".ser"
        File[] fichiers = dossier.listFiles((dir, name) -> name.endsWith(".ser"));
        if (fichiers == null) return;

        // Pour chaque fichier de collection, charger la collection correspondante
        for (File fichier : fichiers) {
            String nomCollection = fichier.getName().replace(".ser", "");
            chargerCollection(nomCollection);  // Chargement de la collection
        }
    }

    /**
     * Récupère une collection, ou en crée une nouvelle si elle n'existe pas encore.
     * @param nomCollection Le nom de la collection à récupérer.
     * @return La collection demandée.
     */
    public Collection<ObjetBDD> getCollection(String nomCollection) {
        // Si la collection n'existe pas, en créer une nouvelle et la sauvegarder
        return collections.computeIfAbsent(nomCollection, k -> {
            Collection<ObjetBDD> nouvelleCollection = new Collection<>(nomCollection);
            sauvegarderCollection(nomCollection);  // Sauvegarde la nouvelle collection
            return nouvelleCollection;
        });
    }

    /**
     * Sauvegarde une collection sérialisée dans un fichier.
     * @param nomCollection Le nom de la collection à sauvegarder.
     */
    private void sauvegarderCollection(String nomCollection) {
        String cheminFichier = DOSSIER_COLLECTIONS + nomCollection + ".ser";
        try (ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(cheminFichier))) {
            // Sérialisation de la collection
            oos.writeObject(collections.get(nomCollection));
            System.out.println("[OK] Sauvegarde réussie : " + cheminFichier);
        } catch (NotSerializableException e) {
            // Si la classe des objets de la collection n'est pas sérialisable
            System.err.println("Erreur : Classe non sérializable - " + e.getMessage());
        } catch (IOException e) {
            // Autres erreurs d'entrée/sortie lors de la sauvegarde
            System.err.println("Erreur E/S : " + e.getMessage());
            e.printStackTrace();  // Affiche la stack trace complète pour débogage
        }
    }
}
