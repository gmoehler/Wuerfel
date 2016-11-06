#include "RGB_LED.h"

RGB_LED::RGB_LED() {

  //Rotary LED pins
  ledRedPin = 8;
  ledGreenPin = 6;
  ledBluePin = 7;

  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledBluePin, OUTPUT);

}

void RGB_LED::white(int brightness) {
  rgb_ausgabe(255, 255, 255, brightness);
}
void RGB_LED::black(int brightness) {
  rgb_ausgabe(0, 0, 0, brightness);
}
void RGB_LED::red(int brightness) {
  rgb_ausgabe(255, 0, 0, brightness);
}
void RGB_LED::blue(int brightness) {
  rgb_ausgabe(0, 0, 255, brightness);
}
void RGB_LED::green(int brightness) {
  rgb_ausgabe(0, 255, 0, brightness);
}
void RGB_LED::yellow(int brightness) {
  rgb_ausgabe(255, 255, 0, brightness);
}
void RGB_LED::lila(int brightness) {
  rgb_ausgabe(255, 0, 255, brightness);
}
void RGB_LED::cyan(int brightness) {
  rgb_ausgabe( 0, 255, 255, brightness);
}

void RGB_LED::rgb_ausgabe(int red, int green, int blue, int brightness) {

  double faktor = brightness / 100.0;

  int red2 = 255 - (faktor * red);
  int green2 = 255 - (faktor * green);
  int blue2 = 255 - (faktor * blue);

  analogWrite(ledRedPin, red2);
  analogWrite(ledGreenPin, green2);
  analogWrite(ledBluePin, blue2);

  printToSerialRgb(red, green, blue, brightness);

}

void RGB_LED::doFineSerialOutput(bool doOutput) {
  printFineSerialOut = doOutput;
}

void RGB_LED::printToSerialRgb(int red, int green, int blue, int bright) {
  if (printFineSerialOut) {
    Serial.print(" ");
    Serial.print(bright);
    Serial.print("%: ");
    Serial.print(red);
    Serial.print(" ");
    Serial.print(green);
    Serial.print(" ");
    Serial.println(blue);
  }
}

