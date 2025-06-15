package reseaux;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Map;

/**
 * Représente un message réseau échangé entre le client et le serveur.
 *
 * <p>Un message contient :</p>
 * <ul>
 *     <li>Un type (commande ou réponse), comme "LOGIN", "SEARCH", etc.</li>
 *     <li>Un contenu sous forme de paires clé-valeur (paramètres de la requête ou données de la réponse)</li>
 *     <li>Un identifiant de session pour authentifier les requêtes après connexion</li>
 * </ul>
 *
 * <p>Les messages sont sérialisés et transmis via des flux réseau.</p>
 */
public class Message implements Serializable {
    private String sessionId;
    private final String type;
    private final Map<String, Object> contenu;

    /**
     * Crée un message sans identifiant de session.
     * @param type le type de message (ex : LOGIN, SEARCH)
     * @param contenu les paramètres ou données associés
     */
    public Message(String type, Map<String, Object> contenu) {
        this(type, contenu, null);
    }

    /**
     * Crée un message avec un identifiant de session explicite.
     * @param type le type de message
     * @param contenu les paramètres ou données associés
     * @param sessionId l'identifiant de session (null si non authentifié)
     */
    public Message(String type, Map<String, Object> contenu, String sessionId) {
        this.type = type;
        this.contenu = new HashMap<>(contenu);
        this.sessionId = sessionId;
    }

    // --- Accesseurs et mutateurs ---

    public String getType() { return type; }

    public Map<String, Object> getContenu() {
        return new HashMap<>(contenu); // Copie défensive
    }

    public String getSessionId() { return sessionId; }

    public void setSessionId(String sessionId) { this.sessionId = sessionId; }

    /**
     * Retourne une représentation textuelle du message, partiellement anonymisée.
     */
    @Override
    public String toString() {
        return String.format("Message[type=%s, session=%s, contenu=%s]",
                type, sessionId != null ? sessionId.substring(0, 8) + "..." : "null", contenu);
    }
}
