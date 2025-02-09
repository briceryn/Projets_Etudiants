var vdiv2 = document.getElementById ("div2");
var now=new Date();
vdiv2.innerHTML=now;

function bouge()
    {
        var aiguilleSecondes = document.querySelector("#secondes");
        var aiguilleMinutes = document.querySelector("#minutes");
        var aiguilleHeures = document.querySelector("#heures");

        var maDate = new Date();
        var sec = maDate.getSeconds();
        var min = maDate.getMinutes();
        var h = maDate.getHours();

        console.log(sec);
        console.log(min);
        console.log(h);

        aiguilleSecondes.style.transform = "rotate(" + ((sec*6)-90) + "deg)";
        aiguilleMinutes.style.transform = "rotate(" + ((min*6)-90) + "deg)";
        aiguilleHeures.style.transform = "rotate(" + ((h*30)-90) + "deg)";
    }

window.setInterval(bouge, 1000);