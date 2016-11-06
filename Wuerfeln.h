#ifndef Wuerfeln_h
#define Wuerfeln_h

#include "Arduino.h"
#include <ClickEncoder.h>

class Wuerfeln {

  public:
    Wuerfeln();
    void setup();
    void wuerfeln(ClickEncoder::Button b);
    bool isWuerfelnFinished();
    void start();

  private:
    void wuerfeln();
    void flimmern();
    int zufallszahl(int von, int bis);
    void alle_aus();
  
    int min_pin;
    int max_pin;

    int punkt_mitte;
    int punkte_diagonal1;
    int punkte_diagonal2;
    int punkte_mitte_aussen;

    // leerer analog pin fuer zufallszahl
    int analog_input_leer;
    int wuerfeln_phase;

    unsigned long pressed_start;

};

#endif
