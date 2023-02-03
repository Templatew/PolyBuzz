#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Bouton
const int bouton_pins[] = {
  53, 51, 49, 47, 45, 43, 41, 39, 37, 35,
};


// LED
const int led_pins[] = {
  52, 50, 48, 46, 44, 42, 40, 38, 36, 34,
};


// Liste bouton taille
const int NOMBRE_BOUTON = sizeof(bouton_pins)/sizeof(bouton_pins[0]); 

// Bouton random
int bouton_aleatoire = 0;
int led_eteinte = 1;

// Temps limite pression bouton (jeu_un)
unsigned long temps_ini = 0;
unsigned long intervalle = 2000;


// Simon variables
int temps_simon = 500;
int step = 0;
int num_courant = 0;
int num_courant_joueur = -1;
const int SIMON_LENGTH_MAX = 10;
String etat = "first";
String liste_simon = "";

//Parametres pour l'encodeur rotatif
const int clkPin  = 10;
const int dtPin   = 9;
const int swPin   = 8;

//Variables pour l'encodeur rotatif
int rotVal  = 0;
bool clkState  = LOW;
bool clkLast  = HIGH;
bool swState  = HIGH;
bool swLast   = HIGH;

//Variables pour la fonction: readRotaty
int choix_mode_jeu = 0;
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

//Variables pour la fonction Mastermind

int val[]= {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; // déclaration d’une variable globale qui mémorise l’état du bouton_pins
int choix_bouton[]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int tentative_bouton[]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int ancien_val[]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int verification_code = 0;
int similitude = 0;
int phase = 1;
int vie = 4;
bool blocage_bouton2 = HIGH;
int isolement_fonction = 0;

// Score
int score = 0;

void pinMode_bouton() {
  
  // Initialiser les pins boutons
  for (int i = 0; i < NOMBRE_BOUTON; i++) { 
    pinMode(bouton_pins[i], INPUT_PULLUP);
  }
  Serial.println("Bouton ok");
}

void pinMode_led() {
  
  // Initialiser les pins LED
  for (int i = 0; i < NOMBRE_BOUTON; i++) { 
    pinMode(led_pins[i], OUTPUT);
  }
  Serial.println("LED ok");
}

void LED_off() {
  for (int i = 0; i <= 10; i++) { 
    digitalWrite(led_pins[i], 1);
  }
}

void setup() {

  Serial.begin(9600);

  pinMode_bouton();
  pinMode_led();
  LED_off();
  randomSeed(millis());

  // Initialisation de l'afficheur
  lcd.init();
  lcd.backlight();
  setup_afficheur();

  //Initialisation encodeur rotatif
  pinMode(clkPin,INPUT);
  pinMode(dtPin,INPUT);
  pinMode(swPin,INPUT_PULLUP);
}


void setup_afficheur() {

  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Bienvenue");
  lcd.setCursor(2,1);
  lcd.print("Tournez-moi!");

}

void end_game() {

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Score final:");
  lcd.setCursor(0,1);
  lcd.print(score);
  delay(2000);
  reset_game();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fin de la partie");
  lcd.setCursor(0, 1);
  lcd.print("A bientot, Merci");
  delay(3000);
  setup_afficheur();

}

void reset_game() {

  //Reset LED
  LED_off();

  // Reset Simon
  temps_simon = 500;
  step = 0;
  num_courant = 0;
  num_courant_joueur = -1;
  etat = "simon";
  liste_simon = "";
  
  // Reset jeu_un
  bouton_aleatoire = 0;
  led_eteinte = 1;
  temps_ini = 0;
  intervalle = 2000;
  
  // Reset Mastermind
  verification_code = 0;
  similitude = 0;
  phase = 1;
  vie = 4;
  blocage_bouton = HIGH;
  isolement_fonction = 0;
  
  // Reset score
  score = 0;

  // Revenir au menu
  etape = 0;

}

void jeu_un_next() {
  led_eteinte = 1;
  digitalWrite(bouton_aleatoire - 1, led_eteinte);
  bouton_aleatoire = bouton_pins[random(0,NOMBRE_BOUTON)];
  delay(200);
}

void jeu_un() {
  
  if ((bouton_aleatoire == 0) && (led_eteinte == 1)) {
    bouton_aleatoire = bouton_pins[random(0,NOMBRE_BOUTON)];
  }
  
  if (led_eteinte == 1) {
    led_eteinte = 0;
    digitalWrite(bouton_aleatoire - 1, led_eteinte);
  }
  
  if (led_eteinte == 0) {

    if (((choix_mode_jeu == 1) || (choix_mode_jeu == 2)) && (millis() - temps_ini > intervalle)) {
        jeu_un_next();
        temps_ini += intervalle;
    }
    
    for (int i = 0; i < NOMBRE_BOUTON; i++) {

      if (digitalRead(bouton_pins[i]) == 0) {
        if (bouton_pins[i] != bouton_aleatoire) {
          if (score>0) {score -= 1;}
        }
        else {score += 1;}
        jeu_un_next();        
      }
    }  
  }
}

void simon() {
    
    if (etat == "first") {
      randomSeed(millis());
      etat = "simon";
    }

    if (etat == "simon") {

      if (step == 0) {
        liste_simon += random(NOMBRE_BOUTON);
      }
      
      for (int i = 0; i < liste_simon.length(); i++) {

        num_courant = liste_simon[i] - '0';
        digitalWrite(led_pins[num_courant], 0);
        delay(temps_simon);
        LED_off();
        delay(temps_simon);
      }

      etat = "joueur";
      step = 0;
    }
    
    
    if (etat == "joueur") {

      for (int i = 0; i < NOMBRE_BOUTON; i++) {
        if (digitalRead(bouton_pins[i]) == 0) {
          num_courant_joueur = i;
          num_courant = liste_simon[step] - '0';
          if (num_courant_joueur == num_courant) {
            step ++;
          }
          else {
            return end_game();
          }
          delay(200);
        }
      }

      if (step >= liste_simon.length()) {
        step = 0;
        etat = "simon";
        score += 1;
      }
    }

    if (liste_simon.length() == SIMON_LENGTH_MAX) {
      return end_game();
    }

}

void afficher_score() {
    lcd.backlight();
    //Envoi du message
    lcd.setCursor(0, 0);
    lcd.print("SCORE: ");
    lcd.setCursor(7, 0);
    lcd.print(score);
}

void loop() {

    //Post gameplay
    readRotary();

    //Etape 4 : le jeu est lancé
    if (etape==4){
        if ((choix_mode_jeu == 0) || (choix_mode_jeu == 1) || (choix_mode_jeu == 2)){

          // Réduire le temps durant lequel le bouton est allumé afin d'augmenter la difficulté.
          if (choix_mode_jeu==2) {intervalle = 1000;}

          afficher_score();
          jeu_un();
          codeTimer();
        }

        if (choix_mode_jeu==3){ //Simon
          afficher_score();
          simon();
        }

        if (choix_mode_jeu==4){ //Mastermind
            if (isolement_fonction==0){
                blocage_bouton==LOW;
                lcd.setCursor(2,0);
                lcd.print("Choissiez un");
                lcd.setCursor(0,1);
                lcd.print("code a 4 boutons");
                for (int i=0; i<4; i++) {
                pinMode(led_pins[i], OUTPUT); // l'I/O 2 est utilisée comme une sortie
                pinMode(bouton_pins[i], INPUT_PULLUP); // l'I/O 7 est utilisée comme une sortie
                }
                isolement_fonction = 1;
            }
            if (isolement_fonction==1){
                mastermind();
            }
            if (isolement_fonction==2){
                blocage_bouton2==HIGH;
                etape = 0;
            }
        }
        if (choix_mode_jeu==5){ //Duel
        codeScore();
        }
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
                rotVal = 5;
            }
        }
        else {
            rotVal++;
            if ( rotVal > 5 ) {
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
        if (rotVal==4) {
            Serial.println("Mastermind");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Mode de Jeu :");
            lcd.setCursor(0,1);
            lcd.print("Mastermind");
        }
        if (rotVal==5) {
            Serial.println("Duel");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Mode de Jeu :");
            lcd.setCursor(0,1);
            lcd.print("Duel");
        }
        choix_mode_jeu = rotVal;
        if ((choix_mode_jeu == 3 || choix_mode_jeu == 4 || choix_mode_jeu == 5) && (clkState == LOW)){
          etape = 2;
        }
        delay(200);
    }
  }


  // Etape 2 : reglage du temps
  if (etape==1 && choix_mode_jeu >= 0 && choix_mode_jeu <= 2){  // Le Simon, Mastermind et Duel n'auront pas besoin de temps
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
            tSec = 30;
            tMin = 0;
        }
        if (rotVal==1) {
            Serial.println("00:45");
            lcd.setCursor(4,1);
            lcd.print("00:45");
            tSec = 45;
            tMin = 0;
        }
        if (rotVal==2) {
            Serial.println("01:00");
            lcd.setCursor(4,1);
            lcd.print("01:00");
            tSec = 0;
            tMin = 1;
        }
        temps = rotVal;
        timeInit=timeToCounter(tMin,tSec);
        delay(200);
    }
  }


  // Etape 3 : verification, Serial uniquement
  if (etape==2){
    Serial.println("Pret à jouer");
    Serial.print("Mode de jeu : ");
    const String texte[6] = {"Reflexe Facile", "Reflexe Moyen", "Reflexe Difficile", "Simon", "Mastermind", "Duel"};
    Serial.print(texte[choix_mode_jeu]);
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
     lcd.clear();
     etape = 4;
  }
 
  clickEncodeur();
}

//Partie qui fait fonctionner le "click" de
//l'encodeur rotatif qui fait passer les étapes
void clickEncodeur(){
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

void mastermind(){

  // Etape 1 : Choix du code
  if (phase == 1){
    verification_code = 0;
    for (int i=0; i<10; i++) {
      val[i]=digitalRead(bouton_pins[i]); // lecture de l’état de l’entrée 2
  
      if ((val[i]==LOW)&&(ancien_val[i]==HIGH)) {
        choix_bouton[i]=1-choix_bouton[i];
        delay(20);
      }
      ancien_val[i]=val[i];
  
      if (choix_bouton[i]==LOW) { // allume la LED
        digitalWrite(led_pins[i], LOW);
      }
      if (choix_bouton[i]==HIGH) { // éteint la LED
        digitalWrite(led_pins[i], HIGH);
      }

      if (digitalRead(led_pins[i])==LOW){
        verification_code += 1;
      }
    }
  
    if (swState == LOW && verification_code == 4){
      phase = 2;
      for (int i=0; i<10; i++) {
        digitalWrite(led_pins[i], HIGH);
        val[i] = 1;
        ancien_val[i] = 1;
      }
      Serial.println("Validé");
      Serial.println("Passage à l'étape 2");
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Code valide");
      delay(3000);
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("Trouvez le");
      lcd.setCursor(0,1);
      lcd.print("code a 4 boutons");
    }
  }

  // Etape 2 : Tentative de trouver le code
  if (phase == 2){
    verification_code = 0;
    for (int i=0; i<10; i++) {
      val[i]=digitalRead(bouton_pins[i]); // lecture de l’état de l’entrée 2
  
      if ((val[i]==LOW)&&(ancien_val[i]==HIGH)) {
        tentative_bouton[i]=1-tentative_bouton[i];
        delay(20);
      }
      ancien_val[i]=val[i];
  
      if (tentative_bouton[i]==LOW) { // allume la LED
        digitalWrite(led_pins[i], LOW);
      }
      if (tentative_bouton[i]==HIGH) { // éteint la LED
        digitalWrite(led_pins[i], HIGH);
      }

      if (digitalRead(led_pins[i])==LOW){
        verification_code += 1;
      }
    }

    similitude = 0;
    if (swState == LOW && verification_code == 4 && blocage_bouton2 == HIGH){
      blocage_bouton2 = LOW;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Similitude: ");
      lcd.setCursor(0,1);
      lcd.print("Vie restante:");
      Serial.println("Check en cours");
      for (int i=0; i<10; i++) {
        Serial.print(choix_bouton[i]);
        Serial.print(" = ");
        Serial.println(tentative_bouton[i]);
        if (choix_bouton[i] == 0 && tentative_bouton[i] == 0){
          similitude += 1;
        }
        if (similitude == 4){
          Serial.println("vrai");
          phase = 3;
          lcd.clear();
        }
        if (similitude != 4){
          Serial.print("faux, il y a ");
          Serial.print(similitude);
          Serial.println(" similitude");
        }
        if (vie == 1){
          phase = 4;
          lcd.clear();
        }
      }
      vie -= 1;
      Serial.print("vie restante");
      Serial.println(vie);
      lcd.setCursor(14,0);
      lcd.print(similitude);
      lcd.setCursor(14,1);
      lcd.print(vie);
    }

    if (swState == HIGH){
      blocage_bouton2 = HIGH;
    }
  }

  // Etape 3 : fin gagné
  if (phase == 3) {
    Serial.println("Bravo");
    lcd.setCursor(2,0);
    lcd.print("Victoire du  ");
    lcd.setCursor(3,1);
    lcd.print("Decrypteur  ");
    delay(5000);
    isolement_fonction = 2;
  }

  // Etape 4 : fin perdu
  if (phase == 4) {
    Serial.println("Perdu");
    lcd.setCursor(2,0);
    lcd.print("Victoire du  ");
    lcd.setCursor(4,1);
    lcd.print("Crypteur    ");
    delay(5000);
    isolement_fonction = 2;
  }
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
    end_game();
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
