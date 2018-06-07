#!/usr/bin/python

# imports
import naoqi

# init text-to-speech module
tts = naoqi.ALProxy("ALTextToSpeech", "localhost", 9559)

# print some information
lang = tts.getAvailableLanguages()
print "Available languages: ", str(lang)
print "\tactive: ", tts.getLanguage()

voices = tts.getAvailableVoices()
print "Available voices: ", voices
print "\tactive: ", tts.getVoice()

# say something (to file)
#tts.say("ERROR: usb wrong filesystem")

text = ["Der Akku wird geladen",
        "Es konnte kein Kontakt zum Ladegerät hergestellt werden",
        "Ich bin hingefallen",
        "Ich bin bereit",
        "Es sind zu viele Roboter auf dem Feld",
        "Ich muss meine Knochen schonen",
        "Ich muss meinen Akku laden",
        "Los gehts",
        "Ich habe gewonnen",
        "Oh nein"]

for idx, t in enumerate(text):
    file = "/tmp/bdr"+str(idx)+".wav"
    tts.syToFile(t, file)
