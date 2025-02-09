/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package exploration;

/**
 *
 * @author
 */
public class Sortie extends Objet {
    
    public Sortie(){
        super(
                "<>",
                "Sortie"
        );
    }
    
    public void interaction(Joueur j)
    {
        System.out.println("Vous avez trouve la sortie, vous pouvez enfin vous echapper ! BRAVO");
        j.setGagnant(true);
    }   
}
