# MIDI SWITCH wiring and setup

The diagram preserves D2–D12 assignments from the original `READY CODE FOR LINE6M5 UP_DOWN .ino` sketch and Nano wiring diagram. It shows electrical connections, not physical lead positions. The design has two footswitches and MIDI OUT only.

## Parts list

| Part | Quantity | Notes |
|---|---:|---|
| Classic Arduino Nano ATmega328P, 5 V / 16 MHz | 1 | Original board type |
| Two-digit common-anode 7-segment LED display | 1 | Alternatively, two digits with matching segments connected together |
| Momentary normally-open footswitch | 2 | Contacts close only while pressed; not a latching switch |
| 1.5 kΩ, 0.25 W resistor | 7 | R1–R7: conservative proposed values for direct anode drive |
| 220 Ω, 0.25 W resistor | 2 | R8 and R9: 5 V MIDI OUT |
| 5-pin DIN / 180° socket | 1 | MIDI OUT |
| DC socket, enclosure, wire and mounting board | As needed | As in the original build |
| 9 V DC supply | 1 | Connect to VIN; USB can be used on the bench |

Existing resistor values have not been measured. The proposed 1.5 kΩ values were not identified from the photographs and are not an instruction to replace components in a working unit.

## LED display

| Nano | Through | LED function | Lead number only for the archived LD-D056XXX-C datasheet |
|---|---|---|---:|
| D2 | R1 | A | 10 |
| D3 | R2 | B | 9 |
| D4 | R3 | C | 1 |
| D5 | R4 | D | 4 |
| D6 | R5 | E | 3 |
| D7 | R6 | F | 6 |
| D8 | R7 | G | 5 |
| D9 | Direct | CA1, left digit | 8 |
| D10 | Direct | CA2, right digit | 7 |
| Unconnected | — | DP, decimal point | 2 |

**Physical lead numbers depend on the display model.** The archived datasheet shows a 10-lead package, while an original code comment mentions nine leads at the top and nine at the bottom. Photographs of the finished enclosure do not resolve this discrepancy. For another display model, retain functions A–G, CA1 and CA2 but use its own datasheet. Verify the model before using the lead numbers above.

Segment positions: A top, B upper right, C lower right, D bottom, E lower left, F upper left, G middle. A common-anode digit is enabled with HIGH; an illuminated segment is driven LOW. The firmware disables both digits before changing segment outputs.

### Resistors and brightness

With direct drive, each common-anode pin carries the combined current of all illuminated segments in that digit. A small resistor selected for just one segment can therefore overload D9/D10. The Nano documentation specifies a 20 mA limit per I/O pin.

At 5 V with a 1.9 V LED drop, 1.5 kΩ gives about 2.1 mA per segment, or about 14.5 mA for seven illuminated segments while the digit is active. Multiplexing reduces average current. Green LEDs with a higher forward drop will draw less current and may be dimmer. These are design calculations, not measurements of the original unit.

If the existing build includes transistors or another digit-driving circuit, review that circuit separately. This diagram assumes direct anode connections, as described by the original pin assignments.

## Footswitches

- D11 → one DOWN contact; the other contact → GND.
- D12 → one UP contact; the other contact → GND.
- For a three-terminal switch, identify COM and NO with a multimeter.
- `INPUT_PULLUP` holds each input HIGH at rest. Pressing the switch pulls it LOW. Do not connect the switches to 9 V.

## MIDI OUT

| Nano | Connection | DIN contact |
|---|---|---|
| 5 V | R8 = 220 Ω | 4 |
| D1 / TX | R9 = 220 Ω | 5 |
| GND | Wire | 2 |
| None | Leave unconnected | 1 and 3 |

This is a simple unbuffered 5 V UART output following the original design concept, not a declaration of product electrical certification. Use the socket's contact numbers: the solder-side view is mirrored relative to the mating face. The drawing does not specify the physical terminal layout of a particular panel socket.

Connect **controller MIDI OUT → M5 MIDI IN**. Leave M5 MIDI OUT unused. Data format: 31250 baud, 8 data bits, no parity, 1 stop bit. Do not use `Serial.print()` for debugging because the same port carries MIDI.

## Power

- Positive 9 V DC → Nano VIN.
- DC negative → Nano GND.
- Never apply 9 V to the 5 V pin.
- USB can power the Nano on the bench. Use one power source for initial testing.
- The DC socket's center/sleeve polarity depends on the supply and wiring. Verify it with a multimeter before labeling it; photographs alone do not establish polarity.

## Hardware checks

1. With power disconnected, verify the pin assignments and check for shorts between 5 V–GND and VIN–GND.
2. Disconnect the M5 while uploading. Confirm that the display shows 01 at startup.
3. Power the M5 before restarting the controller, set CH1 and connect MIDI. The controller should select preset 01 and enable the effect after startup.
4. Check 01 → 02 → 03, DOWN, and wrapping between 24 and 01.
5. Check short taps, hold-to-scroll and release behavior.
6. Press both switches together: the preset should stay unchanged, the M5 should bypass and the displayed number should flash. Release both and select another preset: the effect should turn on.
7. Check digits 08, 18 and 24 for legibility, correct digit order and brightness. If the digits are reversed, compare D9/D10 with the actual common-anode connections.

These checks still need to be performed on physical hardware. Simulation does not verify wiring, LED current or the M5 itself.
