#include <Arduino.h>
#include <SmartRC_CC1101.h>
#include "button_config.h"

SmartRC_CC1101 radio;

constexpr int RX_PIN = 27;
constexpr int MAX_PULSES = 200;

volatile uint16_t pulses[MAX_PULSES];
volatile uint16_t pulseCount = 0;
volatile uint32_t lastEdge = 0;
volatile bool frameReady = false;

uint16_t snapshot[MAX_PULSES];

uint32_t previousCode = 0;
uint8_t repeats = 0;

uint32_t frameCount = 0;
uint32_t decodedCount = 0;

bool isShort(uint16_t d) {
  return d >= 170 && d <= 600;
}

bool isLong(uint16_t d) {
  return d >= 700 && d <= 1500;
}

void IRAM_ATTR onEdge() {
  uint32_t now = micros();
  uint32_t duration = now - lastEdge;
  lastEdge = now;

  if (frameReady) return;

  if (duration > 6000) {
    if (pulseCount >= 48) {
      frameReady = true;
    } else {
      pulseCount = 0;
    }
    return;
  }

  if (duration >= 100 && duration <= 5000) {
    if (pulseCount < MAX_PULSES) {
      pulses[pulseCount++] = duration;
    } else {
      frameReady = true;
    }
  }
}

void decodeFrame(uint16_t *data, int count) {
  frameCount++;

  // EV1527: 24 бита, по два импульса на бит.
  // Ищем последовательность в любом месте кадра.
  for (int offset = 0; offset + 48 <= count; offset++) {
    uint32_t code = 0;
    bool valid = true;

    for (int bit = 0; bit < 24; bit++) {
      uint16_t a = data[offset + bit * 2];
      uint16_t b = data[offset + bit * 2 + 1];

      code <<= 1;

      if (isShort(a) && isLong(b)) {
        // Бит 0
      } else if (isLong(a) && isShort(b)) {
        code |= 1;
      } else {
        valid = false;
        break;
      }
    }

    if (!valid) continue;

    decodedCount++;


#ifdef TINKER_BELL_LEARN_MODE
    // Diagnostic mode: report RF codes without sending DOORBELL.
    static uint32_t lastPrintedCode = 0;
    static uint32_t lastPrintMs = 0;
    uint32_t printNow = millis();
    if (code != lastPrintedCode || printNow - lastPrintMs >= 1000) {
      Serial.printf("BUTTON_CODE=0x%06lX\n", (unsigned long)code);
      lastPrintedCode = code;
      lastPrintMs = printNow;
    }
#else
    constexpr uint32_t DEBOUNCE_MS = 1500;

    static uint32_t lastBellMs = 0;
    static bool bellSent = false;

    if (code == BUTTON_CODE) {
      uint32_t now = millis();

      if (!bellSent || now - lastBellMs >= DEBOUNCE_MS) {
        Serial.println("DOORBELL");
        lastBellMs = now;
        bellSent = true;
      }
    }

#endif
    return;
  }

  if (frameCount <= 10) {
    Serial.printf("UNDECODED pulses=%d\n", count);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("EV1527 DECODER START");

  radio.setSpiPin(18, 19, 23, 5);
  radio.setGDO0(RX_PIN);
  radio.Init();

  if (!radio.getCC1101()) {
    Serial.println("CC1101 ERROR");
    return;
  }

  radio.setCCMode(false);
  radio.setModulation(2);
  radio.setMHZ(433.92);
  radio.setRxBW(325.0);
  radio.setDRate(4.8);
  radio.setPktFormat(3);
  radio.SpiWriteReg(CC1101_IOCFG0, 0x0D);
  radio.SetRx();

  pinMode(RX_PIN, INPUT);
  lastEdge = micros();

  attachInterrupt(
    digitalPinToInterrupt(RX_PIN),
    onEdge,
    CHANGE
  );

  Serial.println("RADIO READY");
}

void loop() {
  bool ready;

  noInterrupts();
  ready = frameReady ||
    (pulseCount >= 48 &&
     micros() - lastEdge > 12000);
  interrupts();

  if (ready) {
    detachInterrupt(digitalPinToInterrupt(RX_PIN));

    noInterrupts();

    int count = pulseCount;

    for (int i = 0; i < count; i++) {
      snapshot[i] = pulses[i];
    }

    pulseCount = 0;
    frameReady = false;
    lastEdge = micros();

    interrupts();

    attachInterrupt(
      digitalPinToInterrupt(RX_PIN),
      onEdge,
      CHANGE
    );

    decodeFrame(snapshot, count);
  }

  static uint32_t lastStatus = 0;

  if (millis() - lastStatus >= 3000) {
    lastStatus = millis();

    Serial.printf(
      "STAT frames=%lu decoded=%lu\n",
      (unsigned long)frameCount,
      (unsigned long)decodedCount
    );
  }

  delay(1);
}
