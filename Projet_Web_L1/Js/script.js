document.addEventListener("DOMContentLoaded", function() {
    var canvas = document.getElementById("canvas");
    var context = canvas.getContext("2d");

    // Définir les couleurs
    var backgroundColor = "#F1EFEF";
    var curveColor1 = "#7DC2A5";
    var curveColor2 = "#404040";
    var textColor = "#404040";

    // Définir les propriétés du canva(logo NatureUnity)
    canvas.width = 300;  
    canvas.height = 150; 
    context.fillStyle = backgroundColor;
    context.fillRect(0, 0, canvas.width, canvas.height);

    // courbe de Bézier de degré 2
    context.beginPath();
    context.moveTo(canvas.width * 0.2, canvas.height * 0.8);
    context.quadraticCurveTo(canvas.width * 0.5, canvas.height * 0.2, canvas.width * 0.8, canvas.height * 0.8);
    context.strokeStyle = curveColor1;
    context.lineWidth = 4;
    context.stroke();

    //courbe de Bézier de degré 3
    context.beginPath();
    context.moveTo(canvas.width * 0.1, canvas.height * 0.5);
    context.bezierCurveTo(canvas.width * 0.3, canvas.height * 0.1, canvas.width * 0.7, canvas.height * 0.9, canvas.width * 0.9, canvas.height * 0.5);
    context.strokeStyle = curveColor2;
    context.lineWidth = 4;
    context.stroke();


    var boutonAfficherObjet = document.getElementById("boutonAfficherObjet");
            boutonAfficherObjet.addEventListener("click", function() {
                // Appel de la fonction pour afficher l'objet
                afficherObjet(context, 150, 100, 20, "red");
            });
        });

    function afficherObjet(context, x, y, size, color) {
    context.beginPath();
    context.moveTo(x, y);
    context.bezierCurveTo(x - size, y - size, x - 2 * size, y + size, x, y + 2 * size);
    context.bezierCurveTo(x + 2 * size, y + size, x + size, y - size, x, y);
    context.fillStyle = color;
    context.fill();

    
}
    


var couleurPicker = document.getElementById("couleurPicker");
couleurPicker.addEventListener("input", function() {
    // Appeler la fonction pour modifier le fond du canvas
        changerCouleurFond(context, couleurPicker.value);
});

function changerCouleurFond(couleur) {
var canvas = document.getElementById("canvas");
var context = canvas.getContext("2d");

// Changer la couleur de fond
context.fillStyle = couleur;
context.fillRect(0, 0, canvas.width, canvas.height);
changerCouleurFond(couleur);
}



function choisirValeur(valeur) {
    
    console.log("Valeur choisie :", valeur);

    // Afficher un message dans une alerte
    alert("Vous avez choisi la valeur : " + valeur);
}

function choisirValeurs(valeur) {
    console.log("Valeur choisie :", valeur);

     //Afficher un message dans une alerte
    alert("Vous avez choisi la valeur : " + valeur);
}

