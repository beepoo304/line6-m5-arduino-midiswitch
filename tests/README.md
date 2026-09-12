# Weryfikacja — 2026-09-12

Kompilacja: Arduino CLI, Arduino AVR Boards 1.8.8, `arduino:avr:nano:cpu=atmega328old`. Wynik: 3398 B Flash (z 30720), 240 B RAM (z 2048). Brak zewnętrznych bibliotek Arduino.

Skompilowany plik HEX uruchomiono w AVR8js 0.21.1 z modelami ATmega328P, UART, Timer0, Timer2 i GPIO. **45 sprawdzeń zakończonych powodzeniem.** Test nie jest przepisaniem logiki szkicu do JavaScript — wykonuje instrukcje skompilowanego programu AVR.

Sprawdzono start i ramki MIDI, 31250 baud, oba kierunki, zawijanie zakresu, pełny cykl 24 presetów, krótkie naciśnięcie, drgania styków, opóźnienie i częstotliwość powtarzania, zatrzymanie po zwolnieniu, bypass dla obu kolejności przycisków, brak zalewania MIDI, częściowe zwolnienie, późny drugi przycisk i powrót z bypassu. Monitorowano wyjścia anod: nie były jednocześnie aktywne, obie były odświeżane.

Nie przetestowano fizycznego M5, jasności LED, prądów, rzeczywistej transmisji elektrycznej DIN ani długotrwałej pracy po przepełnieniu millis(). Kod używa odejmowania bez znaku dla interwałów przycisków.

## Powtórzenie

Wymagane Arduino CLI, Node.js 20.19+ i npm. Z katalogu projektu:

```sh
arduino-cli core install arduino:avr@1.8.8
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old --build-path build .
npm install --no-save --package-lock=false avr8js@0.21.1
node tests/simulate.cjs build/MIDI_SWITCH.ino.hex
```

Test sprawdza domyślną konfigurację kanału, liczby presetów i czasów. Po zmianie konfiguracji należy odpowiednio zmienić oczekiwania testu. Próba na fizycznym urządzeniu: `hardware/POLACZENIA.md`.
