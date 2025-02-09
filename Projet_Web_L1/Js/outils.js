// On va appliquer une gestion des évènements pour chaque bouton
// Methode 1 : on a déjà l'évènement dans le html, à proscrire sinon
  
// 1. Fonction addition
function Additionne() {
    // deux variables input et une variable résultat
    var input1 = "";
    var input2 = "";
    var resultat = "";
    // on attache les variables à des éléments du HTML
    input1 = document.getElementById("input1").value;
    input2 = document.getElementById("input2").value;
    // et enfin le calcul
    resultat = parseInt(input1) + parseInt(input2);
    // On affiche
    console.log("addition !");
    console.log(resultat);
    document.getElementsByClassName("resultat")[0].innerHTML = input1+' + '+input2+' = '+resultat+' <br>';
}
  
// 2. Fonction soustraction
function Soustract() {
      // deux variables input et une variable résultat
      var input1 = "";
      var input2 = "";
      var resultat = "";
      // on attache les variables à des éléments du HTML
      input1 = document.getElementById("input1").value;
      input2 = document.getElementById("input2").value;
      // et enfin le calcul
      resultat = input1 - input2;
      // On affiche
      console.log("soustraction !");
      console.log(resultat);
      document.getElementsByClassName("resultat")[0].innerHTML = input1+' - '+input2+' = '+resultat+' <br>';
 
      }
  
// 3. Fonction multiplication
function Multiply() {
      // deux variables input et une variable résultat
      var input1 = "";
      var input2 = "";
      var resultat = "";
      // on attache les variables à des éléments du HTML
      input1 = document.getElementById("input1").value;
      input2 = document.getElementById("input2").value;
      // et enfin le calcul
      resultat = input1 * input2;
      // On affiche
      console.log("multiplication !");
      console.log(resultat);
      document.getElementsByClassName("resultat")[0].innerHTML = input1+' * '+input2+' = '+resultat+' <br>';
  
}
  
// 4. Fonction division
function Divide() {
      // deux variables input et une variable résultat
      var input1 = "";
      var input2 = "";
      var resultat = "";
      // on attache les variables à des éléments du HTML
      input1 = document.getElementById("input1").value;
      input2 = document.getElementById("input2").value;
      // et enfin le calcul
      resultat = input1 / input2;
      // On affiche
      console.log("division !");
      console.log(resultat);
      document.getElementsByClassName("resultat")[0].innerHTML = input1+' / '+input2+' = '+resultat+' <br>';
}
  
//PB : Les opérations ne semblent pas se faire, j'ai limpression que le programme
// ne reconnait pas les valeurs saisies dans les zones input
// Comme des éléments à calculer
// vérification faite avec console log




/* //////////////////////////////////////////////
  Plus d'informations liées à la solution  : 
  //////////////////////////////////////////////

*/

/*
Toutes le fonctions ci-dessous peuvent être optimisées
elles sont même volontairement non optimisées
Elles sont là juste pour vous présenter le concept à vous de les améliorer 
*/

function setMode(oEvent){
      document.body.classList.replace(aModes[Number(!this.checked)],aModes[Number(this.checked)]);
      localStorage.setItem('mode',aModes[Number(this.checked)]);
      this.parentNode.nextElementSibling.innerHTML = (this.checked)? "Dark mode":"White mode"
    }
    
    function loadMode(){
      let sMode = localStorage.getItem('mode');
      document.forms["mytheme"]["theme-mode"].checked = sMode == aModes[1];
      setMode.call(document.forms["mytheme"]["theme-mode"])
    }
    let aModes = ["mode-white", "mode-dark"];
    document.addEventListener('DOMContentLoaded',function(){
      document.body.classList.add(aModes[0]);
      document.forms["mytheme"]["theme-mode"].addEventListener('click', setMode);
      loadMode()
    });