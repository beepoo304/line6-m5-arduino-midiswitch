# Połączenia MIDI SWITCH

Schemat zachowuje przypisanie D2–D12 z pliku `READY CODE FOR LINE6M5 UP_DOWN .ino` i starego schematu Nano. Nie dodaje wejścia MIDI ani nowych przycisków. Jest schematem funkcjonalnych połączeń, a nie rysunkiem mechanicznego rozmieszczenia nóżek.

## Lista części

| Element | Liczba | Uwagi |
|---|---:|---|
| Klasyczne Arduino Nano ATmega328P, 5 V / 16 MHz | 1 | Dotychczasowa płytka |
| Dwucyfrowy wyświetlacz 7-segmentowy, wspólna anoda | 1 | Lub dwie cyfry z połączonymi odpowiadającymi segmentami |
| Footswitch chwilowy NO | 2 | Zwierany tylko podczas naciśnięcia; nie zatrzaskowy |
| Rezystor 1,5 kΩ, 0,25 W | 7 | R1–R7: proponowany zachowawczy dobór dla bezpośrednich anod |
| Rezystor 220 Ω, 0,25 W | 2 | R8 i R9: MIDI OUT przy 5 V |
| Gniazdo DIN 5-pin / 180° | 1 | MIDI OUT |
| Gniazdo DC, obudowa, przewody, płytka montażowa | wg montażu | Jak w oryginalnym urządzeniu |
| Zasilacz 9 V DC | 1 | Do VIN; na stole można użyć USB |

Wartości rezystorów w istniejącym egzemplarzu nie zostały zmierzone. Nie traktuj wartości 1,5 kΩ jako odczytu ze zdjęć ani polecenia wymiany elementów w sprawnym urządzeniu.

## Wyświetlacz

| Nano | Przez | Funkcja LED | Nóżka wyłącznie według lokalnej karty LD-D056XXX-C |
|---|---|---|---:|
| D2 | R1 | A | 10 |
| D3 | R2 | B | 9 |
| D4 | R3 | C | 1 |
| D5 | R4 | D | 4 |
| D6 | R5 | E | 3 |
| D7 | R6 | F | 6 |
| D8 | R7 | G | 5 |
| D9 | bezpośrednio | CA1, lewa cyfra | 8 |
| D10 | bezpośrednio | CA2, prawa cyfra | 7 |
| nie podłączaj | — | DP, kropka | 2 |

**Numery fizycznych nóżek są warunkowe.** Karta w archiwum przedstawia obudowę 10-nóżkową. Komentarz w starym kodzie wspomina po dziewięć nóżek na górze i dole. To niespójność materiałów; zdjęcia gotowej obudowy nie rozstrzygają typu wlutowanego wyświetlacza. Dla innego modelu zachowaj funkcje A–G, CA1 i CA2, ale użyj jego własnego pinoutu. Nie podłączaj według numerów z tabeli bez sprawdzenia modelu.

Segmenty oznacza się: A górny, B prawy górny, C prawy dolny, D dolny, E lewy dolny, F lewy górny, G środkowy. W wersji ze wspólną anodą aktywna cyfra otrzymuje HIGH, a świecące segmenty LOW. Kod gasi obie cyfry przed zmianą segmentów.

### Rezystory i jasność

Przy bezpośrednim sterowaniu wspólna anoda przenosi sumę prądów wszystkich świecących segmentów danej cyfry. Dlatego mały rezystor dobierany tylko do jednego segmentu może nadmiernie obciążyć D9/D10. Dokumentacja Nano podaje limit 20 mA na pin I/O.

Proponowane 1,5 kΩ daje przy 5 V i spadku LED 1,9 V około 2,1 mA na segment, czyli około 14,5 mA dla siedmiu segmentów w chwili świecenia cyfry. Średni prąd jest mniejszy przez multipleksowanie. Dla zielonych LED o wyższym spadku prąd i jasność będą mniejsze. To obliczenie projektowe, nie pomiar Twojego urządzenia. Jeżeli istniejący układ ma tranzystory albo inne sterowanie cyframi, wymaga osobnego sprawdzenia; przedstawiony schemat zakłada anody bez tranzystorów, jak w starym opisie pinów.

## Footswitche

- D11 → pierwszy styk DOWN; drugi styk → GND.
- D12 → pierwszy styk UP; drugi styk → GND.
- Dla przełącznika z trzema wyprowadzeniami użyj COM i NO, potwierdzonych miernikiem.
- `INPUT_PULLUP` utrzymuje spoczynkowo HIGH. Wciśnięcie daje LOW. Nie łącz przycisków z 9 V.

## MIDI OUT

| Nano | Element | Styk DIN |
|---|---|---|
| 5 V | R8 = 220 Ω | 4 |
| D1 / TX | R9 = 220 Ω | 5 |
| GND | przewód | 2 |
| brak | nie podłączaj | 1 i 3 |

To prosty, niebuforowany nadajnik UART 5 V odpowiadający koncepcji oryginału. Nie jest to deklaracja certyfikacji zgodności elektrycznej produktu. Numery DIN odnoszą się do oznaczeń styków: widok od strony lutowania jest lustrzanym odbiciem widoku od strony wtyku. Rysunek celowo nie zgaduje układu wyprowadzeń konkretnego gniazda panelowego.

Przewód MIDI: **MIDI OUT switcha → MIDI IN M5**. M5 MIDI OUT pozostaje wolne. Parametry danych: 31250 baud, 8 bitów, bez parzystości, 1 bit stopu. Nie używaj `Serial.print()` do debugowania, bo ten sam port przenosi MIDI.

## Zasilanie

- Dodatni biegun 9 V DC → VIN Nano.
- Ujemny biegun → GND Nano.
- 9 V nie wolno podać na pin 5 V.
- Na stole możesz zasilać Nano przez USB. Do pierwszego uruchomienia wybierz jedno źródło zasilania.
- Polaryzacja centrum i tulei gniazda DC zależy od zastosowanego zasilacza i połączeń; oznacz ją dopiero po sprawdzeniu miernikiem. Fotografia nie wystarcza do jej ustalenia.

## Próba na urządzeniu

1. Bez zasilania porównaj piny i sprawdź brak zwarcia 5 V–GND oraz VIN–GND.
2. Odłącz M5 na czas wgrywania szkicu. Po uruchomieniu sprawdź wyświetlenie 01.
3. Włącz M5 przed restartem switcha, ustaw CH1 i podłącz MIDI. Po starcie switch powinien wybrać 01 i włączyć efekt.
4. Sprawdź 01 → 02 → 03, DOWN oraz przejścia 24 ↔ 01.
5. Sprawdź krótkie naciśnięcia, przytrzymanie i zwolnienie.
6. Naciśnij oba przyciski razem: preset nie powinien się zmienić, M5 powinien wejść w bypass. Numer miga. Po zwolnieniu obu wybierz kolejny preset: efekt się włącza.
7. Sprawdź czytelność cyfr 08, 18 i 24, brak zamiany cyfr oraz jasność. W razie zamiany stron porównaj D9/D10 z rzeczywistymi anodami.

Wynik tej próby pozostaje do wykonania na fizycznym urządzeniu; symulator nie sprawdza połączeń, prądu LED ani zachowania samego M5.
