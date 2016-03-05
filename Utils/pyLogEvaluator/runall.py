#!/usr/bin/python
import os, sys, getopt
import mainExportKicks

if __name__ == "__main__":

  rootDir = "D:\\AppData\\xampp\\htdocs\\VideoLogLabeling\\log\\20150426-Game-NaoDevils"
  
  #rootDir = "D:\\AppData\\xampp\\htdocs\\VideoLogLabeling\\log\\2015-07-21-competition-day3-NaoDevils\\half2"
  
  for root, dirs, files in os.walk(rootDir):
    for file in files:
        if file.endswith(".log"):
          logFile = os.path.join(root, file)
          outFile = os.path.join(root, "labels.json")
          print(logFile)
          print(outFile)
          cache = mainExportKicks.init(logFile)
          mainExportKicks.run(cache, outFile)