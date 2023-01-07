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
}


void loop() {
  readRotary();
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
    etape = 3;
    blocage_bouton = LOW;
    delay(200);
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
