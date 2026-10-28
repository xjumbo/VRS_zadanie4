# Zadanie 4 – Komunikácia MCU <=> PC cez USART2 a DMA

**Meno:** Dominik Ďurmek
**AIS ID:** 126716
**Predmet:** VRS – Vnorené riadiace systémy
**Cvičenie:** štvrtok 10:00

> Poznámka: Tento README súbor bol vygenerovaný pomocou AI (Claude od Anthropic)
> a upravený podľa môjho riešenia.

> Pri riešení zadania som využil AI asistenta (Claude) na vysvetlenie postupu
> a kontrolu kódu. Kód som písal a testoval sám.

## Čo program robí

Program beží na doske Nucleo (STM32F303K8) a komunikuje s počítačom
cez USART2. Príjem dát je riešený pomocou DMA a prerušení HT, TC a IDLE.

1. **Príjem dát** – DMA ukladá prijaté znaky do buffera (128 bajtov,
   normálny režim). Keď je v bufferi málo miesta, buffer sa vráti na začiatok,
   aby nedošlo k pretečeniu.
2. **Spracovanie reťazca** – platný reťazec začína znakom `#` a končí
   znakom `$`. Program spočíta malé a veľké písmená a výsledok pošle do PC,
   napr. `Valid string: Platn15uborZnakov, lower-case: 13, upper-case: 2`.
   Znaky pred `#` ignoruje. Ak do 35 znakov nepríde `$`, reťazec zahodí.
3. **Stav buffera** – každé 2 sekundy (0,5 Hz) program pošle, koľko je
   buffer zaplnený, napr.
   `Buffer capacity: 128 bytes, occupied memory: 19 bytes, load [in %]: 14.8%`.

## Kde je čo v kóde

- `Src/usart.c` – nastavenie DMA a USART, funkcia `USART2_CheckDmaReception`
  a `USART2_GetDmaOccupancy`
- `Src/main.c` – funkcia `proccesDmaData` a odosielanie správ vo `while` slučke
- `Src/stm32f3xx_it.c` – časovač SysTick (počíta milisekundy)

## Ako to vyskúšať

1. Nahrať program do dosky.
2. Otvoriť terminál (napr. PuTTY) na porte ST-Link, rýchlosť 115200,
   8 dátových bitov, bez parity, 1 stop bit.
3. Poslať napr. `#Platn15uborZnakov$` a pozrieť odpoveď.