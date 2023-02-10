#include <Servo.h>

Servo monservo;  // crée l’objet pour contrôler le servomoteur
int valeur = 0;

void setup(){
  Serial.begin(9600);
  monservo.attach(9);  // utilise la broche 9 pour le contrôle du servomoteur
  monservo.write(0);  // positionne le servomoteur à 0°
  delay(1000);
}

void loop() {
  if (valeur == 0){
    monservo.write(0);
    Serial.println("fermeture");
    delay(1000);
    valeur = 1;
  }
  if (valeur == 5){
    monservo.write(40);
    Serial.println("ouverture");
    delay(500);
    valeur = 0;
  }
  else{
    valeur += 1;
    delay(5000);
  }
}
