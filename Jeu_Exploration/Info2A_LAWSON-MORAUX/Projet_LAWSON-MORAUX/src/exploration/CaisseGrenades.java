/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package exploration;

import java.util.Random;

/**
 * À compléter
 * @author jo
 */
public class CaisseGrenades extends Objet
{
    private final static int MAX = 9;
    private boolean ramasser = false;
    private int disponible;

    public CaisseGrenades(int nbGrenades)
    {
        super(
                "o",
                "Caisse de grenades"
        );
        //suite code
        this.setDisponible(nbGrenades);
    }
    
    public CaisseGrenades()
    {
        this(generateRandomDisponible());
    }
        
    public int getDisponible(){
        return this.disponible;
    }
    
    public void setDisponible(int disponible){
        if(this.getDisponible() >= 0 && this.getDisponible() <= MAX)
            this.disponible = disponible;

    }
    
    public boolean getRamasser(){
        return this.ramasser;
    }
    
    private void setRamasser(boolean bool){
        this.ramasser = bool;
    }
    
    private static int generateRandomDisponible() {
        Random rand = new Random();
        return rand.nextInt(MAX) + 1;
    }
    
    
    @Override
    public void interaction(Joueur j)
    {
        /* S'il reste des grenades dans cette réserve,
           propose au joueur d'en prendre un certain nombre,
           ce qui augmente les grenades du joueur et diminue d'autant cette réserve
        */
        if(this.getDisponible() > 0){
            System.out.println("\nIl y a " + this.getDisponible()+" grenade(s) dans cette caisse.");
            int nbUnites = Lire.i("De combien d'unites avez vous besoin");

            if(nbUnites  > this.getDisponible()){
                nbUnites = this.getDisponible();
            }

            if(j.getMunitions()+nbUnites>MAX){
                System.out.println("\nVous ne pouvez pas en prendre tant ! Le max de grenades transportable est : " + MAX);
            }else{
                j.augmenterMunitions(nbUnites);
                this.setDisponible(this.getDisponible() - nbUnites);
                System.out.println("\nVous avez recupere : " + nbUnites + " grenade(s).");
            }
        }
        if (this.getDisponible() == 0){
            this.setRamasser(true);
        }
    }
    
    @Override
    public String toString() {
        if(this.getDisponible() != 0)
            return "" + this.getSymbole() + this.getDisponible();
        else
            return "";
    }
}
