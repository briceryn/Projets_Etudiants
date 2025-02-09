/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package exploration;

/**
 *
 * @author jo
 */
public class Mine extends Objet
{
    public Mine(){
        super(
                "**",
                "Mine"
        );
    }
    
    @Override
    public void interaction(Joueur j)
    {
        System.out.println("Vous avez marche sur une mine... Vous etes mort ! PERDU");
        j.setPerdant(true);
    }
}