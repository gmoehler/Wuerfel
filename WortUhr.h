#ifndef WortUhr_h
#define WortUhr_h

#include <Adafruit_LEDBackpack.h>
#include <RTClib.h>
#include <ClickEncoder.h>

// I2C address of the display.  Stick with the default address of 0x70
// unless you've changed the address jumpers on the back of the display.
#define DISPLAY_ADDRESS   0x70

/**
   Klasse die die WortUhr betreibt, und mit der man die Zeit durch Rotary
   Encoder setzen kann.
 **/
class WortUhr {
  public:
    WortUhr();

    // to be called during setup
    void setup();

    // to be called during each millisecond to keep wort uhr current
    void tick1ms();

    // to be called in the loop to set the wort uhr each minute
    void loop();

    // start the setting phase
    void startSetting();
    void startAlarmSetting();

    bool isSettingFinished();
    bool isAlarmSettingFinished();

    // wecken!
    bool isAlarmTimeNow();
    // wecker an?
    bool isAlarmActive();

    // to be called to set the clock using the rotary encoder
    void setWithRotaryEncoder(ClickEncoder *encoder, int step, ClickEncoder::Button b);

    // to be called to set the clock using the rotary encoder
    void setAlarmWithRotaryEncoder(ClickEncoder *encoder, int step, ClickEncoder::Button b);

    void setAllLEDs(bool onoff);

    // Alarm um 3 min rausschieben
    void snooze();

    // Snooze-Verschiebung rueckgaengig machen
    void resetAlarmToOriginal();

  private:
    // display time on WortUhr
    void displayUhrzeit();

    // set minutes and hours (either time or alarm)
    void setMinutes(int s, int &mins);
    void setHours(int s, int &hrs);
    void setAlarmOnOff(int s);

    // show full time, hours only, minutes only
    void blinkColon();
    void showTimeOn7Seg(int hrs, int mins);
    void showHoursOn7Seg(int hrs);
    void showMinsOn7Seg(int mins);
    void showOnOffOn7Seg(bool onoff);

    // get time from time shield and save it back
    void getTimeFromDs3231();
    void saveTimeOnDs3231();

    // set timer time to compile time
    void setInitialTimeOnDs3231();

    // printout phase to serial output
    void printToSerialPhase();

    // 7-segment display for clock
    Adafruit_7segment clockDisplay;

    // DS3132 clock shield
    RTC_DS1307 rtc;

    // Stunden und Minuten der aktuellen Zeit
    int hours;
    int minutes;

    // Stunden und Minuten des Alarms
    int alarm_hours;
    int alarm_minutes;

    // Zustaende waehrend des Setzens
    // 0: uhr läuft; 1: stunden setzen, 2: minuten setzen
    int set_uhr_phase;
    
    // Zustaende waehrend des Alarm Setzens
    // 0: Normalbetrieb ; 1: alarm anzeigen 2: stunden setzen, 3: minuten setzen
    int set_alarm_phase;

    // ist Alarm angeschaltet
    bool alarm_active;

    // ist Alamzeit
    bool alarm_time_now;

    // output pins for words
    int u_es;
    int u_ist;
    int u_zehn;
    int u_fuenf;
    int u_vor;
    int u_nach;
    int u_drei;
    int u_viertel;
    int u_halb;
    int u_1;
    int u_2;
    int u_3;
    int u_4;
    int u_5;
    int u_6;
    int u_7;
    int u_8;
    int u_9;
    int u_10;
    int u_11;
    int u_12;
    int u_uhr;
    int u_wecker;

    // zaehler, in dem die setps des Rotary Encoders angesammelt werden
    int steps;

    long count_ms;

    long correct_seconds;

    long uhr_update_interval;

    long last_minutes;

    int original_alarm_hours;

    int original_alarm_minutes;
};
#endif
