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

// Variables pour illuminer les LED (effet visuel)
bool animation_on = true;
bool animation_already_on = false;
int animation_state = 1;
unsigned long animation_temps = 300;
unsigned long animation_temps_ini = 0;


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
}

void pinMode_led() {
  
  // Initialiser les pins LED
  for (int i = 0; i < NOMBRE_BOUTON; i++) { 
    pinMode(led_pins[i], OUTPUT);
  }
}

void LED_off() {
  for (int i = 0; i < 10; i++) { 
    digitalWrite(led_pins[i], 1);
  }
}

void setup() {

  Serial.begin(9600);

  pinMode_bouton();
  pinMode_led();
  LED_off();

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

void animation_passer_temps() {

    if (millis() - animation_temps_ini > animation_temps) {
            animation_state += 1;
            LED_off();
            animation_already_on = false;
            animation_temps_ini = millis();
            if (animation_state > 3) {
                animation_state = 1;
            }
    }
}

void animation() {

    if ((animation_state == 1) && (animation_already_on == false)) {

        for (int i = 8; i < 10; i++) { 
            digitalWrite(led_pins[i], 0);
        }       
        animation_already_on = true; 
        animation_passer_temps();
    }

    if ((animation_state == 2) && (animation_already_on == false)) {

        for (int i = 5; i < 8; i++) { 
            digitalWrite(led_pins[i], 0);
        }       
        animation_already_on = true; 
        animation_passer_temps();

    }

    if ((animation_state == 3) && (animation_already_on == false)) {

        for (int i = 0; i < 5; i++) { 
            digitalWrite(led_pins[i], 0);
        }       
        animation_already_on = true; 
        animation_passer_temps();

    }


}


void end_game() {

  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Score final:");
  lcd.setCursor(0,1);
  lcd.print(score);
  delay(4000);
  reset_game();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fin de la partie");
  lcd.setCursor(0, 1);
  lcd.print("A bientot, Merci");
  delay(3000);
  setup_afficheur();

}

void end_game_mastermind() {
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
  for (int i = 0; i < NOMBRE_BOUTON; i++) {
    val[i] = 1;
    choix_bouton[i] = 1;
    tentative_bouton[i] = 1;
    ancien_val[i] = 1;
  }
  
  // Reset score
  score = 0;

  // Revenir au menu
  etape = 0;

  // Restart animation
  animation_on = true;
  animation_already_on = false;
}

void jeu_un_next() {
  led_eteinte = 1;
  digitalWrite(bouton_aleatoire - 1, led_eteinte);
  bouton_aleatoire = bouton_pins[random(0,NOMBRE_BOUTON)];
  delay(200);
}

void jeu_un() {
  
  if (etat == "first") {
    randomSeed(millis());
    etat = "ok";
  }

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

    // Animation des leds
    if (animation_on) {
        animation();
    }

  //Etape 4 : le jeu est lancé
  if (etape==4){

    //Jeu 1 (test réflexes)
    if ((choix_mode_jeu == 0) || (choix_mode_jeu == 1) || (choix_mode_jeu == 2)){

      // Réduire le temps durant lequel le bouton est allumé afin d'augmenter la difficulté.
      if (choix_mode_jeu==2) {intervalle = 1000;}

      afficher_score();
      jeu_un();
      codeTimer();
    }

    //Simon
    if (choix_mode_jeu==3){ 
      afficher_score();
      simon();
    }
    
    //Mastermind
    if (choix_mode_jeu==4){ 
      mastermind();
    }

    //Duel
    if (choix_mode_jeu==5){ 
    codeScore();
    }
  }
}

void effet_bouton() {
  for (int i = 0; i < NOMBRE_BOUTON; i++) {
    digitalWrite(bouton_pins[i], 1);
    delay(100);
    digitalWrite(bouton_pins[i], 0);
  }
}

void readRotary( ) {
  //Etape 1 : mode de jeu
  if (etape==0){
    // Gestion position
    clkState = digitalRead(clkPin);
    if ((clkLast == LOW) && (clkState == HIGH)) { //rotary moving
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
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Mode de Jeu :");
            lcd.setCursor(0,1);
            lcd.print("Reflexe Easy");
        }
        if (rotVal==1) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Mode de Jeu :");
            lcd.setCursor(0,1);
            lcd.print("Reflexe Normal");
        }
        if (rotVal==2) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Mode de Jeu :");
            lcd.setCursor(0,1);
            lcd.print("Reflexe Hard");
        }
        if (rotVal==3) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Mode de Jeu :");
            lcd.setCursor(0,1);
            lcd.print("Simon");
        }
        if (rotVal==4) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Mode de Jeu :");
            lcd.setCursor(0,1);
            lcd.print("Mastermind");
        }
        if (rotVal==5) {
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
            lcd.setCursor(4,1);
            lcd.print("00:30");
            tSec = 30;
            tMin = 0;
        }
        if (rotVal==1) {
            lcd.setCursor(4,1);
            lcd.print("00:45");
            tSec = 45;
            tMin = 0;
        }
        if (rotVal==2) {
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
    etape = 3;
    blocage_bouton = LOW;
    //animation_on = false;
    //LED_off();
  }


  // Etape 4 : preparation avant lancement
  if (etape==3){
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Lancement dans");
      for(int i=3; i>0; i--){
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
      etape += 1;
      LED_off();
      delay(100);//debounce
  }
  swLast = swState;
}

void codeScore(){
  // Score
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
}

void mastermind_welcome() {
  blocage_bouton==LOW;
  lcd.setCursor(2,0);
  lcd.print("Choissiez un");
  lcd.setCursor(0,1);
  lcd.print("code a 4 boutons");
  isolement_fonction = 1;
}

void mastermin_phase_une() {
  verification_code = 0;
  for (int i=0; i<10; i++) {
    val[i]=digitalRead(bouton_pins[i]);

    if ((val[i]==LOW)&&(ancien_val[i]==HIGH)) {
      choix_bouton[i]=1-choix_bouton[i];
      delay(200);
    }
    ancien_val[i]=val[i];

    // allume la LED
    if (choix_bouton[i]==LOW) { 
      digitalWrite(led_pins[i], LOW);
    }

    // éteint la LED
    if (choix_bouton[i]==HIGH) { 
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

void mastermin_phase_deux() {
  verification_code = 0;
  for (int i=0; i<10; i++) {
    val[i]=digitalRead(bouton_pins[i]); // lecture de l’état de l’entrée 2

    if ((val[i]==LOW)&&(ancien_val[i]==HIGH)) {
      tentative_bouton[i]=1-tentative_bouton[i];
      delay(200);
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
    for (int i=0; i<10; i++) {
      if (choix_bouton[i] == 0 && tentative_bouton[i] == 0){
        similitude += 1;
      }
      if (similitude == 4){
        phase = 3;
        lcd.clear();
      }
      if (vie == 1){
        phase = 4;
        lcd.clear();
      }
    }
    vie -= 1;
    lcd.setCursor(14,0);
    lcd.print(similitude);
    lcd.setCursor(14,1);
    lcd.print(vie);
  }

  if (swState == HIGH){
    blocage_bouton2 = HIGH;
  }
}

void mastermind_phase_trois() {
  lcd.setCursor(2,0);
  lcd.print("Victoire du  ");
  lcd.setCursor(3,1);
  lcd.print("Decrypteur  ");;
}

void mastermin_phase_quatre() {
  lcd.setCursor(2,0);
  lcd.print("Victoire du  ");
  lcd.setCursor(4,1);
  lcd.print("Crypteur    ");
}

void mastermind() {

  if (isolement_fonction==0){
    mastermind_welcome();
  }

  else {

    // Etape 1 : Choix du code
    if (phase == 1){mastermin_phase_une();
    }

    // Etape 2 : Tentative de trouver le code
    if (phase == 2){
      mastermin_phase_deux();
    }

    // Etape 3 ou 4 : fin
    if ((phase == 3) || (phase == 4)) {
      if (phase == 3) {
        mastermind_phase_trois();
      }
      else {
        mastermin_phase_quatre();
      }
      delay(5000);
      return end_game_mastermind();
    }
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