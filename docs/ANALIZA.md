# Analiza archiwum i uporządkowanie kodu

## Wybrana baza

`READY CODE FOR LINE6M5 UP_DOWN .ino` jest najlepiej dopasowaną bazą: dwa przyciski D11/D12, segmenty A–G na D2–D8, anody D9/D10 i 24 presety M5. Odpowiada staremu schematowi Nano oraz opisowi użytkownika. `NANO SWITCH.txt` ma tę samą logikę i różnice w białych znakach. Nie można z tego ustalić, który plik faktycznie wgrano do sprzedanych egzemplarzy.

| Materiał | Ocena |
|---|---|
| READY CODE / NANO SWITCH | Właściwa rodzina kodu dla tego projektu |
| Wydruk Pastebin z 06.11.2017 | Podobny kod, ale odwrotna kolejność segmentów, przyciski 9/10 i anody 12/13; nie zgadza się z późniejszym schematem Nano |
| EDUARDO_INZUNZA_SWITCH.ino | Inna konstrukcja, 5 przycisków i LED; niedostępny case 5, błędne nawiasy w digitalRead |
| WORSHKY MIDI.txt | Inna konstrukcja z bankami, czterema wyborami i dodatkowymi LED |
| tap tempo.txt | Niekompletna mieszanka API TM1637 i Adafruit; to nie firmware tego switcha |
| DISPLAY SHIELD 4 | Przykład i biblioteki czterocyfrowego TM1637; nie odpowiadają dwucyfrowemu LED podłączonemu bezpośrednio |
| Zdjęcia z 23.12.2017 | Dokumentacja zbudowanego urządzenia, w tym praca obok M5 |

## Co poprawiono

| Wcześniej | Teraz | Skutek |
|---|---|---|
| Odczyt poziomu + delay(250) | Osobny stabilizowany stan każdego przycisku, 25 ms | Drgania nie powodują nieplanowanych wyborów |
| Dwa niezależne if dla UP i DOWN | Jedna obsługa gestu, okno 80 ms | Rozpoznanie wspólnego naciśnięcia przed zmianą presetu |
| while przy obu przyciskach | Jednorazowy bypass i oczekiwanie na zwolnienie | Brak lawiny CC oraz blokowania programu |
| Samotny Serial.write(contOff) | Wyłącznie kompletne PC i CC | Usunięcie osieroconego bajtu danych z transmisji |
| fxOn przed i po PC w UP | Jeden PC, następnie jeden CC11 on | Mniej zbędnych komunikatów |
| Nieużywana tablica presets[16] | Stała PRESET_COUNT = 24 | Jedno źródło zakresu i brak martwej tablicy |
| Rozproszone liczby i słabo dopasowane komentarze | Stałe pinów, kanału i czasów; osobne funkcje | Łatwiejsze utrzymanie |
| Biblioteka SevenSeg, której nie było w archiwum | Własny krótki sterownik Timer2 | Szkic kompiluje się bez instalowania starej biblioteki |

## Zachowana funkcjonalność i widoczne różnice

Zachowano piny, kierunki, zakres 24 presetów, zapętlenie, automatyczne przewijanie, start od presetu 01, włączenie efektu po wyborze oraz wspólny bypass. MIDI nadal pracuje na kanale 1 z prędkością 31250 baud. M5 przyjmuje 0–23 dla presetów 01–24 i CC11 z wartościami 0–63 dla bypassu, 64–127 dla on. Wartość 70 w oryginale była prawidłowa; 127 jest tutaj czytelną wartością „on”, nie naprawą błędnego zakresu.

Widoczne różnice: ekran pokazuje 01–09 z zerem wiodącym; na starcie 01 zamiast chwilowego „on”; w bypassie numer miga. Pierwsza zmiana po wciśnięciu ma opóźnienie do około 105 ms (25 + 80 ms), umożliwiające rozpoznanie obu przycisków. Powtarzanie zaczyna się po 600 ms od pierwszej zmiany zamiast natychmiast co około 275 ms. `AUTO_REPEAT = false` wyłącza przewijanie przy trzymaniu.

Timer2 odświeża każdą cyfrę 250 razy na sekundę. Timer0 pozostaje dla millis(). Kod nie używa delay ani blokującej pętli przycisków. Przed przestawieniem segmentów obie anody są wyłączane. Jednobajtowy numer ekranu jest pobierany na początek pełnej pary cyfr. Timer2 jest zarezerwowany: nie dodawaj tone() ani innej biblioteki używającej tego timera bez przebudowy obsługi ekranu.

Nie dodano MIDI IN, tap tempo, banków ani zapisu do EEPROM. Odbiór zmian z M5 został wycofany zgodnie z końcową decyzją użytkownika. Przełączenie bezpośrednio na M5 nie aktualizuje wskazania switcha.

## Ograniczenia ustaleń

Nowy szkic sprawdzono kompilatorem AVR i w symulatorze, nie na oryginalnym sprzęcie. Wartości wlutowanych rezystorów, fizyczny pinout LED i polaryzacja gniazda DC wymagają sprawdzenia egzemplarza. Materiały zawierają rozbieżność 10/18 nóżek wyświetlacza; szczegóły w instrukcji połączeń. Źródła protokołu i parametrów Nano są podlinkowane w README.
