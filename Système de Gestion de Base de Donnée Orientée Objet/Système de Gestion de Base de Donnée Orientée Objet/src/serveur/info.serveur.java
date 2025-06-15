/**
 * Ce package contient les classes nécessaires à la gestion du serveur ainsi que la manipulation des collections d'objets.
 *
 * <p>Le serveur est chargé d'accepter les connexions clients, de gérer les collections d'objets
 * et de traiter les différentes requêtes envoyées par les clients via des messages sérialisés.</p>
 *
 * <h2>Classes principales :</h2>
 * <ul>
 *   <li>{@link serveur.Serveur} : Classe principale qui démarre le serveur, écoute les connexions des clients
 *       et délègue le traitement des requêtes à des gestionnaires dédiés. Elle gère aussi la persistance des collections d'objets.</li>
 *   <li>{@link serveur.GestionnaireClient} : Classe responsable de la gestion de la communication avec un client particulier.
 *       Elle traite les requêtes du client et renvoie des réponses appropriées.</li>
 *   <li>{@link serveur.GestionnaireCollections} : Classe qui gère les collections d'objets persistants.
 *       Elle permet de charger, sauvegarder et manipuler les collections d'objets stockées sur disque.</li>
 * </ul>
 *
 * <p>Les collections sont stockées sous forme de fichiers sérialisés et sont chargées au démarrage du serveur.
 * Chaque requête client peut impliquer des opérations de création, mise à jour, suppression ou recherche dans ces collections.</p>
 */
package serveur;
