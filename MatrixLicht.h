#ifndef MatrixLicht_h
#define MatrixLicht_h

//#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <ClickEncoder.h>
#include "RGB_LED.h"


class MatrixLicht {
  public:
    MatrixLicht(RGB_LED rotary_led);

    // to be called in setup
    void setup();

    // to be called in each loop
    void loop();

    void startSetting();

    bool isSettingFinished();

    // to be called in loop on color set
    void setWithRotaryEncoder(ClickEncoder *encoder, int step, ClickEncoder::Button b);

    void sendSelectedColorToMatrix();

    // Kreisanimation fuer Alarm
    void alarmAnimation();

    void black();

  private:

    void Grundeinstellung();
    void brightness(int s);
    void rainbow(int schritt );
    void rgb_ausgabe(int red, int green, int blue);
    void printToSerialModus();

    void printToSerialRgb(int red, int green, int blue);
    void printToSerialHelligkeit(int hell, int hell2);

    // Zustaende waehrend des Setzens
    int rainbow_einstell_modus = 0;         // 0:Grundeinstellungen 1:Farbe 2:Helligkeit 3:Farbe an Matrix
    int rainbow_rgb_phase = 1;              // rainbow RGB phasen 1:RG 2:GB 3:BR 4:White

    // rgb of rotary encoder
    RGB_LED rotary_led;

    //Helligkeitswerte der RGB LEDs (Aus:0 An:255)
    int valueRed;
    int valueGreen;
    int valueBlue;

    // Helligkeit insgesamt (100:hell 0:aus)
    int hell;

    int animation_count;
};

#endif //MatrixLicht_h
