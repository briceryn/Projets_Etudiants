/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package projetgraphe2026;

/**
 *
 * @author Thomas MORAUX & Ryan LAWSON
 */
import java.util.*;

/**
 * Classe représentant un graphe non orienté
 * Stocké sous forme de liste d’adjacence
 */
public class Graph {

    private int nombreSommets;                          // nombre total de sommets
    private ArrayList<ArrayList<Integer>> adjacence;    // liste d'adjacence

    /**
    * Constructeur de la classe Graph.
    * @param nombreSommets le nombre total de sommets dans le graphe
    */
   public Graph(int nombreSommets) {
       this.nombreSommets = nombreSommets;
       adjacence = new ArrayList<>();

       // Initialisation des listes de voisins pour chaque sommet
       for (int i = 0; i < nombreSommets; i++) {
           adjacence.add(new ArrayList<>());
       }
   }

    /**
     * Ajoute une arête entre deux sommets u et v
     * (graphe non orienté → ajout dans les deux sens)
     */
    public void ajouterArete(int u, int v) {
        adjacence.get(u).add(v);
        adjacence.get(v).add(u);
    }

    /**
     * Retourne la liste des voisins d’un sommet
     */
    public ArrayList<Integer> getVoisins(int sommet) {
        return adjacence.get(sommet);
    }

    /**
     * Retourne le nombre de sommets du graphe
     */
    public int getNombreSommets() {
        return nombreSommets;
    }

    /**
     * Retourne le degré d’un sommet
     * (nombre de voisins)
     */
    public int getDegre(int sommet) {
        return adjacence.get(sommet).size();
    }
}
