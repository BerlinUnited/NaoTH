# Python scripts for the nao robot

This directory contains python scripts, which can be directly executed on the nao.

Most of the scripts needs some additional naoqi modules to be enabled. Therefore, the
`/etc/naoqi/autoload.ini` must be edited and the necessary modules added. Afterwards naoqi must be restarted: `nao restart`

Some further infos can be found here:  
https://gitlab.informatik.hu-berlin.de/berlinunited/NaoTH-2018/wikis/python_nao

## Sonar test script
Prints out all sonar sensors read from ALMemory.

Script: `sonars.py`  
Modules: `pythonbridge`, `sensors`

## Text to speech generator
Creates a sound file in the `/tmp/text-to-speech_XXXX.wav` directory with the given text. 

Script: `text-to-speach.py`  
Modules: `pythonbridge`, ??? **TODO** !!!
