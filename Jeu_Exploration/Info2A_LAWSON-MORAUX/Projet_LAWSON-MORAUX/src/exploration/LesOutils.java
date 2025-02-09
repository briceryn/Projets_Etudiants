/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package exploration;
import java.util.ArrayList;

/**
 * Gère la liste des outils d'un joueur
 * @author jo
 */
public class LesOutils
{
    private ArrayList<Outil> liste;
    private Joueur proprietaire;
    
    public LesOutils(Joueur proprietaire)
    {
        this.setProprietaire(proprietaire);
        this.init();
    }
        
    public ArrayList<Outil> getListe()
    {
        return liste;
    }
    
    public void setListe(ArrayList<Outil> liste){
        this.liste = liste;
    }
  
    public Joueur getProprietaire(){
        return this.proprietaire;
    }
    
    private void setProprietaire(Joueur proprietaire){
        this.proprietaire = proprietaire;
    }
    
    public int getTaille(){
        return this.getListe().size();
    }
    
    public Outil get(int i){
        return this.getListe().get(i);
    }
    
    public void init()
    {
        this.setListe(new ArrayList<Outil>()); // Crée une liste d'outils vide
        this.ajoute(new ScannerUnidirectionnel()); // outil ajouté dès le départ
        this.ajoute(new DetecteurMines()); // idem
    }
    
    public void ajoute(Outil o)
    {
        if(this.getListe().contains(o))
            System.out.println("Vous est deja en possession de "+o.getNature());
        else{
            this.getListe().add(o);
            System.out.println("Vous recuperer " + o.getInfos());
            this.getProprietaire().getSalle().setObjet(null);
        }       
    }
    
    public void retire(Outil o){
        System.out.print("L'outil (" + o.getSymbole() + " " + o.getNature() + ") est utilise et retire de votre inventaire.");
        this.getListe().remove(o);
    }
}
