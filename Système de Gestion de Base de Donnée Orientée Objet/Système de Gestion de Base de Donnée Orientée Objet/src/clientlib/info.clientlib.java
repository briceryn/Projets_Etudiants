/**
 * Ce package contient les classes nécessaires pour interagir avec le serveur de base de données,
 * permettant l'exécution de diverses opérations telles que la connexion, l'authentification,
 * la gestion des collections d'objets et les recherches.
 *
 * <p>Il fournit un client réutilisable pour effectuer des opérations sur la base de données,
 * telles que l'ajout d'objets, la recherche, la création de collections, ainsi que la gestion des sessions.</p>
 *
 * <h2>Classes principales :</h2>
 * <ul>
 *   <li>{@link clientlib.ClientDB} : Représente un client de base de données permettant de :
 *     <ul>
 *       <li>Se connecter au serveur</li>
 *       <li>Se connecter/déconnecter (authentification)</li>
 *       <li>Créer des collections d'objets</li>
 *       <li>Ajouter des objets aux collections</li>
 *       <li>Effectuer des recherches dans les collections</li>
 *     </ul>
 *   </li>
 *   <li>{@link clientlib.AuthException} : Exception levée en cas d'échec d'authentification lors de la connexion au serveur.</li>
 *   <li>{@link clientlib.DBException} : Exception générale levée pour signaler des erreurs liées aux opérations sur la base de données.</li>
 * </ul>
 *
 * <h2>Exemple d'utilisation :</h2>
 * <pre>
 * ClientDB client = new ClientDB("localhost", 8080);
 * client.connect();  // Connexion au serveur
 * client.login("admin", "password");  // Authentification
 * client.createCollection("utilisateurs");  // Création d'une collection
 * client.addObject("utilisateurs", new Utilisateur("alice", "pass123"));  // Ajout d'un objet
 * List<ObjetBDD> results = client.search("utilisateurs", "nomUtilisateur", "alice");  // Recherche d'objets
 * </pre>
 *
 * <h2>Flux de données :</h2>
 * <p>Les données sont envoyées et reçues sous forme d'objets sérialisés. Les requêtes sont représentées
 * par des objets {@link reseaux.Message}, envoyées au serveur via des flux d'objets.</p>
 */
package clientlib;
