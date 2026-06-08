/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Main.java to edit this template
 */
package projetgraphe2026;

/**
 *
 * @author Thomas MORAUX & Ryan LAWSON
 */
import java.util.*;

/**
 * Algorithme DSATUR optimisé pour grands graphes
 */
public class DSATUR {

    /**
     * Classe interne représentant un sommet
     */
    static class Sommet {
        int id;
        int degre;
        BitSet couleursVoisins; // couleurs utilisées par les voisins
        int saturation;

        public Sommet(int id, int degre) {
            this.id = id;
            this.degre = degre;
            this.couleursVoisins = new BitSet();
            this.saturation = 0;
        }
    }

    /**
     * Colorie le graphe avec DSATUR et calcule rho et temps d'exécution
     */
    public Result colorGraph(Graph graphe) {
        int n = graphe.getNombreSommets();

        int[] couleurs = new int[n];
        Arrays.fill(couleurs, -1);

        Sommet[] sommets = new Sommet[n];
        for (int i = 0; i < n; i++) {
            sommets[i] = new Sommet(i, graphe.getVoisins(i).size());
        }

        // PriorityQueue max-heap selon saturation puis degré
        PriorityQueue<Sommet> pq = new PriorityQueue<>(
            (a, b) -> (b.saturation != a.saturation) ? b.saturation - a.saturation : b.degre - a.degre
        );
        pq.addAll(Arrays.asList(sommets));

        int colorMax = -1;
        int[] n_i = new int[n + 1]; // stockage nk
        int colories = 0;

        long start = System.currentTimeMillis();

        while (!pq.isEmpty()) {
            Sommet u = pq.poll();

            // lazy update : déjà coloré
            if (couleurs[u.id] != -1) continue;

            // première couleur disponible
            int c = u.couleursVoisins.nextClearBit(0);

            couleurs[u.id] = c;
            colories++;

            // ajuster n_i si besoin
            if (c >= n_i.length) n_i = Arrays.copyOf(n_i, c + 1);
            if (c > colorMax) {
                colorMax = c;
                n_i[c] = colories;
            }

            // mise à jour des voisins
            for (int v : graphe.getVoisins(u.id)) {
                if (couleurs[v] == -1) {
                    Sommet voisin = sommets[v];
                    if (!voisin.couleursVoisins.get(c)) {
                        voisin.couleursVoisins.set(c);
                        voisin.saturation++;
                        pq.add(voisin); // lazy update
                    }
                }
            }
        }

        long end = System.currentTimeMillis();
        double rho = (double) n_i[colorMax] / n;
        int k = colorMax + 1;

        return new Result(k, rho, end - start);
    }
}