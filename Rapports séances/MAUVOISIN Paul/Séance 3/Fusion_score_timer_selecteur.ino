 #include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

//Parametres pour l'encodeur rotatif
const int clkPin  = 2;
const int dtPin   = 4;
const int swPin   = 7;

//Variables pour l'encodeur rotatif
int rotVal  = 0;
bool clkState  = LOW;
bool clkLast  = HIGH;
bool swState  = HIGH;
bool swLast   = HIGH;

//Variables pour la fonction: readRotaty
int choix_mode = 0;
int temps = 0;
int etape = 0;
bool blocage_bouton = HIGH;

//Variables pour la fonction: Timer
int tSec = 15;
int tMin = 1;
unsigned long timeInit=0;
unsigned long intervalleTemps = 1000;
unsigned long initialeTemps = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;


void setup() {
  // Initialisation de l'afficheur
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Bienvenue");
  lcd.setCursor(2,1);
  lcd.print("Tournez-moi!");
  
  //Initialisation Serial
  Serial.begin(9600);
  Serial.println(F("Initialize System"));
  
  //Initialisation encodeur rotatif
  pinMode(clkPin,INPUT);
  pinMode(dtPin,INPUT);
  pinMode(swPin,INPUT_PULLUP);
}


void loop() {
  //Post gameplay
  readRotary();
  
  //Transition entre temps choisit par le joueur
  //et le temps que va durer la partie
  if (etape == 4){
    lcd.clear();
    if (temps == 0){
      tSec = 30;
      tMin = 0;
    }
    if (temps == 1){
      tSec = 45;
      tMin = 0;
    }
    if (temps == 2){
      tSec = 0;
      tMin = 1;
    }
    timeInit=timeToCounter(tMin,tSec);
    etape = 5;
  }

  //Etape 5 : le jeu est lancé
  if (etape==5){
    Serial.println(tSec);//ok
    Serial.println(etape);
    //Fonctions qui affichent le Timer
    //et le score selon le mode de jeu !!Par encore dissocié!!
    codeScore();
    codeTimer();
  }
  
}


void readRotary( ) {
  //Etape 1 : mode de jeu
  if (etape==0){
    // Gestion position
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

        // Selection mode de jeu en tournant l'encodeur rotatif
        if (rotVal==0) {
            Serial.println("Reflexe Facile");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Mode de Jeu :");
            lcd.setCursor(0,1);
            lcd.print("Reflexe Easy");
        }
        if (rotVal==1) {
            Serial.println("Reflexe Moyen");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Mode de Jeu :");
            lcd.setCursor(0,1);
            lcd.print("Reflexe Normal");
        }
        if (rotVal==2) {
            Serial.println("Reflexe Difficile");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Mode de Jeu :");
            lcd.setCursor(0,1);
            lcd.print("Reflexe Hard");
        }
        if (rotVal==3) {
            Serial.println("Simon");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Mode de Jeu :");
            lcd.setCursor(0,1);
            lcd.print("Simon");
        }
        choix_mode = rotVal;
        delay(200);
    }
  }


  // Etape 2 : reglage du temps
  if (etape==1 && choix_mode != 3){  // Le Simon n'aura pas besoin de temps
    // gestion position
    clkState = digitalRead(clkPin);
    if ((clkLast == LOW) && (clkState == HIGH)) {//rotary moving
        Serial.print("Temps de jeu : ");
        lcd.setCursor(0,0);
        lcd.print("Temps de Jeu :  ");
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


  // Etape 3 : verification, Serial uniquement
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
    delay(2000);
  }

  // Etape 4 : preparation avant lancement
  if (etape==3){
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Lancement dans");
      for(int i=5; i>0; i--){
          lcd.setCursor(7,1);
          lcd.print("0");
          lcd.setCursor(8,1);
          lcd.print(i);
          delay(1000);
      }
     etape = 4;
  }
  
  //Partie qui fait fonctionner
  //le "click" de l'encodeur rotatif
  clkLast = clkState;
  swState = digitalRead(swPin);
  if (swState == LOW && swLast == HIGH && blocage_bouton==HIGH) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Validation etape");
      lcd.setCursor(7,1);
      lcd.print("0");
      lcd.print(etape);
      Serial.print("Validation de l'étape :");
      Serial.println(etape);
      etape += 1;
      delay(100);//debounce
  }
  swLast = swState;
}



void codeScore(){
  // Score
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
}



void codeTimer(){
  //Envoie du temps choisit par le joueur
  //à la fonction qui calcule
  counterToTime(timeInit);

  //Affichage Timer normal
  if(timeInit != 0){
    //Affichage de base
    lcd.setCursor(0,1);
    lcd.print("Temps: ");
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
    currentMillis = millis();
    if (currentMillis - previousMillis > intervalleTemps){
      previousMillis = currentMillis;
      timeInit = timeInit-1;
      initialeTemps += intervalleTemps;
    }
   }

   //Timer arrivé à la fin
   if (timeInit==0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fin de la partie");
    lcd.setCursor(0, 1);
    lcd.print("A bientot, Merci");
    etape = 6;
   }
}



//Conversion en sec du Timer
int timeToCounter(int m,int s){
  return m*60+s;
}



//Fonction de calcule du temps choisit
void counterToTime(long cnt){
  tMin = (cnt%3600)/60;
  tSec = ((cnt%3600)%60);
}
