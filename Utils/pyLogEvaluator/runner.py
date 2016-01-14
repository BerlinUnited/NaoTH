import sys
import mainscript

part1Cache = None
if __name__ == "__main__":
    while True:
        if not part1Cache:
            part1Cache = mainscript.part1()
        
        try:
          mainscript.part2(part1Cache)
        except Exception as ex:
          print ex
        
        print "Press enter to re-run the script, CTRL-C to exit"
        sys.stdin.readline()
        
        try:
          reload(mainscript)
        except Exception as ex:
          print ex