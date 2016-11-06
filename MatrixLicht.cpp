#include "MatrixLicht.h"

// pins for Matrix
#define CLK 11
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

// Matrix panel
RGBmatrixPanel rgb_matrix(A, B, C, D, CLK, LAT, OE, false);


MatrixLicht::MatrixLicht(RGB_LED rotary_encoder_led) {

  rainbow_einstell_modus = 0;         // 0:Grundeinstellungen 1:Farbe 2:Helligkeit 3:Farbe an Matrix
  rainbow_rgb_phase = 1;              // rainbow RGB phasen 1:RG 2:GB 3:BR 4:White

  rotary_led = rotary_encoder_led;

  //value RGB Aus:0 An:255
  valueRed = 0;
  valueGreen = 0;
  valueBlue = 0;

  hell = 50; //Helligkeit 100:hell 0:aus

  rotary_led.doFineSerialOutput(false);

  animation_count = 0;
}

void MatrixLicht::setup() {

  rgb_matrix.begin();
  rgb_matrix.fillScreen(rgb_matrix.Color888(0, 0, 0));
  rotary_led.rgb_ausgabe(0, 0, 0);
}

void MatrixLicht::loop() {
  // nothing to be done on a regular basis
}

void MatrixLicht::setWithRotaryEncoder(ClickEncoder *encoder, int s, ClickEncoder::Button b) {

  // long click: operation abbrechen
  if (rainbow_einstell_modus >0 && b == ClickEncoder::Held) {
      rainbow_einstell_modus = 0;
      Serial.println("MatrixLicht setting cancelled.");
      printToSerialModus();
    }

  else if (rainbow_einstell_modus == 0) {
    Grundeinstellung();
    if (b == ClickEncoder::Clicked) {
      rainbow_einstell_modus = 1;
      printToSerialModus();
    }
  }

  else if (rainbow_einstell_modus == 1) {
    rainbow(s);
    if (b == ClickEncoder::Clicked) {
      rainbow_einstell_modus = 2;
      printToSerialModus();
    }
  }

  else if (rainbow_einstell_modus == 2) {
    brightness(s);
    if (b == ClickEncoder::Clicked) {
      rainbow_einstell_modus = 3;
      printToSerialModus();
    }
  }

  else if (rainbow_einstell_modus == 3) {
    sendSelectedColorToMatrix();
    rainbow_einstell_modus = 0;
    printToSerialModus();
  }

} /* loop*/


void MatrixLicht::startSetting() {
  rainbow_einstell_modus = 1;
  printToSerialModus();
}

bool MatrixLicht::isSettingFinished() {
  return rainbow_einstell_modus == 0;
}

void MatrixLicht::Grundeinstellung() {
  rotary_led.black();
}

void MatrixLicht::sendSelectedColorToMatrix() {

  double faktor = hell / 100.0;

  int red2 = faktor * valueRed;
  int green2 = faktor * valueGreen;
  int blue2 = faktor * valueBlue;

  Serial.print("Farbe an Matrix :^) ");
  Serial.print(red2);
  Serial.print(" ");
  Serial.print(green2);
  Serial.print(" ");
  Serial.println(blue2);

  black();
  //rgb_matrix.fillScreen(rgb_matrix.Color888(0, 0, 0));
  delay(500);

  for (int i = 1; i < 25; i++) {
    rgb_matrix.fillCircle(15, 15, i, rgb_matrix.Color888(red2, green2, blue2, true));
    delay(20);
  }
  rotary_led.black();
}

/**
   Einstellen der Helligkeit
*/

void MatrixLicht::brightness(int s) {

  hell = hell + s;
  if (hell < 0) {
    hell = 0;
  }
  if (hell > 100) {
    hell = 100;
  }

  rotary_led.rgb_ausgabe(valueRed, valueGreen, valueBlue, hell);
}

/**
    Rainbow macht ein Farbübergang abhängig vom Parameter "schritt"
*/
void MatrixLicht::rainbow(int schritt ) {

  // wenn Matrix aus war -> rot
  if (valueRed <10 && valueGreen <10 && valueBlue <10){
    valueRed = 255;
  }

  if ( rainbow_rgb_phase == 1) {
    valueRed = valueRed - schritt;
    valueGreen = 255 - valueRed;

    if (valueGreen > 255) {
      rainbow_rgb_phase = 2;
      valueRed = 0;
      valueGreen = 255;
    }

    else if (valueRed > 255) {
      rainbow_rgb_phase = 1; //unnötig
      valueRed = 255;
      valueGreen = 0;
    }
    else {
      rotary_led.rgb_ausgabe(valueRed, valueGreen, valueBlue);
    }
  }

  if ( rainbow_rgb_phase == 2) {
    valueGreen = valueGreen - schritt;
    valueBlue = 255 - valueGreen;

    if (valueBlue > 255) {
      rainbow_rgb_phase = 3;
      valueGreen = 0;
      valueBlue = 255;
    }
    else if (valueGreen > 255) {
      rainbow_rgb_phase = 1;
      valueBlue = 0;
      valueGreen = 255;
    }
    else {
      rotary_led.rgb_ausgabe(valueRed, valueGreen, valueBlue);
    }
  }

  if ( rainbow_rgb_phase == 3) {
    valueBlue = valueBlue - schritt;
    valueRed = 255 - valueBlue;

    if (valueRed > 255) {
      rainbow_rgb_phase = 4;
      valueBlue = 0;
      valueRed = 255;
    }
    else if  (valueBlue > 255) {
      rainbow_rgb_phase = 2;
      valueRed = 0;
      valueBlue = 255;
    }
    else   {
      rotary_led.rgb_ausgabe(valueRed, valueGreen, valueBlue);
    }
  }

  if (rainbow_rgb_phase == 4) {
    valueRed = 255;
    valueGreen = 255;
    valueBlue = 255;

    //nur eine schnelle Bewegung
    if (schritt < -2) {
      rainbow_rgb_phase = 3;
      valueRed = 250;
      valueGreen = 0;
      valueBlue = 5;
    }
    else   {
      rotary_led.rgb_ausgabe(valueRed, valueGreen, valueBlue);
    }
  }
}

void MatrixLicht::printToSerialModus() {
  Serial.print("rainbow_einstell_modus: ");
  Serial.println(rainbow_einstell_modus);
}

void MatrixLicht::black() {
  //rgb_matrix.fillScreen(rgb_matrix.Color888(0, 0, 0));

  for (int i = 1; i < 25; i++) {
    rgb_matrix.fillCircle(15, 15, i, rgb_matrix.Color888(0, 0, 0, true));
    delay(5);
  }

}

void MatrixLicht::alarmAnimation() {

  animation_count ++;
  if (animation_count > 25) {
    animation_count = 0;
  }
  rgb_matrix.drawCircle(15, 15, (animation_count )  % 25, rgb_matrix.Color888(0, 0, 0, true));
  rgb_matrix.drawCircle(15, 15, (animation_count + 1) % 25, rgb_matrix.Color888(255, 0, 0, true));
  rgb_matrix.drawCircle(15, 15, (animation_count + 2) % 25, rgb_matrix.Color888(0, 255, 0, true));
  rgb_matrix.drawCircle(15, 15, (animation_count + 3) % 25, rgb_matrix.Color888(0, 0, 255, true));
  rgb_matrix.drawCircle(15, 15, (animation_count + 4) % 25, rgb_matrix.Color888(127, 127, 0, true));
  rgb_matrix.drawCircle(15, 15, (animation_count + 5) % 25, rgb_matrix.Color888(0, 127, 127, true));
  rgb_matrix.drawCircle(15, 15, (animation_count + 6) % 25, rgb_matrix.Color888(127, 0, 127, true));
  rgb_matrix.drawCircle(15, 15, (animation_count + 7) % 25, rgb_matrix.Color888(85, 85, 85, true));
  rgb_matrix.drawCircle(15, 15, (animation_count + 8) % 25, rgb_matrix.Color888(0, 0, 0, true));
}



