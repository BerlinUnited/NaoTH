#!/usr/bin/python

# imports
import naoqi
import argparse
import random

parser = argparse.ArgumentParser(description='Text-to-speech')
parser.add_argument('text', help='Text (string) that should be converted to speech.')

args = parser.parse_args()
print args

if args.text:
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
	file = "/tmp/text-to-speech_%04d.wav" % (random.random()*10000)
	tts.sayToFile(args.text, file)
	print "\nconverted '" + args.text + "' to speech: " + file + "\n"
