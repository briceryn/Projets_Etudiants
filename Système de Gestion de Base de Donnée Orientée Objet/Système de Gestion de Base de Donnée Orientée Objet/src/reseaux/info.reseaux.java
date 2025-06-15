/**
 * Ce package contient les classes liées à la communication entre le client et le serveur.
 *
 * <p>Il définit le protocole de messagerie basé sur des objets sérialisés, permettant
 * l’envoi de requêtes et de réponses structurées entre les différents composants de l’application.</p>
 *
 * <h2>Classe principale :</h2>
 * <ul>
 *   <li>{@link reseaux.Message} : représente un message réseau sérialisable. Il contient :</li>
 *   <ul>
 *     <li>Un type de message (ex: LOGIN, SEARCH, etc.)</li>
 *     <li>Un contenu (clé-valeurs) encapsulant les données utiles</li>
 *     <li>Un identifiant de session pour la gestion de l’authentification</li>
 *   </ul>
 * </ul>
 *
 * <p>Les instances de {@code Message} sont transmises via des flux d’objets
 * entre le client et le serveur pour exécuter des opérations métier.</p>
 */
package reseaux;
