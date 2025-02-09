/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package exploration;

/**
 * À compléter
 * @author jo
 */
public class Jeu
{
    private Plateau plateau;
    private Categorie[] listeCategories;
    private int proportionVides;
    
    public Jeu(int nbLig, int nbCol, int proportionVides, Categorie... listeCategories)
    {
        /*  initialisations du jeu : création d'un plateau et exécution de joue()
            nbLig et nbCol sont les tailles du plateau (Elles peuvent être fixes au moins au début)
        */
        this.setProportionVides(proportionVides);
        this.setListeCategories(listeCategories);
        Objet[] objets = this.initObjets(nbCol*nbLig-2);
        this.setPlateau(new Plateau(nbLig, nbCol, this));
        Position p = new Position(Math.round((this.getPlateau().getNbLig()+1)/2), Math.round((this.getPlateau().getNbCol()+1)/2), this.getPlateau());
        Joueur j = new Joueur("", p);
        this.getPlateau().setJoueur(j);
        this.getPlateau().getSalle(this.getPlateau().getPosJoueur()).setVisible(true);
        j.setBatterie(new ReserveEnergie(9));
        j.setMunitions(new CaisseGrenades(9));
        this.getPlateau().initObjets(objets);
        this.joue();
    }
    
    public Plateau getPlateau(){
        return this.plateau;
    }
    
    public void setPlateau(Plateau plateau){
        this.plateau = plateau;
    }  

    
    public void setListeCategories(Categorie[] listeCategories){
        this.listeCategories = listeCategories;
    }
    
    public Categorie[] getListeCategories(){
        return this.listeCategories;
    }
    
    public Categorie getCategorie(int i){
        return this.listeCategories[i];
    }

    /**
     * Proportion de salles vides par rapport à l'ensemble des salles : 50 signifie que 50% des salles du plateau sont vides
     * @return
     */
    public int getProportionVides(){
        return this.proportionVides;
    }
    
    private void setProportionVides(int proportionVides){
        this.proportionVides = proportionVides;
    }
    
     /**
     * Restitue le joueur qui a été créé dans le plateau (Le joueur pourrait aussi être référencé dans une instance de jeu)
     * @return
     */
    public Joueur getJoueur(){
        return this.getPlateau().getJoueur();
    }
    
    public boolean isFini(){
        return this.getJoueur().isGagnant() || this.getJoueur().isPerdant();
    }
    
    public Objet[] initObjets(int taille){
        Objet[] objets = new Objet[taille-Math.round(taille*this.getProportionVides()/100)];
        int nbTotal = 0;
        for(int i = 0 ; i<this.getListeCategories().length ; i++) // Parcours des catégories
        {
            nbTotal = nbTotal+this.getCategorie(i).getProportion();
        }
        // nbTotal contient désormais la somme des proportions des différents objets à créer
        /* crée autant d'objets qu'il y a de cases dans le tableau d'objets (ici 100) dans les proportions voulues */
        int indiceObjet = 0;
        for(int indiceCategorie = 0 ; indiceCategorie<this.getListeCategories().length ; indiceCategorie++)
        {
            int nbObjets = objets.length*this.getCategorie(indiceCategorie).getProportion()/nbTotal; // calcule le nombre d'objets nbObjets de la catégorie courante à créer

            for(int compteurObjets = 1 ; compteurObjets<= nbObjets ; compteurObjets++,indiceObjet++) // crée nbObjets instances de la catégorie courante
            {   
                Objet o = this.getCategorie(indiceCategorie).getNouveau();
                objets[indiceObjet] = o; // crée une instance de la catégorie courante avec le constructeur par défaut
            }
        }
        return objets;
    }
    
    public void menuChoix()
    {
        System.out.println("\nVoici la carte :\n"+this.getPlateau().toString());
        System.out.println("Tu disposes de "+this.getJoueur().getMunitions()+" grenades et de "+this.getJoueur().getBatterie()+" energies.\n");
        System.out.println("Voici vos options :\n  1 - Avancer\n  2 - Lancer une Grenade\n  3 - Utiliser un outil");
        String choix= Lire.S("Votre choix");
        switch (choix) {
            case "1":{
                getJoueur().avance(new Direction(Lire.S("\nEntrez une direction en combinant 'h','b','g','d' ou 'haut','bas','gauche','droite\nQuelle direction ?")));
                break;
            }
            case "2":{
                getJoueur().lanceGrenade(new Direction(Lire.S("\nEntrez une direction en combinant 'h','b','g','d' ou 'haut','bas','gauche','droite\nQuelle direction ?")));
                break;
            }
            case "3":{
                this.menuOutils();
                break;
            }
            default:{
                this.menuChoix();
            }
        }
    }
    
    public void menuOutils()
    {
        System.out.println("\nVoici votre inventaire : ");
        int i=1;
        for(Outil o : this.getJoueur().getOutils().getListe())
        {
            System.out.println("  "+i+" - "+o.getNature());
            i++;
        }
        System.out.println("  "+i+" - Retour au menu");
        int reponse = Lire.i("Votre choix");
        if(reponse<i){
            this.getJoueur().utilise(this.getJoueur().getOutils().get(reponse-1));
            System.out.println();
        }else{
            this.menuChoix();
        }       
    }
    
    public void joue()
    {
        do {
            menuChoix();
        } while (!isFini());
    }
}
