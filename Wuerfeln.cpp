#include "Wuerfeln.h"

Wuerfeln::Wuerfeln() {

  // pins fuer punkte
  min_pin = 14;
  max_pin = 17;

  punkt_mitte = 14;
  punkte_diagonal1 = 17;
  punkte_diagonal2 = 16;
  punkte_mitte_aussen = 15;

  // leerer analog pin fuer zufallszahl
  analog_input_leer = A10;

  // 0: kein wuerfeln, 1: wuerfeln
  wuerfeln_phase = 0;

}

void Wuerfeln::setup() {
  unsigned long startTime = millis();
  Serial.begin(9600);

  pinMode(punkt_mitte, OUTPUT);
  pinMode(punkte_diagonal1, OUTPUT);
  pinMode(punkte_diagonal2, OUTPUT);
  pinMode(punkte_mitte_aussen, OUTPUT);

  //Let's make it more random
  int seed = analogRead(analog_input_leer) + millis() - startTime;
  randomSeed(seed);
}

void Wuerfeln::wuerfeln(ClickEncoder::Button b) {
  if (wuerfeln_phase == 1 && b == ClickEncoder::Clicked) {

    alle_aus();
    flimmern();
    alle_aus();
    wuerfeln();
  }
  // nach langem druecken: ende
  else if (wuerfeln_phase == 1 && b == ClickEncoder::Held) {
    Serial.println("Wuerfeln ende");
    alle_aus();
    wuerfeln_phase = 0;
  }
}

void Wuerfeln::start() {
  Serial.println("Start wuerfeln");
  wuerfeln_phase = 1;
}

bool Wuerfeln::isWuerfelnFinished() {
  return wuerfeln_phase == 0;
}

//zufallszahl zwischen von und bis
int Wuerfeln::zufallszahl(int von, int bis) {
  if (von >= bis) {
    return von;
  }
  long diff = bis - von + 1;
  return random(diff) + von;
}


// schalte alle leds aus
void Wuerfeln::alle_aus() {
  for (int pin = min_pin; pin <= max_pin; pin++) {
    digitalWrite(pin, LOW);
  }
}

void Wuerfeln::wuerfeln() {

  int zahl = zufallszahl(1, 6);
  // gib zufallszahl im seriell monitor aus
  Serial.print("Wuerfelzahl:");
  Serial.println(zahl);

  // leds fuer alle wuerfelergebnisse
  if (zahl == 1) {
    digitalWrite(punkt_mitte, HIGH);
  } else if (zahl == 2) {
    digitalWrite(punkte_diagonal1, HIGH);
  } else if (zahl == 3) {
    digitalWrite(punkt_mitte, HIGH);
    digitalWrite(punkte_diagonal1, HIGH);
  } else if (zahl == 4) {
    digitalWrite(punkte_diagonal1, HIGH);
    digitalWrite(punkte_diagonal2, HIGH);
  } else if (zahl == 5) {
    digitalWrite(punkte_diagonal1, HIGH);
    digitalWrite(punkte_diagonal2, HIGH);
    digitalWrite(punkt_mitte, HIGH);
  } else if (zahl == 6) {
    digitalWrite(punkte_diagonal1, HIGH);
    digitalWrite(punkte_diagonal2, HIGH);
    digitalWrite(punkte_mitte_aussen, HIGH);
  }
}

void Wuerfeln::flimmern() {

  int anzahl =  zufallszahl(80, 120);

  Serial.print("Flimmern. an:");
  for (int i = 0; i < anzahl; i++) {
    // zufällige pins an
    int zahl = zufallszahl(min_pin, max_pin);
    //Serial.print(zahl);
    //Serial.print(" ");
    digitalWrite(zahl, HIGH);

    // zufällige pins aus
    zahl = zufallszahl(min_pin, max_pin);
    //Serial.print(zahl);
    //Serial.print(" ");
    digitalWrite(zahl, LOW);
    delay(10);
  }
  delay(30);
  Serial.print("\n");
}


