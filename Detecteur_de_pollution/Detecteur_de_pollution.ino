#include <DHT.h>
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define PIN_MQ2 A0

#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int LED_Verte = 9; 
int LED_Rouge = 10;
int LED_Orange = 8;
const int buzzer =3; // Broche pour le buzzer

void setup(){
  Serial.begin(9600);
  // Initialise la capteur DHT11
  dht.begin();

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  //lcd.print("hello world");
  lcd.setCursor(0,0);
  pinMode(PIN_MQ2, INPUT);

  //pinMode(gaz, INPUT); // On initialise le capteur en entrée pour lire sa valeur
  pinMode(buzzer, OUTPUT); // On initialise le buzzer en sortie pour le contrôler

  // On branche les leds en sortie
  pinMode(LED_Verte, OUTPUT);
  pinMode(LED_Orange, OUTPUT);
  pinMode(LED_Rouge, OUTPUT);
  lcd.write((uint8_t)0);
  digitalWrite(buzzer, LOW); // On laisse le buzzer éteint


}
void loop(){
  int valeur = analogRead(PIN_MQ2);
  float pourcentage = (valeur/1023.0)*100.0; // On transforme la valeur en pourcentage
  Serial.println("Gaz:" + String(pourcentage)+ "%");
  lcd.setCursor(0,0);
  lcd.println( "GAZ"+ String(pourcentage)+ " %");
  allume_led_buz();
  delay(1000);
  affiche_temp_hum();
  Serial.println(" ");
  delay(5000);
  

}
void allume_led_buz(){
  int valeur = analogRead(PIN_MQ2); // On lit la valeur du capteur de gaz
  float pourcentage = (valeur/1023.0)*100.0; // On transforme la valeur en pourcentage
  if (pourcentage>=30  && pourcentage<50){
    digitalWrite(LED_Verte, HIGH);
    lcd.setCursor(1,0);
    lcd.println("ALERTE NIVEAU 1");
    delay(3000);
    lcd.clear();
    digitalWrite(LED_Verte,LOW);
  }
  if(pourcentage>=50 && pourcentage<80){
    digitalWrite(LED_Orange,HIGH); // On allume la led orange si le pourcentage est supérieur à 50 %
    digitalWrite(LED_Verte,HIGH);
    lcd.setCursor(1,0);
    lcd.println("ALERTE NIVEAU 2");
    delay(3000);
    lcd.clear();
    digitalWrite(LED_Orange,LOW);
    digitalWrite(LED_Verte,LOW);

  }
  if (pourcentage>=80){
    digitalWrite(LED_Rouge, HIGH); // On allume la led rouge si le pourcentage est supérieur à 80 %
    digitalWrite(LED_Orange,HIGH);
    digitalWrite(LED_Verte,HIGH);
    tone(buzzer, 494);// On allume le buzzer
    lcd.setCursor(1,0);
    lcd.println("ALERTE NIVEAU 3");
    delay(4000); // On attends une demi-seconde
    lcd.clear();
    digitalWrite(LED_Rouge,LOW);
    digitalWrite(LED_Orange,LOW);
    digitalWrite(LED_Verte,LOW);
    noTone(buzzer);// On stop le buzzer 

  }
}
void affiche_temp_hum(){
  //lcd.clear();
  Serial.println("Temperature = " + String(dht.readTemperature())+" °C");
  lcd.setCursor(0,0);
  lcd.println("TEMPERATURE:" + String(dht.readTemperature()));
  delay(4000);
  lcd.clear();
  delay(1000);
  Serial.println("Humidite=" + String(dht.readHumidity())+"%");
  lcd.setCursor(0,0);
  lcd.println("HUMIDITE:" + String(dht.readHumidity()));
  delay(5000);
  lcd.clear();
}