/*
 * MIDI SWITCH - Pawel, wersja uporzadkowana 2026-09-12
 * Arduino Nano CLASSIC ATmega328P, 5 V / 16 MHz; Line 6 M5.
 * D2..D8 = segmenty A..G; D9/D10 = anody lewej/prawej cyfry.
 * D11 = DOWN, D12 = UP; chwilowe styki NO zwierane do GND.
 * D1/TX = MIDI OUT przez 220 ohm. Schemat i instrukcja w projekcie.
 * Bez bibliotek zewnetrznych. Timer2 jest zarezerwowany dla ekranu.
 */
#include <Arduino.h>
#include <avr/interrupt.h>

#if !defined(__AVR_ATmega328P__) || F_CPU != 16000000UL
#error Ten szkic wymaga ATmega328P 16 MHz (klasyczne Nano lub Uno).
#endif

constexpr uint8_t MIDI_CHANNEL = 1;    // 1..16; ustaw taki sam w M5
constexpr uint8_t PRESET_COUNT = 24;   // M5: PC 0..23 -> ekran 1..24
constexpr bool AUTO_REPEAT = true;    // przewijanie przy przytrzymaniu
constexpr uint32_t DEBOUNCE_MS = 25;
constexpr uint32_t CHORD_MS = 80;      // okno na nacisniecie obu pedalow
constexpr uint32_t REPEAT_DELAY_MS = 600;
constexpr uint32_t REPEAT_MS = 250;
constexpr uint32_t STARTUP_MS = 1000;
constexpr uint8_t DOWN_PIN = 11;
constexpr uint8_t UP_PIN = 12;
constexpr uint8_t SEGMENT_PINS[7] = {2, 3, 4, 5, 6, 7, 8};
constexpr uint8_t DIGIT_PINS[2] = {9, 10};
static_assert(MIDI_CHANNEL >= 1 && MIDI_CHANNEL <= 16, "Kanal MIDI: 1..16");
static_assert(PRESET_COUNT >= 1 && PRESET_COUNT <= 99, "Ekran: maks. 99");

// Bity 0..6 = A..G; 1 oznacza zapalony segment.
const uint8_t DIGITS[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66,
                            0x6D, 0x7D, 0x07, 0x7F, 0x6F};
volatile uint8_t displayNumber = 1;   // atomowy odczyt na 8-bitowym AVR
volatile bool displayVisible = true;

// 500 przerwan/s; kazda cyfra odswiezana 250 razy/s.
// Najpierw gasimy obie anody, dopiero potem zmieniamy segmenty.
ISR(TIMER2_COMPA_vect) {
  static uint8_t digit = 0;
  static uint8_t frameNumber = 1;
  static bool frameVisible = true;
  digitalWrite(DIGIT_PINS[0], LOW);
  digitalWrite(DIGIT_PINS[1], LOW);
  if (digit == 0) {
    frameNumber = displayNumber;
    frameVisible = displayVisible;
  }
  const uint8_t value = digit == 0 ? frameNumber / 10 : frameNumber % 10;
  const uint8_t pattern = DIGITS[value];
  for (uint8_t s = 0; s < 7; ++s) {
    digitalWrite(SEGMENT_PINS[s], (pattern & (1U << s)) ? LOW : HIGH);
  }
  if (frameVisible) digitalWrite(DIGIT_PINS[digit], HIGH);
  digit ^= 1;
}

struct Button {
  uint8_t pin;
  bool raw = false;
  bool pressed = false;
  uint32_t changedAt = 0;
  explicit Button(uint8_t p) : pin(p) {}
  void begin(uint32_t now) {
    pinMode(pin, INPUT_PULLUP);
    raw = pressed = digitalRead(pin) == LOW;
    changedAt = now;
  }
  void update(uint32_t now) {
    const bool sample = digitalRead(pin) == LOW;
    if (sample != raw) { raw = sample; changedAt = now; }
    if (uint32_t(now - changedAt) >= DEBOUNCE_MS) pressed = raw;
  }
};
Button down(DOWN_PIN), up(UP_PIN);
enum class Gesture : uint8_t { Locked, Idle, Pending, Single };
Gesture gesture = Gesture::Locked;
int8_t direction = 0;
uint8_t preset = 0;
bool fxEnabled = true;
bool started = false;
bool repeating = false;
uint32_t bootAt = 0, gestureAt = 0, repeatAt = 0;

void sendFx(bool enabled) {
  Serial.write(uint8_t(0xB0 | (MIDI_CHANNEL - 1)));
  Serial.write(uint8_t(11));
  Serial.write(uint8_t(enabled ? 127 : 0));
  fxEnabled = enabled;
}

void sendPreset() {
  Serial.write(uint8_t(0xC0 | (MIDI_CHANNEL - 1)));
  Serial.write(preset);
  sendFx(true);
  displayNumber = preset + 1;
}

void stepPreset() {
  if (direction > 0) preset = (preset + 1) % PRESET_COUNT;
  else preset = preset == 0 ? PRESET_COUNT - 1 : preset - 1;
  sendPreset();
}

void setup() {
  Serial.begin(31250);
  for (uint8_t s = 0; s < 7; ++s) {
    digitalWrite(SEGMENT_PINS[s], HIGH);
    pinMode(SEGMENT_PINS[s], OUTPUT);
  }
  for (uint8_t d = 0; d < 2; ++d) {
    digitalWrite(DIGIT_PINS[d], LOW);
    pinMode(DIGIT_PINS[d], OUTPUT);
  }
  bootAt = millis();
  down.begin(bootAt);
  up.begin(bootAt);
  const uint8_t savedSreg = SREG;
  cli();
  TCCR2A = _BV(WGM21);                // CTC, 16 MHz / 128 / 250 = 500 Hz
  TCCR2B = _BV(CS22) | _BV(CS20);
  TCNT2 = 0;
  OCR2A = 249;
  TIFR2 = _BV(OCF2A);
  TIMSK2 = _BV(OCIE2A);
  SREG = savedSreg;
}

void loop() {
  const uint32_t now = millis();
  down.update(now);
  up.update(now);
  if (!started) {
    if (uint32_t(now - bootAt) < STARTUP_MS) return;
    sendPreset();
    started = true;
  }
  // W bypassie numer miga; kolejny wybor presetu wlacza efekt.
  displayVisible = fxEnabled || ((now / 500UL) % 2 == 0);
  const bool both = down.pressed && up.pressed;
  const bool neither = !down.pressed && !up.pressed;
  switch (gesture) {
    case Gesture::Locked:
      if (neither && !down.raw && !up.raw) gesture = Gesture::Idle;
      break;
    case Gesture::Idle:
      if (both) { sendFx(false); gesture = Gesture::Locked; }
      else if (!neither) {
        direction = up.pressed ? 1 : -1;
        gestureAt = now;
        gesture = Gesture::Pending;
      }
      break;
    case Gesture::Pending:
      if (both) { sendFx(false); gesture = Gesture::Locked; }
      else if (neither || uint32_t(now - gestureAt) >= CHORD_MS) {
        stepPreset();
        repeatAt = now;
        repeating = false;
        gesture = neither ? Gesture::Idle : Gesture::Single;
      }
      break;
    case Gesture::Single: {
      const bool held = direction > 0 ? up.pressed : down.pressed;
      if (both) { gesture = Gesture::Locked; break; }
      if (!held) { gesture = neither ? Gesture::Idle : Gesture::Locked; break; }
      const uint32_t interval = repeating ? REPEAT_MS : REPEAT_DELAY_MS;
      if (AUTO_REPEAT && uint32_t(now - repeatAt) >= interval) {
        stepPreset();
        repeatAt = now;
        repeating = true;
      }
      break;
    }
  }
}
