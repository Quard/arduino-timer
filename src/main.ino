#include <Arduino.h>
#include <Wire.h>
#include <Ticker.h>

#include "display.h"

#define BUZZER 9
#define RELAY_PIN 13
#define ENC_PIN_A 4
#define ENC_PIN_B 3
#define BTN_PIN 2


volatile bool on = false;
volatile bool modeChanged = false;
volatile unsigned long count = 0;
volatile unsigned long actualcount = 0;
volatile int state = 0;
volatile unsigned long countChangeDuration = 0;
volatile unsigned int lastCountChanged = 0;
volatile int clicking = 0;

volatile int pinButtonValue = 0;
volatile long timeButtonDown = 0;
unsigned long timeButtonPressed = 1500;
volatile bool isButtonDown = false;
volatile int encAValue = 0;
volatile int encBValue = 0;


void countDown();

Ticker timer(countDown, 1000);

void setup() {
  Serial.begin(115200);
  displayBegin();

  pinMode(ENC_PIN_A, INPUT);
  pinMode(ENC_PIN_B, INPUT);
  pinMode(BTN_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(ENC_PIN_A), enc_int, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_PIN_B), enc_int, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), btn_int, CHANGE);

  displayDuration(on, actualcount);

  playBoot();
}

void loop(void) {
  if (count != actualcount) {
    actualcount = count;

    displayDuration(on, actualcount);
  } else if (modeChanged) {
    displayDuration(on, actualcount);
    modeChanged = false;
  }

  timer.update();
}

void start() {
  on = true;
  modeChanged = true;
  digitalWrite(RELAY_PIN, HIGH);
  timer.start();
}

void stop() {
  on = false;
  modeChanged = true;
  digitalWrite(RELAY_PIN, LOW);
  timer.stop();
}

void btn_int() {
  if (millis() - timeButtonDown < 50) return;

  pinButtonValue = digitalRead(BTN_PIN);

  cli();
  timeButtonDown = millis();
  if (pinButtonValue) {
    isButtonDown = true;
  } else if (isButtonDown) {
    clicking = 1;
    if (on) {
      stop();
    } else {
      playStart();
      start();
    }
    isButtonDown = false;
  }
  sei();
}

void enc_int() {
  if (on) return;
  encAValue = digitalRead(ENC_PIN_A);
  encBValue = digitalRead(ENC_PIN_B);

  cli();
  if (!encAValue && encBValue) state = 1;
  if (!encAValue && !encBValue) state = -1;
  if (!encAValue && state != 0) {
    if ((state == 1 && encBValue) || (state == -1 && !encBValue)) {
      int factor = 1;
      unsigned int delta = millis() - lastCountChanged;
      if (delta < 100) {
        if (countChangeDuration == 0) {
          countChangeDuration = millis();
        }
        delta = millis() - lastCountChanged;
        float coef = (float)(300 - delta) / 100;
        factor = max(1, (int) pow(coef, 3));
      } else if (countChangeDuration != 0) {
        countChangeDuration = 0;
      }
      lastCountChanged = millis();
      int step = state * factor;
      if (step < 0 && count < abs(step)) {
        count = 0;
      } else {
        count += step;
      }
      state = 0;
    }
  }
  sei();
}

void countDown() {
  if (on) {
    if (count > 0) {
      count -= 1;
    }
    if (count == 0) {
      stop();
      playStop();
    }
  }
}

void playBoot() {
  tone(BUZZER, 147);
  delay(200);
  tone(BUZZER, 330);
  delay(200);
  tone(BUZZER, 622);
  delay(200);
  tone(BUZZER, 880);
  delay(200);
  tone(BUZZER, 1109);
  delay(200);
  noTone(BUZZER);
}

void playStart() {
  tone(BUZZER, 1000);
  delay(200);
  noTone(BUZZER);
}

void playStop() {
  for (int i=0; i<3; i++) {
    tone(BUZZER, 1000);
    delay(400);
    noTone(BUZZER);
    delay(200);
  }
}
