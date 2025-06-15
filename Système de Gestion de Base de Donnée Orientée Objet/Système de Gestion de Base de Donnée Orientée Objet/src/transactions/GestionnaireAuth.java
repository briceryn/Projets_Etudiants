package transactions;

import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Classe responsable de la gestion de l'authentification des utilisateurs via des sessions.
 * <p>Elle permet de créer des sessions, valider leur existence, récupérer l'utilisateur associé à une session
 * et fermer des sessions. Cette gestion est effectuée à l'aide d'une map concurrente associant des identifiants de session
 * à des noms d'utilisateurs.</p>
 */
public class GestionnaireAuth {

    /**
     * Stocke les sessions actives sous forme d'une map, avec le sessionId comme clé et le nom d'utilisateur comme valeur.
     * La map est de type {@link ConcurrentHashMap} pour permettre un accès thread-safe dans un environnement multi-thread.
     */
    private static final Map<String, String> sessions = new ConcurrentHashMap<>();

    /**
     * Crée une nouvelle session pour un utilisateur donné.
     * <p>Un identifiant de session unique (UUID) est généré et associé à l'utilisateur. La session est ensuite ajoutée à la map.</p>
     *
     * @param username Le nom d'utilisateur pour lequel la session est créée.
     * @return L'identifiant de session généré.
     */
    public static String creerSession(String username) {
        // Génération d'un identifiant unique pour la session.
        String sessionId = UUID.randomUUID().toString();
        // Ajout de la session à la map
        sessions.put(sessionId, username);
        return sessionId;
    }

    /**
     * Valide si une session est active et existe dans le système.
     * <p>Cette méthode vérifie si l'identifiant de session fourni est présent dans la map des sessions.</p>
     *
     * @param sessionId L'identifiant de session à valider.
     * @return true si la session est valide (présente dans la map), false sinon.
     */
    public static boolean validerSession(String sessionId) {
        // Vérifie que la sessionId n'est pas nulle et qu'elle est présente dans la map des sessions.
        return sessionId != null && sessions.containsKey(sessionId);
    }

    /**
     * Récupère le nom d'utilisateur associé à un identifiant de session.
     * <p>Cette méthode retourne le nom d'utilisateur qui correspond à l'identifiant de session donné.</p>
     *
     * @param sessionId L'identifiant de session pour lequel le nom d'utilisateur est recherché.
     * @return Le nom d'utilisateur associé à la session, ou null si la session n'existe pas.
     */
    public static String getUsername(String sessionId) {
        // Retourne le nom d'utilisateur correspondant à l'identifiant de session, ou null si la session n'est pas trouvée.
        return sessions.get(sessionId);
    }

    /**
     * Ferme une session en la supprimant de la map des sessions actives.
     * <p>Cette méthode supprime une session à partir de son identifiant, mettant ainsi fin à la session.</p>
     *
     * @param sessionId L'identifiant de la session à fermer.
     */
    public static void fermerSession(String sessionId) {
        // Supprime l'identifiant de session de la map, fermant ainsi la session.
        sessions.remove(sessionId);
    }
}
