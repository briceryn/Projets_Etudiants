/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package exploration;


/**
 *
 * @author jo
 */
public class ScannerUnidirectionnel extends Outil
{
    private static final int MARGE_ERREUR = 0;
    private Direction directionCourante;
    private boolean ramasser = false;
    
    public ScannerUnidirectionnel()
    {
        super
        (
                "->",
                "Scanner unidirectionnel a longue portee",
                "Detecte a travers les murs la distance a laquelle se situe le premier objet ou le mur du plateau.\nChaque utilisation consomme 2 unites d'energie.",
                2//coût énergétique de l'utilisation du scanner
        );
    }
    
    private int getMarge(){
        return ScannerUnidirectionnel.MARGE_ERREUR;
    }
     
    public Direction getDirectionCourante()
    {
        return this.directionCourante;
    }
    
    private void setDirectionCourante()
    {
        do
        {
            this.directionCourante = new Direction(Lire.S("\nEntrez une direction en combinant 'h','b','g','d' ou 'haut','bas','gauche','droite\nQuelle direction ?"));
        }while(!directionCourante.isValide());
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
        String reponse;
        do{
            reponse = Lire.S("Souhaitez-vous recuperer cette objet (oui/non)");
        }while(!reponse.equals("oui") && !reponse.equals("non"));
        if(reponse.equals("oui")){
            j.recupere();
            this.setRamasser(true);
        }
    }
    
    @Override
    public void activation(Joueur j)
    {
        /* recherche dans une direction demandée au joueur jusqu'à tomber sur une position null (en dehors de l'enceinte)
            ou une salle contenant un objet. Le nombre de salles parcourues est affiché à 20% près
        */
        this.setDirectionCourante();
        Salle scanSalle = j.getSalle().getVoisine((this.getDirectionCourante()));
        int nbSalles = 0;
        while(scanSalle != null && scanSalle.isVide()){
            nbSalles +=1;
            scanSalle = scanSalle.getVoisine(this.getDirectionCourante());       
        }
        int intervalleHaut = nbSalles*(1+this.getMarge()/100);
        int intervalleBas = nbSalles*(1-this.getMarge()/100);
        nbSalles = (int)(intervalleBas + (intervalleHaut-intervalleBas+1)*Math.random());
        System.out.println("\nIl y a " + nbSalles + " avant une salle avec un objet ou le bord du plateau dans la direction " + this.getDirectionCourante() + ", a " + this.getMarge() + " % près.");
    }
}
