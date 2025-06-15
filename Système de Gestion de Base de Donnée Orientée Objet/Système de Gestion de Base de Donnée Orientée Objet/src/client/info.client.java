/**
 * Le package {@code client} contient les classes nécessaires pour implémenter le terminal
 * client de l'application de gestion de base de données orientée objet.
 *
 * <p>Ce terminal permet à un utilisateur de se connecter à un serveur via socket,
 * d'exécuter diverses commandes (authentification, création de collections, recherche, etc.),
 * et de recevoir des réponses du serveur au format objet sérialisé.</p>
 *
 * <p>Le client interagit avec le serveur via des objets {@link reseaux.Message} et utilise
 * des classes de données comme {@link donnees.Utilisateur} ou {@link donnees.ObjetBDD}.</p>
 *
 * @author Brice
 */

package client;