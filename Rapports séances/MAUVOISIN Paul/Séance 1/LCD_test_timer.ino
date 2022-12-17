#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int tSec = 15;
int tMin = 1;
long timeInit=0;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup(){
  lcd.init(); // initialisation de l'afficheur

  timeInit=timeToCounter(tMin,tSec);
}

void loop(){
  lcd.backlight();
  counterToTime(timeInit);

  //Timer normal
  if(timeInit != 0){
    //Affichage de base
    lcd.setCursor(0,1);
    lcd.print("TEMPS: ");
    lcd.setCursor(8,1);
    lcd.print(":");

    //Affichage min
    lcd.setCursor(7, 1);
    lcd.print(tMin);

    //Affichage sec, problème de décalage des sec inférieures à 10
    if (tSec > 9){
      lcd.setCursor(9, 1); 
      lcd.print(tSec);
    }

    else{
      lcd.setCursor(9, 1); 
      lcd.print(0);
      lcd.setCursor(10, 1);
      lcd.print(tSec);
    }

    //Faire passer 1sec
    delay(1000);
    timeInit=timeInit-1;
   }

   //Timer arrivé à la fin
   if (timeInit==0) {
    lcd.setCursor(0, 1);
    lcd.print("FIN DE LA PARTIE");
   }
}

//Conversion en sec du Timer
int timeToCounter(int m,int s){
  return m*60+s;
}


void counterToTime(long cnt){
  tMin = (cnt%3600)/60;
  tSec = ((cnt%3600)%60);
}
