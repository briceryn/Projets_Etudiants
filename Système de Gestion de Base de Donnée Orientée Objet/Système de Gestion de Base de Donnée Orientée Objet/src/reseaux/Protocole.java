package reseaux;

/**
 * Définit les constantes utilisées pour les types de messages (commandes)
 * et les codes de réponse dans le protocole de communication client-serveur.
 *
 * <p>Les noms sont courts pour limiter la taille des messages sérialisés.</p>
 */
public class Protocole {
    // --- Opérations côté client ---

    /** Créer une nouvelle collection d'objets */
    public static final String CREATE_COLLECTION = "CREATE_COLL";

    /** Ajouter un objet à une collection */
    public static final String ADD_OBJECT = "ADD_OBJ";

    /** Rechercher des objets dans une collection */
    public static final String SEARCH = "SEARCH";

    /** Mettre à jour un objet existant */
    public static final String UPDATE_OBJECT = "UPDATE_OBJ";

    /** Supprimer un objet d'une collection */
    public static final String DELETE_OBJECT = "DELETE_OBJ";

    /** Supprimer une collection entière */
    public static final String DELETE_COLLECTION = "DELETE_COLL";

    /** Lister toutes les collections existantes */
    public static final String LIST_COLLECTIONS = "LIST_COLL";

    // --- Réponses du serveur ---

    /** Réponse réussie */
    public static final String OK = "OK";

    /** Réponse contenant une erreur */
    public static final String ERROR = "ERROR";
}
