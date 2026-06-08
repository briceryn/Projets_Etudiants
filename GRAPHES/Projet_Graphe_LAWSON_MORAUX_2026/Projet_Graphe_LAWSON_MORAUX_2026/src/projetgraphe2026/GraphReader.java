/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Main.java to edit this template
 */
package projetgraphe2026;

/**
 *
 * @author Thomas MORAUX & Ryan LAWSON
 */
import java.io.*;
import java.util.*;

/**
 * Classe utilitaire permettant de lire un graphe
 * depuis un fichier au format DIMACS, SNAP ou KONECT
 */
public class GraphReader {

    /**
     * Lit un fichier et construit le graphe correspondant
     */
    public static Graph lireGraphe(String nomFichier) {

        Graph graphe = null;

        try (BufferedReader lecteur = new BufferedReader(new FileReader(nomFichier))) {

            String ligne;
            boolean formatDIMACS = false;

            // Stockage temporaire des arêtes
            ArrayList<int[]> listeAretes = new ArrayList<>();

            int sommetMax = -1; // pour déterminer la taille du graphe

            while ((ligne = lecteur.readLine()) != null) {

                ligne = ligne.trim();

                // Ignorer lignes vides
                if (ligne.isEmpty()) continue;

                // Ignorer commentaires (DIMACS, SNAP, KONECT)
                if (ligne.startsWith("c") || ligne.startsWith("#") || ligne.startsWith("%")) {
                    continue;
                }

                // Cas DIMACS : ligne décrivant le problème
                if (ligne.startsWith("p")) {
                    formatDIMACS = true;

                    String[] parties = ligne.split("\\s+");
                    int nombreSommets = Integer.parseInt(parties[2]);

                    graphe = new Graph(nombreSommets);
                    continue;
                }

                // Cas DIMACS : ligne d'arête
                if (ligne.startsWith("e")) {
                    formatDIMACS = true;

                    String[] parties = ligne.split("\\s+");
                    int u = Integer.parseInt(parties[1]);
                    int v = Integer.parseInt(parties[2]);

                    listeAretes.add(new int[]{u, v});
                    sommetMax = Math.max(sommetMax, Math.max(u, v));
                    continue;
                }

                // Cas SNAP / KONECT : format simple "u v"
                String[] parties = ligne.split("\\s+");

                if (parties.length >= 2) {
                    int u = Integer.parseInt(parties[0]);
                    int v = Integer.parseInt(parties[1]);

                    listeAretes.add(new int[]{u, v});
                    sommetMax = Math.max(sommetMax, Math.max(u, v));
                }
            }

            // Si le graphe n’est pas en DIMACS → on crée après lecture
            if (!formatDIMACS) {
                graphe = new Graph(sommetMax + 1);
            }

            // Détection automatique de l’indexation (0-based ou 1-based)
            boolean indexationCommenceA1 = true;

            for (int[] arete : listeAretes) {
                if (arete[0] == 0 || arete[1] == 0) {
                    indexationCommenceA1 = false;
                    break;
                }
            }

            // Ajout des arêtes dans le graphe
            for (int[] arete : listeAretes) {

                int u = indexationCommenceA1 ? arete[0] - 1 : arete[0];
                int v = indexationCommenceA1 ? arete[1] - 1 : arete[1];

                graphe.ajouterArete(u, v);
            }

        } catch (IOException e) {
            e.printStackTrace();
        }

        return graphe;
    }
}