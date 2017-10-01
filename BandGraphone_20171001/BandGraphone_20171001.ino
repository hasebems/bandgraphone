/*

*/
#include "ada88.h"  // for Debug

#define MIDI_CH          (0)
#define SERIAL_SPEED     (38400)  // for Serial MIDI  //(31250)  // for MIDI Shield
#define NOTE_NUMBER_BASE (40)
#define NOTE_ON_VELOCITY (100)

#define NUM_MODULE              (5)
#define NUM_CH                  (8)
#define PIN_CH_SELECT_0         (10)
#define ANALOG_PIN_MODULE_0     (1)
#define NOTE_OFF_BRIGHTNESS_MIN (128)

byte brightnessOfSensors[NUM_MODULE][NUM_CH] = {0};

void setup() {
  pinMode(PIN_CH_SELECT_0 + 0, OUTPUT);
  pinMode(PIN_CH_SELECT_0 + 1, OUTPUT);
  pinMode(PIN_CH_SELECT_0 + 2, OUTPUT);

  Serial.begin(SERIAL_SPEED);

  // for Debug
  ada88_init();
  ada88_write(19);
}

void loop() {
  for (byte ch = 0; ch < NUM_CH; ch++) {
    selectCh(ch);

    for (byte module = 0; module < NUM_MODULE; module++) {
      byte noteNumber = (module * NUM_CH) + ch + NOTE_NUMBER_BASE;

      byte prevBrightness = brightnessOfSensors[module][ch];
      byte currBrightness = analogRead(ANALOG_PIN_MODULE_0 + module) >> 2;
      brightnessOfSensors[module][ch] = currBrightness;

      if ((prevBrightness <  NOTE_OFF_BRIGHTNESS_MIN) &&
          (currBrightness >= NOTE_OFF_BRIGHTNESS_MIN)) {
        sendMIDINoteOff(noteNumber);
      }
      if ((prevBrightness >= NOTE_OFF_BRIGHTNESS_MIN) &&
          (currBrightness <  NOTE_OFF_BRIGHTNESS_MIN)) {
        sendMIDINoteOn(noteNumber, NOTE_ON_VELOCITY);
      }

      // for Debug
      if ((module == 0) && (ch == 0)) {
        ada88_writeNumber(10 - currBrightness / 25);
      }
    }
  }
}

void selectCh(byte ch) {
  digitalWrite(PIN_CH_SELECT_0 + 0, ((ch & 0x01) != 0) ? HIGH : LOW);
  digitalWrite(PIN_CH_SELECT_0 + 1, ((ch & 0x02) != 0) ? HIGH : LOW);
  digitalWrite(PIN_CH_SELECT_0 + 2, ((ch & 0x04) != 0) ? HIGH : LOW);
  delay(2);
}

void sendMIDINoteOn(byte noteNumber, byte velocity) {
  Serial.write(0x90 | MIDI_CH);
  Serial.write(noteNumber);
  Serial.write(velocity);
  Serial.flush();
}

void sendMIDINoteOff(byte noteNumber) {
  Serial.write(0x80 | MIDI_CH);
  Serial.write(noteNumber);
  Serial.write(0);
  Serial.flush();
}
