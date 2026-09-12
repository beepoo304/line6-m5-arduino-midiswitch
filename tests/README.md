# Verification — 2026-09-12

Compiled using Arduino CLI, Arduino AVR Boards 1.8.8 and `arduino:avr:nano:cpu=atmega328old`. Result: 3398 bytes of Flash out of 30720, and 240 bytes of RAM out of 2048. No external Arduino libraries are required.

The compiled HEX was executed in AVR8js 0.21.1 with ATmega328P, UART, Timer0, Timer2 and GPIO models. **All 45 checks passed.** The test executes the compiled AVR instructions rather than reimplementing the firmware logic in JavaScript.

Coverage includes startup MIDI messages, 31250 baud, both directions, range wrapping, a full 24-preset cycle, short taps, contact chatter, repeat delay and interval, stopping on release, bypass with either switch pressed first, absence of MIDI flooding, partial release, a late second switch and recovery from bypass. Anode outputs were monitored: both digits refreshed and were never enabled simultaneously.

Physical M5 behavior, LED brightness and current, the DIN electrical interface and extended operation across millis() rollover have not been tested. Switch timing uses unsigned subtraction for elapsed intervals.

## Reproduce the checks

Requirements: Arduino CLI, Node.js 20.19+ and npm. Run from the project folder, named `MIDI_SWITCH`:

```sh
arduino-cli core install arduino:avr@1.8.8
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old --build-path build .
npm install --no-save --package-lock=false avr8js@0.21.1
node tests/simulate.cjs build/MIDI_SWITCH.ino.hex
```

The test expects the default channel, preset count and timing configuration. Update expectations if those settings change. Physical test procedure: [hardware checks](../hardware/POLACZENIA.md#hardware-checks).
