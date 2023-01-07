// Bouton
int bouton_pins[] = {
  53, 51, 49, 47, 45, 43, 41, 39, 37, 35
};

// LED
int led_pins[] = {
  52, 50, 48, 46, 44, 42, 40, 38, 36, 34
};

// Liste bouton taille
const int NOMBRE_BOUTON = sizeof(bouton_pins)/sizeof(bouton_pins[0]); 

// Bouton random
int bouton_aleatoire = 0;
int led_eteinte = 1;

// Temps limite pression bouton (jeu_deux)
unsigned long temps_ini = 0;
unsigned long intervalle = 2000;

// Choix du jeu
int choix_jeu = 2;

// Simon liste bouton
const int MAX_SIZE = 15;
int liste_simon[MAX_SIZE];
int counter = 0;
String etat = "ok";

// Score
int score = 0;

void pinMode_bouton() {
  
  // Initialiser les pins boutons
  for (int i = 0; i <= 10; i++) { 
    pinMode(bouton_pins[i], INPUT);
  }
  Serial.println("Bouton ok");
}

void pinMode_led() {
  
  // Initialiser les pins LED
  for (int i = 0; i <= 10; i++) { 
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
}

void jeu_un_next() {
  led_eteinte = 1;
  digitalWrite(bouton_aleatoire - 1, led_eteinte);
  delay(20);
  bouton_aleatoire = bouton_pins[random(0,NOMBRE_BOUTON)];
  temps_ini += intervalle;
}

void jeu_un() {
  
  if ((bouton_aleatoire == 1) && (led_eteinte == 1)) {
    bouton_aleatoire = bouton_pins[random(0,NOMBRE_BOUTON)];
  }
  
  if (led_eteinte == 1) {
    led_eteinte = 0;
    digitalWrite(bouton_aleatoire - 1, led_eteinte);
  }
  
  if (led_eteinte == 0) {

    if ((choix_jeu == 1) and (millis() - temps_ini > intervalle)) {
        jeu_un_next();
    }
    
    for (int i = 0; i < NOMBRE_BOUTON; i++) {

      if (digitalRead(bouton_pins[i]) == 0) {
        if (bouton_pins[i] != bouton_aleatoire) {score -= 1;}
        else {score += 1;}
        jeu_un_next();
      }
    }  
  }
}

void simon() {
    // V1
}


void loop() {
  if ((choix_jeu==0) || (choix_jeu == 1)) {
    jeu_un();  
  }
  else if (choix_jeu == 2) {
    simon();
  }
    
}
