/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Main.java to edit this template
 */
package projetgraphe2026;

/**
 *
 * @author Thomas MORAUX & Ryan LAWSON
 */

/**
 * Classe principale du projet
 * Permet de charger un graphe, exécuter DSATUR et afficher les résultats
 */
public class ProjetGraphe2026 {

    public static void main(String[] args) {

        // Nom du fichier contenant le graphe
        // Par défaut "david.col"
        String nomFichier = (args.length > 0) ? args[0] : "Email-Enron.txt";

        // Lecture du graphe
        Graph graphe = GraphReader.lireGraphe(nomFichier);

        // Vérification que le graphe a bien été lu
        if (graphe == null) {
            System.err.println("Erreur : impossible de lire le graphe depuis le fichier " + nomFichier);
            return;
        }

        // Exécution de l'algorithme DSATUR
        DSATUR dsatur = new DSATUR();
        Result resultat = dsatur.colorGraph(graphe);

        // Affichage des résultats
        System.out.println("Nom du fichier : " + nomFichier);
        System.out.println("Nombre de sommets : " + graphe.getNombreSommets());
        System.out.println("Nombre de couleurs utilisées : " + resultat.getNbColors());
        System.out.println("Ratio rho : " + resultat.getRho());
        System.out.println("Temps d'exécution : " + resultat.getTimeMs() + " ms");
    }
}