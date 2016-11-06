//#include <TimerOne.h>
#include <MsTimer2.h>
#include <ClickEncoder.h>

#include "WortUhr.h"
#include "MatrixLicht.h"
#include "Wuerfeln.h"

ClickEncoder *encoder;

// pins are fixecd in the libraries classes for now
WortUhr uhr;
RGB_LED rotary_led;
MatrixLicht matrix (rotary_led);
Wuerfeln wuerfeln;

//0: nicht einstellen, 1: auswaehlen der einstellung, 2: einstellen
int einstell_modus = 0;

// 0: nothing, 1: set Matrix light 2: wuerfel, 3: set Wort Uhr, 4: set alarm
int einstell_funktion = 0;

// counts rotary steps during function selection
int rotary_steps = 0;

// how much turn right for a function
int steps_per_function = 30;

// wenn alarm angezeigt wird
bool show_alarm = false;

// wenn alarm ausgeschaltet wird
bool alarm_switched_off = false;

void setup() {

  // Setup Serial port to print debug output.
  Serial.begin(9600);
  Serial.println("Clock starting!");

  uhr.setup();
  matrix.setup();
  wuerfeln.setup();
  setupRotaryEncoder();
}

// Loop function runs over and over again to implement the clock logic.
void loop() {

  uhr.loop();
  matrix.loop();

  // Alarm aktiv
  if (uhr.isAlarmTimeNow()  && !alarm_switched_off) {
    // am Anfang: alles schwarz
    if (!show_alarm) {
      matrix.black();
      delay(30);
    }
    matrix.alarmAnimation();
    show_alarm = true;
  }
  else {
    // alarm geht von alleine nach 1 minute aus
    if (show_alarm) {
      matrix.black();
      matrix.sendSelectedColorToMatrix();
      uhr.resetAlarmToOriginal();
    }
    show_alarm = false;
  }

  // reset variable
  if (!uhr.isAlarmTimeNow() ) {
    alarm_switched_off = false;
  }

  int s = - encoder->getValue();
  ClickEncoder::Button b = encoder->getButton();

  // bei Alarm lange druecken... Alarm ausschalten
  if (b == ClickEncoder::Held && show_alarm) {
    matrix.black();
    matrix.sendSelectedColorToMatrix();
    show_alarm = false;
    alarm_switched_off = true;
    uhr.resetAlarmToOriginal();
  }

  // kurz bei Alarm drücken... snooze
  else if (b == ClickEncoder::Clicked && show_alarm) {
    matrix.black();
    matrix.sendSelectedColorToMatrix();
    show_alarm = false;
    alarm_switched_off = true;
    uhr.snooze();
  }

  else if (einstell_modus == 0) {
    if (b == ClickEncoder::Clicked) {
      einstell_modus = 1;
      printToSerialModus();
    }
  }

  // Auswahl der Funktion
  else if (einstell_modus == 1) {
    funktionsAuswahl(s);
    if (b == ClickEncoder::Clicked) {
      einstell_modus = 2;
      printToSerialModus();
      Serial.print("einstell_funktion: ");
      Serial.println(einstell_funktion);

      // setting modus starts
      if (einstell_funktion == 1) {
        encoder->setAccelerationEnabled(true);
        matrix.startSetting();
      }
      else if (einstell_funktion == 2) {
        wuerfeln.start();
      }
      else if (einstell_funktion == 3) {
        uhr.startAlarmSetting();
      }
      else if (einstell_funktion == 4) {
        uhr.startSetting();
      }
    }
  }

  // Aktive Phase je nach Funktion
  else if (einstell_modus == 2) {

    if (einstell_funktion == 1) {
      // Setzte MatrixLicht with rotary encoder
      matrix.setWithRotaryEncoder(encoder, s, b);

      if (matrix.isSettingFinished()) {
        reset();
      }
    }

    else if (einstell_funktion == 2) {
      //Wuerfeln mit rotary encoder
      wuerfeln.wuerfeln(b);

      if (wuerfeln.isWuerfelnFinished()) {
        Serial.println("Ende wuerfeln");
        reset();
      }
    }

    else if (einstell_funktion == 3) {
      // Setze Alarm mit rotary encoder
      uhr.setAlarmWithRotaryEncoder(encoder, s, b);

      if (uhr.isAlarmSettingFinished()) {
        reset();
      }
    }

    else if (einstell_funktion == 4) {
      // Setze WortUhr mit rotary encoder
      uhr.setWithRotaryEncoder(encoder, s, b);

      if (uhr.isSettingFinished()) {
        reset();
      }
    }


    // no color selected: return
    else {
      reset();
    }
  }
}

// reset led and modus
void reset() {
  einstell_modus = 0;
  rotary_steps = 0;
  printToSerialModus();
  rotary_led.black();
  encoder->setAccelerationEnabled(false);
}

void funktionsAuswahl(int s) {

  rotary_steps += s;

  if (rotary_steps < steps_per_function) {
    einstell_funktion = 0;
    rotary_led.white();

    if (rotary_steps < 0) {
      rotary_steps = 0;
    }
  }
  // Matrix
  else if (rotary_steps < 2 * steps_per_function) {
    einstell_funktion = 1;
    rotary_led.yellow();
  }
  // Wuerfeln
  else if (rotary_steps < 3 * steps_per_function) {
    einstell_funktion = 2;
    rotary_led.blue();
  }
  // Alarm einstellen
  else if (rotary_steps < 4 * steps_per_function) {
    einstell_funktion = 3;
    rotary_led.red();
  }
  // Uhr einstellen
  else {
    einstell_funktion = 4;
    rotary_led.lila();

    if (rotary_steps > 6 * steps_per_function) {
      rotary_steps = 6 * steps_per_function;
    }
  }
}

void setupRotaryEncoder() {
  encoder = new ClickEncoder(A4, A5, A6, 8, LOW); //Rotary PIN-A, PIN-B, PIN-3, Anzahl der Steps, Knopf an 5V oder Ground
  //  Timer1.initialize(1000);
  //  Timer1.attachInterrupt(timerEachMillisecond);
  MsTimer2::set(1, timerEachMillisecond); // 1ms period
  MsTimer2::start();
  encoder->setAccelerationEnabled(false);
}

void timerEachMillisecond() {
  encoder->service();

  // count ticks in uhr
  uhr.tick1ms();
}

void printToSerialModus() {
  Serial.print("einstell_modus: ");
  Serial.println(einstell_modus);
}


