package donnees;

import reseaux.Message;
import serveur.Serveur;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * Classe générique représentant une collection d'objets de type {@link ObjetBDD}.
 *
 * <p>Chaque collection possède un nom unique et stocke ses objets dans une liste.
 * Elle est sérialisable pour permettre la sauvegarde et la persistance automatique.</p>
 *
 * @param <T> le type d'objet contenu, qui doit hériter de {@code ObjetBDD}
 */
public class Collection<T extends ObjetBDD> implements Serializable {
    private static final long serialVersionUID = 1L;

    private final String nom;
    private final List<T> objets = new ArrayList<>();

    /**
     * Crée une nouvelle collection avec un nom donné.
     *
     * @param nom le nom de la collection
     */
    public Collection(String nom) {
        this.nom = nom;
    }

    // =============================
    // === MÉTHODES DE BASE (CRUD)
    // =============================

    /**
     * Ajoute un objet à la collection.
     *
     * @param objet l'objet à ajouter
     */
    public void ajouter(T objet) {
        objets.add(objet);
    }

    /**
     * Supprime un objet de la collection par son ID.
     *
     * @param id l'identifiant de l'objet à supprimer
     * @return true si un objet a été supprimé, false sinon
     */
    public boolean supprimer(String id) {
        return objets.removeIf(obj -> obj.getId().equals(id));
    }

    /**
     * Recherche un objet dans la collection par son ID.
     *
     * @param id identifiant recherché
     * @return l'objet trouvé ou {@code null} si absent
     */
    public T rechercherParId(String id) {
        return objets.stream()
                .filter(obj -> obj.getId().equals(id))
                .findFirst()
                .orElse(null);
    }

    // =============================
    // === RECHERCHE
    // =============================

    /**
     * Recherche tous les objets dont l'ID ou le nom d'utilisateur (si applicable)
     * contient le critère donné.
     *
     * @param critere chaîne de caractères à rechercher
     * @return liste des objets correspondants
     */
    public List<T> rechercher(String critere) {
        return objets.stream()
                .filter(obj -> {
                    boolean matchId = obj.getId().contains(critere);
                    boolean matchUsername = obj instanceof Utilisateur
                            && ((Utilisateur) obj).getNomUtilisateur().contains(critere);
                    return matchId || matchUsername;
                })
                .collect(Collectors.toList());
    }

    // =============================
    // === MISE À JOUR
    // =============================

    /**
     * Met à jour un objet existant dans la collection.
     *
     * @param nouvelObjet l'objet mis à jour avec le même ID
     * @return true si l'objet a été mis à jour, false sinon
     */
    public boolean mettreAJour(T nouvelObjet) {
        for (int i = 0; i < objets.size(); i++) {
            if (objets.get(i).getId().equals(nouvelObjet.getId())) {
                objets.set(i, nouvelObjet);
                return true;
            }
        }
        return false;
    }

    /**
     * (Obsolète / À déplacer dans Serveur) Met à jour un objet via un message.
     *
     * @param contenu map contenant les clés "collection" et "objet"
     * @return message de confirmation
     */
    @Deprecated
    private Message mettreAJourObjet(Map<String, Object> contenu) {
        String nomCollection = (String) contenu.get("collection");
        ObjetBDD objet = (ObjetBDD) contenu.get("objet");
        Collection<ObjetBDD> collection = Serveur.collections.get(nomCollection);
        collection.mettreAJour(objet);
        return new Message("OK", Map.of("message", "Objet mis à jour"));
    }

    // =============================
    // === GETTERS
    // =============================

    /**
     * Retourne une copie défensive de tous les objets.
     *
     * @return liste des objets
     */
    public List<T> getTous() {
        return new ArrayList<>(objets);
    }

    /**
     * Nom de la collection.
     *
     * @return le nom
     */
    public String getNom() {
        return nom;
    }

    /**
     * Taille de la collection (nombre d'objets).
     *
     * @return taille
     */
    public int getTaille() {
        return objets.size();
    }
}
