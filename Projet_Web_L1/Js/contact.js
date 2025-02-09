var colorWell;
var defaultColor = "#0000ff";

window.addEventListener("load", startup, false);

function startup() {
  colorWell = document.querySelector("#colorWell");
  colorWell.value = defaultColor;
  colorWell.addEventListener("input", updateFirst, false);
  colorWell.addEventListener("change", updateAll, false);
  colorWell.select();
}

function updateFirst(event) {
  var p = document.querySelector("p");

  if (p) {
    p.style.color = event.target.value;
  }
}

function updateAll(event) {
  document.querySelectorAll("p").forEach(function (p) {
    p.style.color = event.target.value;
  });
}

function nom(){
  var str = document.forms["form"].nom.value;
  if( !str.replace(/\s+/, '').length ) {
       alert( "Le champ nom est vide!" );
       return false;
  }
}

function prénom(){
  var str = document.forms["form"].prénom.value;
  if( !str.replace(/\s+/, '').length ) {
       alert( "Le champ prénom est vide!" );
       return false;
  }
}

function tel(){
  var str = document.forms["form"].tel.value;
  if( !str.replace(/\s+/, '').length ) {
       alert( "Le champ tel est vide!" );
       return false;
  }
}


function mail(){
  var str = document.forms["form"].mail.value;
  if( !str.replace(/\s+/, '').length ) {
       alert( "Le champ mail est vide!" );
       return false;
  }
}

function date(){
  var str = document.forms["form"].date.value;
  if( !str.replace(/\s+/, '').length ) {
       alert( "Le champ date est vide!" );
       return false;
  }
}