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
public class Plateau
{
    private static final String BORD = "░░";
    public static boolean VISIBLE = true; // mode déboggage : si true, toutes les salles doivent afficher leur contenu
    private Jeu jeu;
    private int NbLig;
    private int NbCol;
    private boolean visible;
    private Salle[][] grille;
    private Joueur joueur;
    
    public Plateau(int nbLig, int nbCol, Jeu jeu)
    {
        this.jeu = jeu;
        this.grille = new Salle[nbLig][nbCol];
        // Crée un joueur et initialise le plateau selon les spécifications du jeu (Gros boulot)
        this.setNbLig(nbLig);
        this.setNbCol(nbCol);
        this.setJeu(jeu);
        for(int i=1;i<=getNbLig();i++)
        {
            for(int j=1;j<=getNbCol();j++)
            {
                this.placeSalle(new Salle(new Position(i,j,this),this));
            }
        }
        this.initSortie();
    }
    
    public Jeu getJeu(){
        return this.jeu;
    }
    
    private void setJeu(Jeu jeu){
        this.jeu = jeu;
    }
    
    public Joueur getJoueur(){
        return this.joueur;
    }
    
    public void setJoueur(Joueur joueur){
        this.joueur = joueur;
    }
    
    public Position getPosJoueur(){
        return this.getJoueur().getPosition();
    }
    
    public void setNbLig(int nblig){
        this.NbLig=nblig;
    }
    
    public void setNbCol(int nbcol){
        this.NbCol=nbcol;
    }
    
    public void setVisible(boolean visible){
        this.visible = visible;
    }

    
    public int getNbLig(){
        return this.grille.length;
    }
    
    public int getNbCol(){
        return this.grille[0].length;
    }
    
    public int getNbSalles(){
        return this.getNbCol()*this.getNbLig();
    }
    
    public Salle getSalle(Position p)
    {
        if(p.isValide()) return this.grille[p.getLig()-1][p.getCol()-1];
        else return null;
    }
    
    public Salle getSalle(int lig, int col){
        return this.getSalle(new Position(lig,col,this));
    }

    /**
     * Place une salle à une position donnée (Les positions en Lig et Col commencent à 1)
     * @param s Une salle
     */
    public void placeSalle(Salle s)
    {
        Position p = s.getPosition();
        if(!p.isValide()) throw new RuntimeException("Affectation de la salle à une position non valide");
        this.grille[p.getLig()-1][p.getCol()-1] = s;       
    }

    /**
     * Crée une salle contenant un objet référencé par o (vide si o = null) et l'affecte au plateau
     * @param p la position dans le plateau
     * @param o l'objet que la salle doit contenir
     */
    public void setNouvelleSalle(Position p, Objet o)
    {
        placeSalle(new Salle(p,this,o));       
    }
    
    public void setNouvelleSalle(Position p){
        setNouvelleSalle(p, null);
    }
    
    public boolean isVisible(){
        return this.visible||Plateau.VISIBLE;
    }

    /**
     *
     * @return
     */
    @Override
    public String toString()
    {
        StringBuilder PlateauBuilder = new StringBuilder();
        for(int i = 0; i<this.getNbLig()+2; i++){
            for (int j = 0; j<this.getNbCol()+2; j++){
                if(i==0 || j==0 || i == this.getNbLig()+1 || j == this.getNbCol()+1){
                    PlateauBuilder.append(BORD);
                }else{
                    if(this.getPosJoueur().equals(this.getSalle(i, j).getPosition())){
                        PlateauBuilder.append("||");
                    }else{
                        PlateauBuilder.append(grille[i-1][j-1].toString());
                    }
                }           
            }
            PlateauBuilder.append("\n");//Ajouter un saut de ligne entre les lignes du plateau
        }
        return PlateauBuilder.toString();// restitue la chaîne qui représente l'ensemble du plateau en combinant les toString des salles (voir exemple dans le sujet)
    }
    
    private void initSortie(){
        int random = (int)(Math.random()*4);
        switch(random){
            case 0: this.getSalle(1,1).setObjet(new Sortie()); break;
            case 1: this.getSalle(this.getNbLig(),1).setObjet(new Sortie()); break;
            case 2: this.getSalle(1,this.getNbCol()).setObjet(new Sortie()); break;
            case 3: this.getSalle(this.getNbLig(),this.getNbCol()).setObjet(new Sortie()); break;
        }
    }
    
    public void initObjets(Objet[] objets){
        int randomSalle;
        int randomObjet;
        Salle salle;
        int nbObjets = 0;
        int objetNull = 0;
        for(int i=0;i<objets.length;i++){
            if(objets[i]==null){
                objetNull+=1;
            }
        }
        while(nbObjets<objets.length-objetNull){
            randomSalle = (int) (this.getNbSalles()*Math.random());
            randomObjet = (int) (objets.length*Math.random());
            salle = this.getSalle(new Position(randomSalle, this));
            if(salle.isVide()==true && salle.getPosition()!=this.getJoueur().getPosition() && objets[randomObjet]!=null){
                salle.setObjet(objets[randomObjet]);
                objets[randomObjet]=null;
                nbObjets+=1;
            }
        }
    }
}
