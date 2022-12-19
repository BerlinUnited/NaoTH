#!/usr/bin/python

from naoth.log import Reader as LogReader
from naoth.log import Parser


if __name__ == "__main__":

    newFile = open("audio.raw", "wb")

    timestamp = 0
    for frame in LogReader("./cognition.log"):

        if "AudioData" in frame.get_names():
            s = frame["AudioData"]
            
            # export only new audio frames
            if s.timestamp > timestamp:
                timestamp = s.timestamp
                print(timestamp)
                
                newFile.write(s.samples)
    
    newFile.close()
