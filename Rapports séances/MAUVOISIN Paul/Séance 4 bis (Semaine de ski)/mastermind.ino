#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

//LED qui s’allume quand on appuie sur un bouton
const int led[]={5, 6, 7, 12}; //on définit une constante de type entier
const int bouton[]={8, 9, 10, 13}; //on définit une autre constante de type entier

int val[]={1, 1, 1, 1}; // déclaration d’une variable globale qui mémorise l’état du bouton
int choix_bouton[]={1, 1, 1, 1};
int tentative_bouton[]={1, 1, 1, 1};
int ancien_val[]={1, 1, 1, 1};
int verification_code = 0;
int similitude = 0;
int etape = 1;
int vie = 4;
bool blocage_bouton = HIGH;


//Parameters
const int clkPin  = 2;
const int dtPin  = 3;
const int swPin  = 11;

//Variables
int rotVal  = 0;
bool clkState  = LOW;
bool clkLast  = HIGH;
bool swState  = HIGH;
bool swLast  = HIGH;


void setup(){
  Serial.begin(9600);
  Serial.println("Initialisé");
  for (int i=0; i<4; i++) {
    pinMode(led[i], OUTPUT); // l'I/O 2 est utilisée comme une sortie
    pinMode(bouton[i], INPUT_PULLUP); // l'I/O 7 est utilisée comme une sortie
  }

  //Init Rotary
  pinMode(clkPin,INPUT);
  pinMode(dtPin,INPUT);
  pinMode(swPin,INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("Choissiez un");
  lcd.setCursor(0,1);
  lcd.print("code a 4 boutons");
}


void loop() {
  mastermind();
  clickEncodeur();
}

void mastermind(){
  // Etape 1 : Choix du code
  if (etape == 1){
    verification_code = 0;
    for (int i=0; i<4; i++) {
      val[i]=digitalRead(bouton[i]); // lecture de l’état de l’entrée 2
  
      if ((val[i]==LOW)&&(ancien_val[i]==HIGH)) {
        choix_bouton[i]=1-choix_bouton[i];
      }
      ancien_val[i]=val[i];
  
      if (choix_bouton[i]==LOW) { // allume la LED
        digitalWrite(led[i], LOW);
      }
      if (choix_bouton[i]==HIGH) { // éteint la LED
        digitalWrite(led[i], HIGH);
      }

      if (digitalRead(led[i])==LOW){
        verification_code += 1;
      }
    }
  
    if (swState == LOW && verification_code == 2){
      etape = 2;
      for (int i=0; i<4; i++) {
        digitalWrite(led[i], HIGH);
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
  if (etape == 2){
    verification_code = 0;
    for (int i=0; i<4; i++) {
      val[i]=digitalRead(bouton[i]); // lecture de l’état de l’entrée 2
  
      if ((val[i]==LOW)&&(ancien_val[i]==HIGH)) {
        tentative_bouton[i]=1-tentative_bouton[i];
      }
      ancien_val[i]=val[i];
  
      if (tentative_bouton[i]==LOW) { // allume la LED
        digitalWrite(led[i], LOW);
      }
      if (tentative_bouton[i]==HIGH) { // éteint la LED
        digitalWrite(led[i], HIGH);
      }

      if (digitalRead(led[i])==LOW){
        verification_code += 1;
      }
    }

    similitude = 0;
    if (swState == LOW && verification_code == 2 && blocage_bouton == HIGH){
      blocage_bouton = LOW;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Similitude: ");
      lcd.setCursor(0,1);
      lcd.print("Vie restante:");
      Serial.println("Check en cours");
      for (int i=0; i<4; i++) {
        Serial.print(choix_bouton[i]);
        Serial.print(" = ");
        Serial.println(tentative_bouton[i]);
        if (choix_bouton[i] == 0 && tentative_bouton[i] == 0){
          similitude += 1;
        }
        if (similitude == 2){
          Serial.println("vrai");
          etape = 3;
          lcd.clear();
        }
        if (similitude != 2){
          Serial.print("faux, il y a ");
          Serial.print(similitude);
          Serial.println(" similitude");
        }
        if (vie == 0){
          etape = 4;
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
      blocage_bouton = HIGH;
    }
  }

  // Etape 3 : fin gagné
  if (etape == 3) {
    Serial.println("Bravo");
    lcd.setCursor(2,0);
    lcd.print("Victoire du  ");
    lcd.setCursor(3,1);
    lcd.print("Decrypteur  ");
  }

  // Etape 4 : fin perdu
  if (etape == 4) {
    Serial.println("Perdu");
    lcd.setCursor(2,0);
    lcd.print("Victoire du  ");
    lcd.setCursor(4,1);
    lcd.print("Crypteur    ");
  }
}

void clickEncodeur(){
  //gestion bouton
  swState = digitalRead(swPin);
  if (swState == LOW && swLast == HIGH) {
    Serial.println("Rotary pressed");
    delay(100);//debounce
  }
  swLast = swState;
}
