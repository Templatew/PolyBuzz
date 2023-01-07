#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

//Parameters
const int clkPin  = 2;
const int dtPin   = 4;
const int swPin   = 7;

//Variables
int rotVal  = 0;
bool clkState  = LOW;
bool clkLast  = HIGH;
bool swState  = HIGH;
bool swLast   = HIGH;

int choix_mode = 0;
int temps = 0;
int etape = 0;
bool blocage_bouton = HIGH;




int tSec = 15;
int tMin = 1;
long timeInit=0;

const int bouton = 5;
int score = 0;
int etat_bouton = 0;
int astuce=0;


void setup() {
  lcd.init(); // initialisation de l'afficheur
  lcd.backlight();
  //Init Serial USB
  Serial.begin(9600);
  Serial.println(F("Initialize System"));
  //Init Rotary
  pinMode(clkPin,INPUT);
  pinMode(dtPin,INPUT);
  pinMode(swPin,INPUT_PULLUP);

  pinMode(bouton,INPUT);
  timeInit=timeToCounter(tMin,tSec);
}


void loop() {
  readRotary();

  if (temps == 0){
    int tSec = 30;
    int tMin = 0;
  }
    if (temps == 1){
    int tSec = 45;
    int tMin = 0;
  }
    if (temps == 2){
    int tSec = 0;
    int tMin = 1;
  }

  Serial.println(temps);
  
  if (etape == 4){
    lcd.clear();
    codeScore();
    codeTimer();
  } 
}


void readRotary( ) { /* function readRotary */
  ////Test routine for Rotary


  // Etape 1 : mode de jeu
  if (etape==0){
    // gestion position
    clkState = digitalRead(clkPin);
    if ((clkLast == LOW) && (clkState == HIGH)) { //rotary moving
        Serial.print("Mode de jeu : ");
        if (digitalRead(dtPin) == HIGH) {
            rotVal = rotVal - 1;
            if ( rotVal < 0 ) {
                rotVal = 3;
            }
        }
        else {
            rotVal++;
            if ( rotVal > 3 ) {
                rotVal = 0;
            }
        }

        if (rotVal==0) {
            Serial.println("Reflexe Facile");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("MODE DE JEU :");
            lcd.setCursor(0,1);
            lcd.print("REFLEXE EASY");
        }
        if (rotVal==1) {
            Serial.println("Reflexe Moyen");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("MODE DE JEU :");
            lcd.setCursor(0,1);
            lcd.print("REFLEXE NORMAL");
        }
        if (rotVal==2) {
            Serial.println("Reflexe Difficile");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("MODE DE JEU :");
            lcd.setCursor(0,1);
            lcd.print("REFLEXE HARD");
        }
        if (rotVal==3) {
            Serial.println("Simon");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("MODE DE JEU :");
            lcd.setCursor(0,1);
            lcd.print("SIMON");
        }
        choix_mode = rotVal;
        delay(200);
    }
  }


  // Etape 2 : reglage du temps
  if (etape==1 && choix_mode != 3){
    // gestion position
    clkState = digitalRead(clkPin);
    if ((clkLast == LOW) && (clkState == HIGH)) {//rotary moving
        Serial.print("Mode de jeu : ");
        lcd.setCursor(0,0);
        lcd.print("TEMPS DE JEU :  ");
        if (digitalRead(dtPin) == HIGH) {
            rotVal = rotVal - 1;
            if ( rotVal < 0 ) {
                rotVal = 2;
            }
        }
        else {
            rotVal++;
            if ( rotVal > 2 ) {
                rotVal = 0;
            }
        }

        if (rotVal==0) {
            Serial.println("00:30");
            lcd.setCursor(4,1);
            lcd.print("00:30");
        }
        if (rotVal==1) {
            Serial.println("00:45");
            lcd.setCursor(4,1);
            lcd.print("00:45");
        }
        if (rotVal==2) {
            Serial.println("01:00");
            lcd.setCursor(4,1);
            lcd.print("01:00");
        }
        temps = rotVal;
        delay(200);
    }
  }


  // Etape 3 : verification
  if (etape==2){
    Serial.println("Pret à jouer");
    Serial.print("Mode de jeu : ");
    if (choix_mode==0){Serial.print("Reflexe Facile");}
    if (choix_mode==1){Serial.print("Reflexe Moyen");}
    if (choix_mode==2){Serial.print("Reflexe Difficile");}
    if (choix_mode==3){Serial.print("Simon");}
    Serial.print(" / Temps de jeu : ");
    if (temps==0){Serial.println("00:30");}
    if (temps==1){Serial.println("00:45");}
    if (temps==2){Serial.println("01:00");}
    etape = 4;
    blocage_bouton = LOW;
    delay(2000);
  }

  // Etape 4 : preparation avant lancement
  if (etape==3){
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("LANCEMENT DANS");
      for(int i=5; i>0; i--){
          lcd.setCursor(7,1);
          lcd.print("0");
          lcd.setCursor(8,1);
          lcd.print(i);
          delay(1000);
      }
  }
  
  
  clkLast = clkState;
  //gestion bouton
  swState = digitalRead(swPin);
  if (swState == LOW && swLast == HIGH && blocage_bouton==HIGH) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("VALIDATION ETAPE");
      lcd.setCursor(7,1);
      lcd.print("0");
      lcd.print(etape);
      Serial.print("Validation de l'etape :");
      Serial.println(etape);
      etape += 1;
      delay(100);//debounce
  }
  swLast = swState;
}



void codeScore(){
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

void codeTimer(){
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
    lcd.clear();
    etape = 5;
    lcd.setCursor(0, 0);
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
