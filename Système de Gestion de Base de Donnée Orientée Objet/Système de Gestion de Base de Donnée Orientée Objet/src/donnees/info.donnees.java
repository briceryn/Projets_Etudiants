/**
 * Ce package contient les classes représentant les données persistantes de l'application.
 *
 * <p>Il fournit une base pour la gestion des objets métiers stockés dans des collections,
 * ainsi que la logique de sérialisation nécessaire à leur sauvegarde et leur transmission.</p>
 *
 * <h2>Principales classes :</h2>
 * <ul>
 *   <li>{@link donnees.ObjetBDD} : classe abstraite de base pour tous les objets persistants. Fournit un identifiant unique.</li>
 *   <li>{@link donnees.Utilisateur} : entité représentant un utilisateur avec un nom et un mot de passe haché.</li>
 *   <li>{@link donnees.Collection} : structure de stockage d'objets de même type, avec opérations CRUD, recherche, et mise à jour.</li>
 * </ul>
 *
 * <h2>Fonctionnalités :</h2>
 * <ul>
 *   <li>Sérialisation automatique des objets</li>
 *   <li>Recherche dans les collections (par ID ou champ spécifique)</li>
 *   <li>Mise à jour et suppression d’éléments</li>
 *   <li>Support de la persistance via {@code Serveur.sauvegarderCollection()}</li>
 * </ul>
 *
 * <p>Les objets de ce package sont conçus pour être manipulés à la fois par le serveur
 * et par les clients à travers des messages réseau.</p>
 */

package donnees;