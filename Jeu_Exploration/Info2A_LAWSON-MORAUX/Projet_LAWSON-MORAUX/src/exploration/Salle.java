/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package exploration;

import java.util.ArrayList;

/**
 *
 * @author jo
 */
public class Salle
{    
    private boolean visible;
    private Position position;
    private Plateau plateau;
    private Objet objet;
    private ArrayList<Direction> acces;
    
    public Salle(Position pos, Plateau p, Objet o)
    {
        // initialisations
        this.setVisible(false); // au départ, la salle n'est pas visible
        this.setPosition(pos);
        this.setPlateau(p);
        this.setObjet(o);
        this.initAcces();
    }
    
    public Salle(Position pos, Plateau p)
    {
        this(pos,p,null);
    }
    
    public void setVisible(boolean visible)
    {
        this.visible = visible;
    }
    
    
    public Position getPosition(){
        return this.position;
    }
    
    private void setPosition(Position position){
        this.position = position;
    }
    
    
    public Plateau getPlateau()
    {
        return this.plateau;
    }
    
    private void setPlateau(Plateau p){
        this.plateau = p;
    }

    /**
     * Restitue la référence à l'objet contenu dans la salle… ou null
     * @return une référence à un objet
     */
    public Objet getContenu() {
        return this.objet;
    }
    
    public void setObjet(Objet objet){
        this.objet = objet;
    }
    
    public String getSymbole()
    {
            if(this.getContenu() == null)
                return "  "; //On renvoie un espace s'il n'y a aucun objet dans la salle
            else
                //On Renvoie la représentation de l'objet contenu dans la salle
                return this.getContenu().toString();
    }
    
    /**
     * Restitue la salle contiguë dans la direction donnée… ou null si on sort du plateau
     * @param d
     * @return
     */
    public Salle getVoisine(Direction d)
    {
        Position posSuivante = this.getPosition().getSuivante(d);
        if(this.getPosition().isSuivanteValide(d)){
            return this.getPlateau().getSalle(posSuivante);
        }
        else{
            return null;
        }
    }
    
    /**
     * Crée un accès vers la salle contiguë dans la direction donnée… si l'accès n'existe pas déjà et si la salle contiguë existe
     * L'accès inverse est aussi ajouté (direction inverse ajoutée à la liste d'accès de la salle contiguë)
     * @param d La direction : haut, bas, gauche, droite
     */
    public void setAcces(Direction d)
    {
        //code
        //vérifions si la salle contigüe dans la direction donnée existe
        Salle salleContigue = this.getVoisine(d);

        if(salleContigue != null){
            // Vérifions si l'accès dans cette direction n'existe pas déjà 0.

            if(!this.isPossible(d))//si non
            {
                // On Crée un accès dans la direction donnée pour la salle actuelle
                this.acces.add(d);

                //Et Ajout de l'accès inverse pour la salle contigüe
                Direction DirectionInverse = d.getInverse();
                salleContigue.acces.add(DirectionInverse);
            }
        }
    }

    public boolean isVide(){
        return this.getContenu()==null;
    }
    
    /*
        Gère les accès aux salles contigües (horizontalement et verticalement). Au départ, la salle est emmurée : aucun accès.
        Quand un mur est détruit, on crée un accès vers la salle contiguë et un accès inverse à partir de cette salle
    */    
    public boolean isPossible(Direction d){
        return acces.contains(d);
    }
    
    /**
     * restitue true si la salle est visible
     * @return
     */
    public boolean isVisible()
    {
        return this.visible||this.getPlateau().isVisible();
    }
    
    private void initAcces(){
        this.acces = new ArrayList<Direction>();
    }//Création d'une liste d'accès vide
    
    public void entree(Joueur j)
    {
        //On vérifie s'il y'a un objet dans la salle
        if(this.getContenu() != null)
            //On déclenche l'interation avec le joueur
            this.getContenu().interaction(j);
    }
    
    @Override
    public  String toString()
    {
        if(this.isVisible()) return this.getSymbole();
        else return "░░"; // salle non visible
    }   
}
