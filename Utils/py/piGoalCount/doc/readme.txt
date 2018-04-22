Raspberry Pi Zugang:
--------------------

user: pi
pwd: raspberry

name des PI im Netzwerk: RASP3-1

Einloggen per ssh:

ssh pi@RASP3-1

NOTE: wenn das nicht geht, dann muss man im Router die ip Adresse des PI finden.


Anschliessen:
--------------------

1. beim PI geht der hintere kleine Deckel per Hand ab. (Wenn man etwas aufschrauben muss, dann macht etwas falsch!)
2. dort sieht man direkt die PINS (Pin Belegung ist im Bild RP2_Pinout.png)
3. +5V anschliessen
4. GND anschliessen
5. Tore anschliessen an: "GPIO 24" und "GPIO 23"
Achtung: Nicht verwechseln: Pinnummer und GPIO Nummer sind zwei verschiedene Sachen! 
Z.B. "GPIO 24" hat die Pinnummer 18, und "GPIO 23" hat die Pinnummer 16.

6. script in den homedirectory von PI kopieren + starten. 
Jedes Mal wenn die Schranke aktiviert wird, müsste eine Ausgabe kommen.