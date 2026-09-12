# Original project analysis and firmware cleanup

## Selected baseline

`READY CODE FOR LINE6M5 UP_DOWN .ino` is the closest match to this build: switches on D11/D12, segments A–G on D2–D8, common anodes on D9/D10 and 24 M5 presets. It matches the archived Nano diagram and the builder's description. `NANO SWITCH.txt` contains the same logic with whitespace differences. The archive does not establish which exact file was flashed to the sold units.

| Archived material | Assessment |
|---|---|
| READY CODE / NANO SWITCH | Correct firmware family for this project |
| Pastebin printout dated 2017-11-06 | Similar logic, but reversed segment order, switches 9/10 and anodes 12/13; does not match the later Nano diagram |
| EDUARDO_INZUNZA_SWITCH.ino | Different five-switch/LED design; unreachable case 5 and incorrect parentheses in digitalRead |
| WORSHKY MIDI.txt | Different bank-based design with four preset selectors and extra LEDs |
| tap tempo.txt | Incomplete mixture of TM1637 and Adafruit APIs; not this controller's firmware |
| DISPLAY SHIELD 4 | Four-digit TM1637 example and libraries; does not match the directly connected two-digit display |
| Photographs dated 2017-12-23 | Original completed hardware, including operation alongside an M5 |

## Changes

| Original implementation | Updated implementation | Effect |
|---|---|---|
| Level reading plus delay(250) | Separate 25 ms debounced state per switch | Rejects contact chatter |
| Independent UP and DOWN if statements | One gesture handler with an 80 ms window | Recognizes both switches before changing presets |
| while loop for both switches | One bypass command, then wait for release | Prevents repeated CC traffic and a blocked main loop |
| Standalone Serial.write(contOff) | Complete PC and CC messages only | Removes an orphan data byte |
| fxOn before and after UP's PC | One PC followed by one CC11 on | Removes duplicate messages |
| Unused presets[16] array | PRESET_COUNT = 24 | Centralizes the range and removes dead data |
| Scattered values and outdated comments | Named pin, channel and timing constants | Easier maintenance |
| SevenSeg library missing from the archive | Small standalone Timer2 display driver | No external Arduino library installation |

## Preserved behavior and visible differences

Pin assignments, directions, 24-preset range, wrapping, hold-to-scroll, startup at preset 01, effect-on after preset selection and the two-switch bypass function are preserved. MIDI defaults to channel 1 at 31250 baud. M5 uses PC 0–23 for presets 01–24, and CC11 values 0–63 for bypass or 64–127 for on. The original on value of 70 was valid; using 127 is a clearer full-scale on value, not a correction of an invalid range.

Visible differences: presets 01–09 have a leading zero; startup shows 01 instead of a brief “on”; the number flashes during bypass. Initial selection takes up to about 105 ms (25 + 80 ms) to allow recognition of both switches. Auto-repeat starts 600 ms after the initial change, then repeats every 250 ms, replacing the original immediate repetition roughly every 275 ms. Set `AUTO_REPEAT = false` to disable hold-to-scroll.

Timer2 refreshes each digit 250 times per second; Timer0 remains available for millis(). The firmware uses no delay() calls or blocking switch loops. Both anodes are disabled before segment changes. The single-byte display number is sampled at the start of each complete pair of digits. Timer2 is reserved: adding tone() or another Timer2 library requires revisiting the display driver.

The design has no MIDI IN, tap tempo, banks or EEPROM storage. Changes made directly on the M5 do not update the controller display.

## Limits of the analysis

The revised firmware was compiled for AVR and simulated, but has not been tested on the original hardware. Installed resistor values, physical LED pinout and DC socket polarity need verification on a unit. The archive contains conflicting 10-lead/18-lead display information; see the wiring guide. Protocol and Nano references are linked from the main README.
