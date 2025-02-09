/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package exploration;


/**
 * À compléter
 * @author jo
 */
public class DetecteurMines extends Outil
{
    private boolean ramasser = false;
    
    public DetecteurMines()
    {
        super
        (
                "#?",
                "Detecteur de mines",
                "Permet de connaitre le nombre total de mines qui se situent dans les salles contigues.\nChaque utilisation consomme 3 unites d'energie.",
                3//coût énergétique de la détection des mines
        );
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
            reponse = Lire.S("\nSouhaitez-vous recuperer cette objet (oui/non)");
        }while(!reponse.equalsIgnoreCase("oui") && !reponse.equalsIgnoreCase("non"));
        if(reponse.equalsIgnoreCase("oui")){
            j.recupere();
            this.setRamasser(true);
        }
    }
    
    @Override
    public void activation(Joueur j)
    {
        /* explore les salles autour de la salle du joueur
        (direction à droite par rapport à la position de la salle du joueur,
        puis positions successives qui sont fondées sur les directions successives dans le sens trigonométrique : voir direction et position),
        compte les mines et affiche le résultat
        */
        int nbMines = 0;
        for(int i = 0;i<8;i++){
            Direction balayageSalle = new Direction(i);
            Position balayage = j.getSalle().getPosition();
            if(balayage.isSuivanteValide(balayageSalle)){
                Salle voisineBalayage = j.getSalle().getVoisine(balayageSalle);
                if(voisineBalayage.getContenu() != null){
                    if(voisineBalayage.getContenu().getNature().equals((new Mine()).getNature())){
                        nbMines += 1;
                    }
                }      
            }
        }
        System.out.println("\nIl y a " + nbMines + " mines autour de vous !");
    }
    
    @Override
    public String toString() {
        if(this.getRamasser() == false)
            return this.getSymbole();
        else
            return "  ";
    }
}
