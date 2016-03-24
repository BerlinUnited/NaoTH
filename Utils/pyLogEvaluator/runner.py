#!/usr/bin/python

import os, sys, getopt
import importlib

def parseArguments(argv):
  scriptPath = None
  
  try:
    opts, args = getopt.getopt(argv,"hs:",["script="])
  except getopt.GetoptError:
    print 'ExportTopImages.py -s <file to run>'
    sys.exit(2)
    
  for opt, arg in opts:
    if opt == '-h':
      print 'ExportTopImages.py -s <file to run>'
      sys.exit()
    elif opt in ("-s", "--script"):
      scriptPath = arg
      
  return scriptPath


cache = None

if __name__ == "__main__":

    module_name = parseArguments(sys.argv[1:])
    if module_name is None:
      print "[ERROR] Expect a name for a module to be loaded"
      sys.exit(-1)
    else:
      mainscript = importlib.import_module(module_name)
    
    while True:
        if not cache:
          cache = mainscript.init()
        
        try:
          mainscript.run(cache)
        except Exception as ex:
          print ex
        
        print "Press enter to re-run the script, CTRL-C to exit"
        sys.stdin.readline()
        
        try:
          reload(mainscript)
        except Exception as ex:
          print ex