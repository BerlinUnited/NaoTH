#!/bin/bash

### this is a set of practical scripts to perform a certain task, (un)comment and
# parameterize the scripts listed here.

# play start signal
aplay /usr/share/naoqi/wav/shortbip.wav

### BEGIN ###

# scripts/wep_wlan "SPL_A" "Nao?!" # SSID KEY
scripts/player 3 4 blue # playerNr teamNr teamColor

### END ##

# restart naoth
naoth restart


