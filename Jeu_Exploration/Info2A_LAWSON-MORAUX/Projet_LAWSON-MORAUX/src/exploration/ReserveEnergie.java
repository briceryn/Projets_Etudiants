/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package exploration;

import java.util.Random;

/**
 *
 * @author jo
 */
public class ReserveEnergie extends Objet
{
    private final static int MAX = 9;
    private boolean ramasser = false;
    private int disponible;

    public ReserveEnergie(int disponible)
    {
        super(
                "⚛",
                "Reserve energie"
        );
        this.setDisponible(disponible);
    }
    
    public ReserveEnergie()
    {
        this(generateRandomDisponible());
    }

    private static int generateRandomDisponible() {
        Random rand = new Random();
        return rand.nextInt(MAX) + 1;
    }

    public int getDisponible(){
        return this.disponible;
    }

    public void setDisponible(int disponible){
        if(this.getDisponible()>= 0 && this.getDisponible()<=MAX)
            this.disponible = disponible;

    }
    
    public boolean getRamasser(){
        return this.ramasser;
    }
    
    private void setRamasser(boolean bool){
        this.ramasser = bool;
    }
    
    @Override
    public void interaction(Joueur j)
    {
        /* S'il reste des unités d'énergie dans cette réserve,
           propose au joueur d'en prendre un certain nombre,
           ce qui augmente l'énergie du joueur et diminue d'autant cette réserve
        */
        if(this.getDisponible() > 0){
            // S'il reste des unités d'énergie dans cette réserve
            //On Propose au joueur d'en prendre un certain nombre
            System.out.println("Il y'a " + this.getDisponible()+" d'unitées d'energie dans la reserve");
            int nbUnites = Lire.i("De combien d'unites avez vous besoin");

            if(nbUnites  > this.getDisponible()){
                nbUnites = this.getDisponible();
            }
            // On Augmente l'énergie du joueur et diminue d'autant cette réserve
            if(j.getBatterie()+nbUnites>MAX){
                System.out.println("\nVous ne pouvez pas en prendre tant ! Le max de grenades transportable est : " + MAX);
            }else{
                j.augmenterEnergie(nbUnites);
                this.setDisponible(this.getDisponible() - nbUnites);
                System.out.println("\nVous avez recupere : " + nbUnites + " energie(s).");
            }
        }
        if (this.getDisponible()== 0){
            this.setRamasser(true);
        }
    }

    @Override
    public String toString() {
        if(this.getDisponible() != 0)// On vérifie si la réserve contient des unités d'énergie
            return "" + this.getSymbole() + this.getDisponible();// Si oui on retourne le symbole de la reserve d'energie
        else
            return "  ";//  Si non on retourne un espace vide
    }
}
