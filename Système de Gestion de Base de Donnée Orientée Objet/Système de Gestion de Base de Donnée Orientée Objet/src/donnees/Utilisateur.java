package donnees;

import java.io.Serializable;

/**
 * Représente un utilisateur du système.
 * <p>
 * Chaque utilisateur possède un nom d'utilisateur et un mot de passe haché.
 * Le hachage est actuellement basé sur {@code String.hashCode()}, ce qui est
 * insuffisant pour une application en production.
 * </p>
 */
public class Utilisateur extends ObjetBDD implements Serializable {
    private static final long serialVersionUID = 1L;

    private String nomUtilisateur;
    private String motDePasseHash;

    /**
     * Crée un utilisateur avec nom d'utilisateur et mot de passe (en clair).
     *
     * @param nom      nom d'utilisateur
     * @param mdpClair mot de passe en clair
     */
    public Utilisateur(String nom, String mdpClair) {
        super();
        this.nomUtilisateur = nom;
        this.motDePasseHash = hacherMdp(mdpClair);
    }

    /**
     * Hache le mot de passe (version simplifiée).
     *
     * @param mdp mot de passe en clair
     * @return hachage du mot de passe
     */
    private String hacherMdp(String mdp) {
        return Integer.toString(mdp.hashCode()); // À remplacer par BCrypt pour la prod
    }

    /**
     * Vérifie si un mot de passe en clair correspond au hachage stocké.
     *
     * @param mdpClair mot de passe à tester
     * @return true si le mot de passe est correct
     */
    public boolean verifierMdp(String mdpClair) {
        return this.motDePasseHash.equals(hacherMdp(mdpClair));
    }

    /**
     * Retourne le nom d'utilisateur.
     *
     * @return le nom d'utilisateur
     */
    public String getNomUtilisateur() {
        return this.nomUtilisateur;
    }

    /**
     * Retourne le mot de passe haché.
     * (À éviter d'exposer en production)
     *
     * @return hachage du mot de passe
     */
    public String getMotDePasseHash() {
        return this.motDePasseHash;
    }

    @Override
    public String toString() {
        return String.format("Utilisateur[id=%s, nom=%s]", id, nomUtilisateur);
    }
}
