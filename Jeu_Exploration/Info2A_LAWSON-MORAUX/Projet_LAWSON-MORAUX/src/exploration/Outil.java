/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package exploration;

import java.util.Objects;

/**
 *
 * @author jo
 */
public abstract class Outil extends Objet
{    
    private int conso;
    private String descriptif;
    
    public Outil(String symbole, String nature, String descriptif, int conso)
    {
        super(
                symbole,
                nature
        );
        this.setDescriptif(descriptif);
        this.setConsommationEnergetique(conso);
    }
    
    public int getConsommationEnergetique(){
        return this.conso;
    }
    
    private void setConsommationEnergetique(int conso){
        this.conso = conso;
    }

    public String getDescriptif(){
        return this.descriptif;
    }
    
    private void setDescriptif(String descriptif){
        this.descriptif = descriptif;
    }
    
    /**
     * Restitue des infos sur l'outil qui serviront à afficher un menu
     * @return
     */  
    public String getInfos(){
        return "("+this.getSymbole()+"), "+this.getNature()+" : \n"+this.getDescriptif();
    }
    
    public boolean isUtilisablePar(Joueur  j)
    {
        // à remplacer par la vérification que le crédit énergétique du joueur est suffisant pour utiliser l'outil
        return j.getBatterie()  >= this.getConsommationEnergetique();
    }
    
    public void utilise(Joueur j)
    {

        /*
            Utilisation de l'outil. Il faut vérifier que le joueur a assez d'énergie (Sinon, faire un message d'erreur),
            activer l'outil (différemment suivant l'outil) et diminuer l'énergie du joueur en conséquence
        */

        if(this.isUtilisablePar(j)){
            this.activation(j);
            j.diminuerEnergie(this.getConsommationEnergetique());
        }
        else
            System.out.println("Vous ne disposez pas d'assez d'énergie pour utiliser cet outil");
    }
    
    public abstract void activation(Joueur j); // activation spécifique à chaque outil

    @Override
    public boolean equals(Object autre)
    {
        if (this == autre) return true;
        if (autre == null) return false;
        if(getClass() != autre.getClass())
            return false;
        final Outil autreOutil = (Outil) autre;
        return Objects.equals(this.getSymbole(), autreOutil.getSymbole());
    }

    @Override
    public int hashCode() {
        int hash = 5;
        hash = 43 * hash + Objects.hashCode(this.descriptif);
        return hash;
    }
}
