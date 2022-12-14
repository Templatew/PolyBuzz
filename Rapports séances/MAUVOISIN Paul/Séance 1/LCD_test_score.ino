#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int bouton = 5;
int score = 0;
int etat_bouton = 0;
int astuce=0;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup(){
  pinMode(bouton,INPUT);
  lcd.init(); // initialisation de l'afficheur
}

void loop(){
  lcd.backlight();
  // Envoi du message
  lcd.setCursor(0, 0);
  lcd.print("SCORE: ");

  etat_bouton = digitalRead(bouton); 
  if ((etat_bouton==LOW)&&(astuce==0)){
    astuce=1;
    score += 1;
    lcd.setCursor(7, 0);
    lcd.print(score);
  }
  
  delay(15);
  
  if (etat_bouton==HIGH){
    astuce=0;
  }
}
