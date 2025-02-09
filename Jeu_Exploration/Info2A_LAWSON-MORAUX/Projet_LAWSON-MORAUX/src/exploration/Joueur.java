/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package exploration;

/**
 * À compléter
 * @author jo
 */
public class Joueur
{    
    private String nom;
    private Position position;
    private LesOutils outils;
    private boolean perdant = false;
    private boolean gagnant = false;
    private ReserveEnergie Batterie ;
    private CaisseGrenades Munitions;
    
    public Joueur(String nom,Position position)
    {
        //code
        this.setIntroNom();
        this.setPosition(position);
        this.setOutils(new LesOutils(this));
    }

    public Joueur(Position position)
    {
        //code
        this.setPosition(position);
    }
    
    public String getNom(){
        return this.nom;
    }
    
    private void setIntroNom()
    {
        System.out.print("Quelle est votre nom : ");
        this.nom = Lire.S();
        System.out.println("\nBienvenue " + this.getNom() + ", vous voila captif.ve de cette grotte. Le but pour vous sera d'en sortir."
                + "\nCette grotte se compose de salles qui sont toutes scellees par la roche. Pour progresser il va falloir détruire ces murs avec des grenades..."
                + "\nCependant personne de sait ce qui se cache derriere ! Detruire un mur vous attirera dans la pièce. Attention certaines sont piegees avec des MINES (**) !"
                + "\nVous possedez un inventaire de grenades, d'energies et d'objets."
                + "\nEn fouillant vos poches, vous trouvez 9 grenades et 9 rations qui sont de l'energie." 
                + "\nAinsi que des objets :");
    }
     
    /**
     * Position du joueur
     * @return une référence à une position
     */
    public Position getPosition(){
        return this.position;
    }
    
    private void setPosition(Position position)
    {
        // affectation de la position en déclenchant la méthode d'entrée de la salle
        this.position = position;
        this.getSalle().entree(this);
    }
    
    /**
     * Salle où se situe le joueur
     * @return une référence à une salle
     */
    public Salle getSalle()
    {
        return this.position.getPlateau().getSalle(this.position);
    }
    
    public LesOutils getOutils()
    {
        return this.outils;
    }
    
    private void setOutils(LesOutils outils)
    {
        this.outils = outils;
    }
    
    public int getBatterie() {
        return Batterie.getDisponible();
    }

    public void setBatterie(ReserveEnergie energie){
        this.Batterie = energie;
    }
    
    public int getMunitions() {
        return Munitions.getDisponible();
    }
    
    public void setMunitions(CaisseGrenades munitions){
        this.Munitions = munitions;
    }
    
    public void setPerdant(boolean perdant){
        this.perdant = perdant;
    }
    
    
    public void setGagnant(Boolean gagnant){
        this.gagnant=gagnant;
    }
    
    public boolean isPerdant(){
        return this.perdant;
    }
    
    public boolean isGagnant(){
        return this.gagnant;
    }
    
    public void augmenterEnergie(int nbUnites){
        if(this.Batterie.getDisponible()+nbUnites <= 9)
            this.Batterie.setDisponible(this.Batterie.getDisponible()+nbUnites);

    }

    public void diminuerEnergie(int nbUnites){
        if(this.Batterie.getDisponible() - nbUnites >= 0 )
            this.Batterie.setDisponible(this.Batterie.getDisponible() - nbUnites);

    }

    public void augmenterMunitions(int nbUnites){
        if(this.Munitions.getDisponible()+nbUnites <= 9 )
            this.Munitions.setDisponible(this.Munitions.getDisponible()+nbUnites);

    }

    public void diminuerMunitions(int nbUnites){
        if(this.Munitions.getDisponible() - nbUnites >= 0 )
            this.Munitions.setDisponible(this.Munitions.getDisponible() - nbUnites);

    }
    
    /**
     * Récupération d'un outil trouvé dans une salle
     */
    public void recupere()
    {
        //Cette méthode est déclenchée par l'interaction avec l'outil
        this.getOutils().ajoute((Outil)getSalle().getContenu());
    }
    
    public void utilise(Outil o){
        o.utilise(this);
        this.getOutils().retire(o);
    }

    /**
     * Avance dans une direction donnée à condition que le mur soit ouvert dans cette direction
     * @param d direction dans laquelle avancer
     */
    public void avance(Direction d)
    {
        //Recuperons la position actuelle du joueur
        Position CurrentPosition = this.getPosition();
        if(!CurrentPosition.isSuivanteValide(d)){
            System.out.println("\nVous ne pouvez pas sortir du plateau");
        }else{
            //Recuperons la position suivante en fonction de la direction
            Position NextPosition = CurrentPosition.getSuivante(d);
            //Obtenir la salle dans la direction donnée
            Salle NextRoom = this.position.getPlateau().getSalle(NextPosition);
            /*
                code : il faut gérer la sortie du plateau, le fait qu'un mur soit ou non ouvert et remettre à jour la position du joueur
            */
            if(this.getSalle().isPossible(d)){
                this.setPosition(NextRoom.getPosition());               
            }else{
                if(NextRoom.isVisible()){
                    this.getSalle().setAcces(d);
                    this.setPosition(NextRoom.getPosition());
                }else{
                    System.out.println("\nIl y a un mur, detruisez le !");
                }
            }
        } 
    }  
    
    public void lanceGrenade(Direction d)
    {
        /*
            La grenade est perdue si un mur est déjà ouvert dans la direction spécifiée
            Sinon,un accès est ajouté à la salle courante vers la salle contiguë dans la direction d (et réciproquement)
            et le joueur est « aspiré » dans la salle nouvellement ouverte. La réserve de grenades du joueur est décrémentée
        */
        Position CurrentPosition = this.getPosition();
        if(CurrentPosition.isSuivanteValide(d)){
            if(this.getMunitions()>0){
                Position NextPosition = this.getPosition().getSuivante(d);
                this.diminuerMunitions(1);
                if(NextPosition.getPlateau().getSalle(NextPosition).isVisible()){                       
                    System.out.println("\nSalle deja accessible, vous avez perdu une grenade !");
                }else{
                    NextPosition.getPlateau().getSalle(NextPosition).setVisible(true);
                    System.out.println("\nSalle maintenant accessible. Vous avancez dedans...");
                    this.avance(d);
                }
            }else{
                System.out.println("\nVous n'avez plus de grenades !");
            }
        }else{
            System.out.println("Vous ne pouvez pas detruire ce mur !");
        }
    }
}
