import sys
import mainscript

cache = None
if __name__ == "__main__":
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