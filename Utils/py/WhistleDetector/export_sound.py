#!/usr/bin/python

from naoth.LogReader import LogReader
from naoth.LogReader import Parser

  
def get_audio(frame):
  return frame["AudioData"]

if __name__ == "__main__":
  myParser = Parser()

  newFile = open("audio.raw", "wb")
  
  timestamp = 0
  for s in LogReader("./test_new_recorder.log", myParser, get_audio):
      if s.timestamp > timestamp:
        timestamp = s.timestamp
        #print (type(s.samples))
        print (timestamp)
        newFile.write(s.samples)

  newFile.close()