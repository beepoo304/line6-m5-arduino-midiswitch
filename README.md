# Line 6 M5 Stompbox Modeler — Arduino MidiSwitch

A compact Arduino Nano MIDI foot controller for the **Line 6 M5 Stompbox Modeler**: two footswitches, a two-digit LED display and one MIDI OUT connection.

Originally built by Paweł in 2017–2018. Several units were built and sold, with positive feedback from their owners. This repository preserves the original build photographs and sketches alongside cleaned-up firmware, an annotated English wiring diagram and simulation tests.

![Original Arduino MidiSwitch displaying preset 19](docs/photos/IMG_20171223_160315409.jpg)

## Features

- Select M5 presets **01–24** using UP and DOWN footswitches.
- Wrap from preset 24 to 01 and from 01 to 24.
- Hold either switch to scroll through presets.
- Press both switches together to bypass the effect.
- Select another preset to enable the effect again.
- Drive a common-anode LED display directly from the Nano.
- Build the firmware without external Arduino libraries.

The display shows the last preset **sent by this controller**. This is a MIDI OUT-only design; changes made directly on the M5 are not received. Preset numbers 01–24 are separate from MIDI channel numbers 1–16.

## Project files

- [Arduino firmware](MIDI_SWITCH.ino)
- [English wiring diagram — PDF](hardware/schemat.pdf), [PNG](hardware/schemat.png), [editable SVG](hardware/schemat.svg)
- [Wiring, parts list and hardware setup](hardware/POLACZENIA.md)
- [Original project analysis and firmware changes](docs/ANALIZA.md)
- [Test results and reproduction instructions](tests/README.md)
- [Archived original sketch](archive/READY_CODE_original.ino.txt)

![Annotated English wiring diagram](hardware/schemat.png)

## Controls

| Action | Result |
|---|---|
| Power on | Display shows 01; after about one second, send PC 0 and enable the effect |
| UP — D12 | Next preset; wrap from 24 to 01 |
| DOWN — D11 | Previous preset; wrap from 01 to 24 |
| Hold either switch | First repeat 600 ms after the initial change, then every 250 ms |
| Press both together | Bypass without changing the preset; the displayed number flashes |
| Select a preset after bypass | Change the preset and enable the effect |

Each switch is debounced for 25 ms. An 80 ms window after the first debounced press allows both switches to be recognized together. A short tap takes effect on debounced release or when that window expires. If the second switch is pressed after a single-switch action has already completed, the controller waits for both to be released without triggering bypass. Switches held during startup must be released before use.

## Upload the firmware

1. After downloading or cloning the repository, name its folder `MIDI_SWITCH` so it matches `MIDI_SWITCH.ino`. Open that sketch in Arduino IDE.
2. Select the classic **Arduino Nano / ATmega328P / 16 MHz**, using **Arduino AVR Boards**. The sketch targets the classic AVR Nano, not Nano Every, ESP32 or Nano R4.
3. Older Nano clones commonly need **ATmega328P (Old Bootloader)**. Select **ATmega328P** if your board uses the newer bootloader.
4. Select the USB port and upload. Disconnect the MIDI cable from the M5 during upload so programmer traffic does not reach the pedal.
5. Set the M5 MIDI channel to **CH1**, matching the default `MIDI_CHANNEL = 1` in the sketch.
6. Connect **controller MIDI OUT → M5 MIDI IN**, then follow the [hardware checks](hardware/POLACZENIA.md#hardware-checks).

## Verification status

The firmware compiled with Arduino AVR Boards 1.8.8, using **3398 bytes of Flash and 240 bytes of RAM**. The compiled AVR program passed **45 simulation checks**.

The updated firmware and wiring diagram still need verification on a physical unit. The photographs show the original working hardware, not a hardware test of this firmware revision. Existing resistor values and the exact display pinout must be checked against the actual unit; the archived materials contain conflicting display package information.

## Original build photographs

![Original enclosure, power socket and MIDI OUT connector](docs/photos/IMG_20171223_162222044.jpg)

![Arduino MidiSwitch connected to a Line 6 M5](docs/photos/IMG_20171223_160323967.jpg)

All three photographs supplied by the builder are included unchanged. Git history separates the original archive and photos, firmware cleanup, documentation and tests, and English schematic labels.

## Archive and licensing

The original sketches are preserved in their original language. No open-source license has been assigned to the archived material or photographs.

## Technical references

- [Line 6 M5 Pilot's Handbook — MIDI Control](https://line6.com/data/6/0a060b316ac34f0593fa7e002/application/pdf/M5%20Pilot): PC 0–23; CC11 values 0–63 for bypass and 64–127 for on.
- [Arduino Nano pinout](https://content.arduino.cc/assets/Pinout-NANO_latest.pdf): pin functions and I/O current limits.
- [MIDI Association — DIN electrical specifications](https://midi.org/5-pin-din-electrical-specs).
- The builder's archived Nano diagram and local `LD-D056Uxx-11.pdf` display datasheet; compatibility notes are included in the [wiring guide](hardware/POLACZENIA.md).
