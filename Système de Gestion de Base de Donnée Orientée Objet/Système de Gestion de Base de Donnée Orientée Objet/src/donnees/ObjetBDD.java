package donnees;

import java.io.Serializable;
import java.util.UUID;

/**
 * Classe abstraite représentant un objet de base de données.
 *
 * <p>Chaque objet possède un identifiant unique généré automatiquement,
 * et peut être sérialisé pour permettre la persistance.</p>
 *
 * <p>Cette classe est conçue pour être héritée par des entités spécifiques
 * comme {@link Utilisateur} ou d'autres types d’objets métier.</p>
 */
public abstract class ObjetBDD implements Serializable {
    private static final long serialVersionUID = 1L;

    /**
     * Identifiant unique de l'objet.
     */
    protected String id;

    /**
     * Constructeur par défaut.
     * Génère automatiquement un identifiant UUID.
     */
    public ObjetBDD() {
        this.id = UUID.randomUUID().toString();
    }

    /**
     * Retourne l'identifiant unique de l'objet.
     *
     * @return l'ID
     */
    public String getId() {
        return id;
    }
}