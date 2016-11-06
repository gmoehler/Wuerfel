#include "WortUhr.h"

WortUhr::WortUhr() {
  // Create display and DS1307 objects.  These are global variables that
  // can be accessed from both the setup and loop function below.
  clockDisplay = Adafruit_7segment();
  rtc = RTC_DS1307();

  // Keep track of the hours, minutes, seconds displayed by the clock.
  // Start off at 0:00:00 as a signal that the time should be read from
  // the DS3231 to initialize it.
  hours = 0;
  minutes = 0;

  // initial alarm
  alarm_hours = 6;
  alarm_minutes = 33;
  original_alarm_hours = 6;
  original_alarm_minutes = 33;

  // 0: uhr läuft; 1: uhr anzeigen, 2: stunden setzen, 3: minuten setzen
  set_uhr_phase = 0;

  // 0: Normalbetrieb ; 1: alarm anzeigen 2: stunden setzen, 3: minuten setzen, 4: an/aus
  set_alarm_phase = 0;

  alarm_active = false;
  alarm_time_now = false;

  // initialize the outputs
  u_es = 30;
  u_ist = 31;
  u_zehn = 32;
  u_fuenf = 33;
  u_vor = 34;
  u_nach = 35;
  u_drei = 36;
  u_viertel = 37;
  u_halb = 38;
  u_1 = 39;
  u_2 = 40;
  u_3 = 41;
  u_4 = 42;
  u_5 = 43;
  u_6 = 44;
  u_7 = 45;
  u_8 = 46;
  u_9 = 47;
  u_10 = 48;
  u_11 = 49;
  u_12 = 50;
  u_uhr = 51;
  u_wecker = 52;

  steps = 0;
  correct_seconds = 0;

  // interval in dem die Uhr ge-updated wird
  uhr_update_interval = 30;
}

// called during arduino setup
void WortUhr::setup() {
  // Setup function runs once at startup to initialize the display
  // and DS3231 clock.

  // Setup the display.
  clockDisplay.begin(DISPLAY_ADDRESS);

  // Setup the DS3231 real-time clock.
  rtc.begin();

  // intially set time to compile time
  //setInitialTimeOnDs3231();

  // initially get time
  getTimeFromDs3231();

  //setup uhr output leds
  for (int i = u_es; i <= u_wecker; i++) {
    pinMode(i, OUTPUT);
  }

  displayUhrzeit();
  clockDisplay.clear();
}

void WortUhr::tick1ms() {
  // just count the milliseconds
  count_ms++;
}

void WortUhr::loop() {

  // read time from timer each minute
  // and set word uhr
  if (count_ms > (uhr_update_interval - correct_seconds) * 1000) {
    correct_seconds = 0;
    getTimeFromDs3231();
    displayUhrzeit();

    // Alarmzeit, aber nur beim ersten Mal wird auch Alarm ausgelöst
    if (alarm_active && hours == alarm_hours && minutes == alarm_minutes) {
      alarm_time_now = true;
      Serial.println("Aufwachen!");
    }
    else {
      alarm_time_now = false;
    }

    // update time on 7seg if we display the current time
    if (set_uhr_phase == 1) {
      showTimeOn7Seg(hours, minutes);
    }
    count_ms = 0;
  }
}

/**
    Uhr mit rotary encoder stellen.
    Phasen:
    0: Uhr läuft, Display dunkel
    1: Uhr auf Display anzeigen
    2: Stunden mit Rotary encoder setzen
    3: Minuten mit Rotary encoder setzen
    weiterer click: gesetzte Zeit auf DS3221 übertragen.
*/

void WortUhr::setWithRotaryEncoder(ClickEncoder *encoder, int s, ClickEncoder::Button b) {


  // long click: operation abbrechen
  if (set_uhr_phase > 0 && b == ClickEncoder::Held) {
    set_uhr_phase = 0;
    clockDisplay.clear();
    Serial.println("Setting Uhr cancelled.");
    printToSerialPhase();
  }


  // Uhr läuft, Display dunkel
  else if (set_uhr_phase == 0) {
    clockDisplay.clear();

    if (b == ClickEncoder::Clicked) {
      startSetting();
    }
  }

  // Uhr auf Display anzeigen
  else if (set_uhr_phase == 1) {
    showTimeOn7Seg(hours, minutes);
    blinkColon();

    if (b == ClickEncoder::Clicked) {
      set_uhr_phase = 2;
      printToSerialPhase();
    }
  }

  // Stunden mit Rotary encoder setzen
  else if (set_uhr_phase == 2) {
    setHours(s, hours);
    showHoursOn7Seg(hours);

    if (b == ClickEncoder::Clicked) {
      set_uhr_phase = 3;
      printToSerialPhase();
    }
  }

  // Minuten mit Rotary encoder setzen
  else if (set_uhr_phase == 3) {
    setMinutes(s, minutes);
    showMinsOn7Seg(minutes);

    if (b == ClickEncoder::Clicked) {
      set_uhr_phase = 0;
      printToSerialPhase();
      // gesetzte Zeit auf DS3221 übertragen.
      saveTimeOnDs3231();
      displayUhrzeit();
      clockDisplay.clear();
    }
  }

  // Now push out to the display the new values that were set above.
  clockDisplay.writeDisplay();

} // setWithRotaryEncoder

void WortUhr::setAlarmWithRotaryEncoder(ClickEncoder *encoder, int s, ClickEncoder::Button b) {

  // long click: operation abbrechen
  if (set_alarm_phase > 0 && b == ClickEncoder::Held) {
    set_alarm_phase = 0;
    clockDisplay.clear();
    Serial.println("Setting Alarm cancelled.");
    printToSerialPhase();
  }


  // Uhr läuft, Display dunkel
  else if (set_alarm_phase == 0) {
    clockDisplay.clear();

    if (b == ClickEncoder::Clicked) {
      startAlarmSetting();
    }
  }

  // Uhr auf Display anzeigen
  else if (set_alarm_phase == 1) {
    showTimeOn7Seg(alarm_hours, alarm_minutes);

    if (b == ClickEncoder::Clicked) {
      set_alarm_phase = 2;
      printToSerialPhase();
    }
  }

  // Stunden mit Rotary encoder setzen
  else if (set_alarm_phase == 2) {
    setHours(s, alarm_hours);
    original_alarm_hours = alarm_hours;
    showHoursOn7Seg(alarm_hours);

    if (b == ClickEncoder::Clicked) {
      set_alarm_phase = 3;
      printToSerialPhase();
    }
  }

  // Minuten mit Rotary encoder setzen
  else if (set_alarm_phase == 3) {
    setMinutes(s, alarm_minutes);
    original_alarm_minutes = alarm_minutes;
    showMinsOn7Seg(alarm_minutes);

    if (b == ClickEncoder::Clicked) {
      set_alarm_phase = 4;
      Serial.print ("Setting alarm time to: ");
      Serial.print(alarm_hours);
      Serial.print(":");
      Serial.println(alarm_minutes);
      printToSerialPhase();
    }
  }

  // an / aus mit rotary setzen
  else if (set_alarm_phase == 4) {
    setAlarmOnOff(s);
    showOnOffOn7Seg(alarm_active);

    if (b == ClickEncoder::Clicked) {
      set_alarm_phase = 0;
      Serial.print ("Setting alarm ");
      if (alarm_active) {
        Serial.println("on");
        digitalWrite(u_wecker, HIGH);
      }
      else {
        Serial.println("off");
        digitalWrite(u_wecker, LOW);
      }
      printToSerialPhase();
      clockDisplay.clear();
    }
  }

  // Now push out to the display the new values that were set above.
  clockDisplay.writeDisplay();

} // setWithRotaryEncoder

void WortUhr::blinkColon() {

  int secs = count_ms / 1000;
  if (secs / 2 == (secs + 1) / 2) {
    clockDisplay.drawColon(false);
  }
  else {
    clockDisplay.drawColon(true);
  }
}

bool WortUhr::isSettingFinished() {
  return set_uhr_phase == 0;
}

bool WortUhr::isAlarmSettingFinished() {
  return set_alarm_phase == 0;
}
bool WortUhr::isAlarmTimeNow() {
  return alarm_time_now;
}

void WortUhr::startSetting() {
  set_uhr_phase = 1;
  printToSerialPhase();
  getTimeFromDs3231();
}
void WortUhr::startAlarmSetting() {
  set_alarm_phase = 1;
  printToSerialPhase();
}

void WortUhr::setMinutes(int s, int &mins) {

  // eine Minute pro Schnapper (4 Schritte)
  int steps_pro_schnapper = 4;
  steps = steps + s;
  if (abs(steps) >= steps_pro_schnapper) {
    int delta = steps / steps_pro_schnapper;
    mins = mins + delta;
    if (mins >= 60) {
      mins = 0;
    }
    if (mins <= -1) {
      mins = 59;
    }
    steps = steps - delta * steps_pro_schnapper;
  }
}

void WortUhr::setHours(int s, int &hrs) {
  // eine Stunde pro Schnapper (4 Schritte)
  int steps_pro_schnapper = 4;
  steps = steps + s;
  if (abs(steps) >= steps_pro_schnapper) {
    int delta = steps / steps_pro_schnapper;
    hrs = hrs + delta;
    if (hrs >= 13) {
      hrs = 1;
    }
    if (hrs <= 0) {
      hrs = 12;
    }
    steps = steps - delta * steps_pro_schnapper;
  }
}

void WortUhr::setAlarmOnOff(int s) {
  // on/off pro Schnapper (4 Schritte)
  int steps_pro_schnapper = 4;
  steps = steps + s;
  if (abs(steps) >= steps_pro_schnapper) {
    alarm_active = !alarm_active;
    steps = 0;
  }
}


void WortUhr::printToSerialPhase() {
  Serial.print("set_uhr_phase:");
  Serial.print(set_uhr_phase);
  Serial.print(" set_alarm_phase:");
  Serial.println(set_alarm_phase);
}

void WortUhr::showHoursOn7Seg(int hrs) {
  clockDisplay.clear();
  if (hrs >= 10) {
    clockDisplay.writeDigitNum(0, hrs / 10);
  }
  clockDisplay.writeDigitNum(1, hrs % 10);
  clockDisplay.drawColon(true);
}

void WortUhr::showMinsOn7Seg(int mins) {
  clockDisplay.clear();
  clockDisplay.writeDigitNum(3, mins / 10);
  clockDisplay.writeDigitNum(4, mins % 10);
  clockDisplay.drawColon(true);
}

void WortUhr::showTimeOn7Seg(int hrs, int mins) {
  // Show the time on the display by turning it into a numeric
  // value, like 3:30 turns into 330, by multiplying the hour by
  // 100 and then adding the minutes.
  int displayValue = hrs * 100 + mins;

  // Now print the time value to the display.
  clockDisplay.print(displayValue, DEC);
  clockDisplay.drawColon(true);
}

void WortUhr::showOnOffOn7Seg(bool onoff) {

  clockDisplay.clear();
  if (onoff) {
    clockDisplay.writeDigitRaw(3, 119); // A
    clockDisplay.writeDigitRaw(4, 84); // N
  }
  else {
    clockDisplay.writeDigitRaw(1, 119);// A
    clockDisplay.writeDigitRaw(3, 62); // U
    clockDisplay.writeDigitRaw(4, 109); // S
  }

  clockDisplay.drawColon(false);
}

void WortUhr::getTimeFromDs3231() {
  // Get the time from the DS3231.
  DateTime now = rtc.now();
  // Now set the hours and minutes.
  hours = now.hour();
  if (hours > 13) {
    hours = hours - 12;
  }
  if (hours == 0) {
    hours = 0;
  }
  minutes = now.minute();
  int seconds = now.second();

  // Print out the time for debug purposes:
  Serial.print("Read date & time from DS3231: ");
  //  Serial.print(now.year(), DEC);
  //  Serial.print('/');
  //  Serial.print(now.month(), DEC);
  //  Serial.print('/');
  //  Serial.print(now.day(), DEC);
  //  Serial.print(' ');
  Serial.print(hours, DEC);
  Serial.print(':');
  Serial.print(minutes, DEC);
  Serial.print(':');
  Serial.println(seconds, DEC);

  correct_seconds = seconds < 30 ? seconds : seconds - 30;
  Serial.print ("correct seconds:");
  Serial.println (correct_seconds);
}

void WortUhr::saveTimeOnDs3231() {
  rtc.adjust(DateTime(2014, 1, 1, hours, minutes, 0));
  // Print out the time for debug purposes:
  Serial.print("Save time to DS3231: ");
  Serial.print(hours, DEC);
  Serial.print(':');
  Serial.println(minutes, DEC);
  // Now set the hours and minutes.

}

void WortUhr::setInitialTimeOnDs3231() {
  // Set the DS3231 clock if it hasn't been set before.
  //bool setClockTime = !rtc.isrunning();
  // Alternatively you can force the clock to be set again by
  // uncommenting this line:
  //setClockTime = true;
  //  if (setClockTime) {
  Serial.println("Setting DS3231 time!");
  // This line sets the DS3231 time to the exact date and time the
  // sketch was compiled:
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // Alternatively you can set the RTC with an explicit date & time,
  // for example to set January 21, 2014 at 3am you would uncomment:
  //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  // }
}

void WortUhr::displayUhrzeit() {
  int stunde_led = u_1 + hours - 1;
  if (hours > 12) {
    stunde_led = stunde_led - 12;
  }
  Serial.print("displayUhrzeit: ");

  // alle LEDs ausiu
  for (int i = u_es; i <= u_wecker; i++) {
    digitalWrite(i, LOW);
  }

  if ( minutes >= 0 && minutes <= 2) {
    Serial.println("a");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(stunde_led, HIGH);
    digitalWrite(u_uhr, HIGH);
  }

  if (minutes >= 3 && minutes <= 7) {
    Serial.println("b");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(u_fuenf, HIGH);
    digitalWrite(u_nach, HIGH);
    digitalWrite(stunde_led, HIGH);
  }
  if (minutes >= 8  && minutes <= 12) {
    Serial.println("c");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(u_zehn, HIGH);
    digitalWrite(u_nach, HIGH);
    digitalWrite(stunde_led, HIGH);
  }
  if (minutes >= 13 && minutes <= 17) {
    Serial.println("d");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(u_viertel, HIGH);
    digitalWrite(stunde_led + 1, HIGH);
  }

  if (minutes >= 18 && minutes <= 22) {
    Serial.println("e");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(u_zehn, HIGH);
    digitalWrite(u_vor, HIGH);
    digitalWrite(u_halb, HIGH);
    digitalWrite(stunde_led + 1, HIGH);
  }

  if (minutes >= 23 && minutes <= 27) {
    Serial.println("f");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(u_fuenf, HIGH);
    digitalWrite(u_vor, HIGH);
    digitalWrite(u_halb, HIGH);
    digitalWrite(stunde_led + 1, HIGH);
  }

  if (minutes >= 28 && minutes <= 32) {
    Serial.println("g");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(u_halb, HIGH);
    digitalWrite(stunde_led + 1, HIGH);
  }

  if (minutes >= 33 && minutes <= 37) {
    Serial.println("h");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(u_fuenf, HIGH);
    digitalWrite(u_nach, HIGH);
    digitalWrite(u_halb, HIGH);
    digitalWrite(stunde_led + 1, HIGH);
  }

  if (minutes >= 38 && minutes <= 42) {
    Serial.println("i");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(u_zehn, HIGH);
    digitalWrite(u_nach, HIGH);
    digitalWrite(u_halb, HIGH);
    digitalWrite(stunde_led + 1, HIGH);
  }

  if (minutes >= 43 && minutes <= 47) {
    Serial.println("j");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(u_drei, HIGH);
    digitalWrite(u_viertel, HIGH);
    digitalWrite(stunde_led + 1, HIGH);
  }

  if (minutes >= 48 && minutes <= 52) {
    Serial.println("k");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(u_zehn, HIGH);
    digitalWrite(u_vor, HIGH);
    digitalWrite(stunde_led + 1, HIGH);
  }

  if (minutes >= 53 && minutes <= 57) {
    Serial.println("l");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(u_fuenf, HIGH);
    digitalWrite(u_vor, HIGH);
    digitalWrite(stunde_led + 1, HIGH);
  }

  if ( minutes >= 58 && minutes <= 60 ) {
    Serial.println("a");
    digitalWrite(u_es, HIGH);
    digitalWrite(u_ist, HIGH);
    digitalWrite(stunde_led + 1, HIGH);
    digitalWrite(u_uhr, HIGH);
  }

  if ( alarm_active) {
    digitalWrite(u_wecker, HIGH);
  }
}

void WortUhr::setAllLEDs(bool onoff) {

  // alle LEDs an oder aus
  for (int i = u_es; i <= u_wecker; i++) {
    digitalWrite(i, onoff ? HIGH : LOW);
  }
}

void WortUhr::snooze() {
  Serial.println("Snoozing...");
  alarm_minutes = alarm_minutes + 2;
  if (alarm_minutes > 59) {
    alarm_minutes = alarm_minutes - 60;
    alarm_hours = alarm_hours + 1;
    if (alarm_hours >  13) {
      alarm_hours = alarm_hours - 12;
    }
  }
  Serial.print ("Moved alarm time to: ");
  Serial.print(alarm_hours);
  Serial.print(":");
  Serial.println(alarm_minutes);
}

void WortUhr::resetAlarmToOriginal() {
  alarm_hours = original_alarm_hours;
  alarm_minutes = original_alarm_minutes;
  Serial.print ("Reset alarm time back to: ");
  Serial.print(alarm_hours);
  Serial.print(":");
  Serial.println(alarm_minutes);
}


