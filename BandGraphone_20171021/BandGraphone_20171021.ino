/*

*/
#include "ada88.h"  // for Debug

//#define SERIAL_SPEED     (38400)  // for Serial MIDI
#define SERIAL_SPEED     (31250)  // for MIDI Shield
#define NOTE_ON_VELOCITY (100)

#define NUM_MODULE              (5)
#define NUM_CH                  (8)
#define PIN_CH_SELECT_0         (10)
#define ANALOG_PIN_MODULE_0     (0)
#define NOTE_OFF_BRIGHTNESS_MIN (200)     //  0 - 255  0:black, 255:white
#define NOTE_ON_CH_THRESHOLD_LEVEL (150)  //  0 - 255  0:black, 255:white

#define STABLE_COLOR            (2)
#define CHATTERING_MAX          (10)     //  more than 100 msec
#define KEYON_MAX               (1000)   //  more than 10 sec

#define STATE_NO_NOTEON         (0)
#define STATE_WAIT_NOTEON       (1)
#define STATE_NOTEON            (2)

//#define USE_ADA88

typedef struct {
  byte  noteNumber;
  byte  brightnessOfSensors;  //  0 - 255
  byte  midiCh;               //  0 or 1
  byte  state;
  unsigned long   noteTimeStock;  //  Absolute Time(4byte)
} SENSOR_STATE;

SENSOR_STATE ss[NUM_MODULE*NUM_CH];
volatile unsigned long time_m = 0;

const byte TraslateNoteTable[NUM_MODULE*NUM_CH] = {
  0x21, 0x23, 0x24, 0x26, 0x28, 0x29, 0x2b, 0x2d,
  0x2f, 0x30, 0x32, 0x34, 0x35, 0x37, 0x39, 0x3b,
  0x3c, 0x3e, 0x40, 0x41, 0x43, 0x45, 0x47, 0x48,
  0x4a, 0x4c, 0x4d, 0x4f, 0x51, 0x53, 0x54, 0x56,
  0x58, 0x59, 0x5b, 0x5d, 0x5f, 0x60, 0x62, 0x64
};

void setup() {

  pinMode(8, OUTPUT); //  for debug
  pinMode(9, OUTPUT); //  for debug

  pinMode(PIN_CH_SELECT_0 + 0, OUTPUT);
  pinMode(PIN_CH_SELECT_0 + 1, OUTPUT);
  pinMode(PIN_CH_SELECT_0 + 2, OUTPUT);

  digitalWrite(8,LOW);
  digitalWrite(9,LOW);

#ifdef USE_ADA88
    // for Debug
  ada88_init();
  ada88_write(19);
#endif

  Serial.begin(SERIAL_SPEED);

  for ( int i=0; i<NUM_MODULE*NUM_CH; i++ ){
    ss[i].noteNumber = TraslateNoteTable[i];
    ss[i].brightnessOfSensors = 0;
    ss[i].midiCh = 0;
    ss[i].state = STATE_NO_NOTEON;
    ss[i].noteTimeStock = 0;
  }
}

void loop() {

  digitalWrite(8,((time_m%2)==0)?LOW:HIGH); // debug
  for (byte ch = 0; ch < NUM_CH; ch++) {
    selectCh(ch);

    for (byte module = 0; module < NUM_MODULE; module++) {
      byte sensorNumber = (module * NUM_CH) + ch;
      byte currBrightness = analogRead(ANALOG_PIN_MODULE_0 + module) >> 2;
      analyseThreshold(ss[sensorNumber],currBrightness);
    }
  }
}

void analyseThreshold( SENSOR_STATE& ssp, byte currBrightness )
{
  byte prevBrightness = ssp.brightnessOfSensors;
  ssp.brightnessOfSensors = currBrightness;
  time_m = millis();

  //  beginning of color
  if (ssp.state == STATE_NO_NOTEON){
    if ((prevBrightness >= NOTE_OFF_BRIGHTNESS_MIN) &&
      (currBrightness <  NOTE_OFF_BRIGHTNESS_MIN)) {
      ssp.state = STATE_WAIT_NOTEON;
      ssp.noteTimeStock = time_m;
    }
  }

  //  judge state transition
  else {
    unsigned long time = time_m - ssp.noteTimeStock;

    //  for Note On Check
    if ( ssp.state == STATE_WAIT_NOTEON ){
      if ( currBrightness >= NOTE_OFF_BRIGHTNESS_MIN ){
        ssp.state = STATE_NO_NOTEON;
      }
      else if ( time > STABLE_COLOR ){
        ssp.midiCh = (currBrightness > NOTE_ON_CH_THRESHOLD_LEVEL)? 1:0;
        sendMIDINoteOn(ssp.midiCh, ssp.noteNumber, NOTE_ON_VELOCITY);
        ssp.state = STATE_NOTEON;
      }
    }

    //  for Note of Check
    else if ( ssp.state == STATE_NOTEON ){
      if ((currBrightness >= NOTE_OFF_BRIGHTNESS_MIN) &&
          ( time > CHATTERING_MAX )){
        sendMIDINoteOff(ssp.midiCh,ssp.noteNumber);
        ssp.state = STATE_NO_NOTEON;
      }

      if ( time > KEYON_MAX ){
        sendMIDINoteOff(ssp.midiCh,ssp.noteNumber);
        ssp.state = STATE_NO_NOTEON;
      }
    }
  }
}

void selectCh(byte ch) {
  digitalWrite(PIN_CH_SELECT_0 + 0, ((ch & 0x01) != 0) ? HIGH : LOW);
  digitalWrite(PIN_CH_SELECT_0 + 1, ((ch & 0x02) != 0) ? HIGH : LOW);
  digitalWrite(PIN_CH_SELECT_0 + 2, ((ch & 0x04) != 0) ? HIGH : LOW);
  delay(1);
}

void sendMIDINoteOn(byte midiCh, byte noteNumber, byte velocity) {
  Serial.write(0x90 | midiCh);
  Serial.write(noteNumber);
  Serial.write(velocity);
  Serial.flush();
#ifdef USE_ADA88
  ada88_writeDot(sensorNumber,false);
#endif
}

void sendMIDINoteOff(byte midiCh, byte noteNumber) {
  Serial.write(0x80 | midiCh);
  Serial.write(noteNumber);
  Serial.write(0x40);
  Serial.flush();
#ifdef USE_ADA88
  ada88_writeDot(sensorNumber,true);
#endif
}
