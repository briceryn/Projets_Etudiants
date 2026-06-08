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
 * Classe représentant le résultat de la coloration d'un graphe
 * avec DSATUR ou autre algorithme
 */
public class Result {

    private int nbCouleurs;    // nombre de couleurs utilisées
    private double rho;        // ratio nk/n (pourcentage de sommets colorés avant la dernière couleur)
    private long tempsMs;      // temps d'exécution en millisecondes

    /**
     * Constructeur du résultat
     * @param nbCouleurs nombre de couleurs utilisées
     * @param rho ratio nk/n
     * @param tempsMs temps d'exécution en millisecondes
     */
    public Result(int nbCouleurs, double rho, long tempsMs) {
        this.nbCouleurs = nbCouleurs;
        this.rho = rho;
        this.tempsMs = tempsMs;
    }

    /**
     * Retourne le nombre de couleurs utilisées
     */
    public int getNbColors() {
        return nbCouleurs;
    }

    /**
     * Retourne le ratio nk/n
     */
    public double getRho() {
        return rho;
    }

    /**
     * Retourne le temps d'exécution en millisecondes
     */
    public long getTimeMs() {
        return tempsMs;
    }
}