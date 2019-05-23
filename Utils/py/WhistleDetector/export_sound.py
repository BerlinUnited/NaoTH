#!/usr/bin/python

from naoth.LogReader import LogReader
from naoth.LogReader import Parser

  
def get_audio(frame):
  if "AudioData" in frame.messages:
    return frame["AudioData"]
  else:
    return None

if __name__ == "__main__":
  myParser = Parser()

  newFile = open("audio.raw", "wb")
  
  timestamp = 0
  for s in LogReader("./test_new_recorder.log", myParser, get_audio):
      if s is not None and s.timestamp > timestamp:
        timestamp = s.timestamp
        #print (type(s.samples))
        print (timestamp)
        newFile.write(s.samples)

  newFile.close()