#!/usr/bin/python
import os, sys, getopt
import mainExportKicks

if __name__ == "__main__":

  rootDir = "D:\\Projects\\2016-web-video-player\\log"
  for root, dirs, files in os.walk(rootDir):
    for file in files:
        if file.endswith(".log"):
          logFile = os.path.join(root, file)
          outFile = os.path.join(root, "labels.json")
          print(logFile)
          print(outFile)
          cache = mainExportKicks.init(logFile)
          mainExportKicks.run(cache, outFile)