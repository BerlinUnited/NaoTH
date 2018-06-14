from naoth.LogReader import LogReader
from naoth.LogReader import Parser

def get_whistle_percept(frame):
  percept = None
  try:
    percept = frame["WhistlePercept"]
  except:
    pass

  if percept:
    return [frame.number, percept]
  else:
    return None

if __name__ == "__main__":
  myParser = Parser()

  for msg in LogReader("L:/game.log", myParser, get_whistle_percept):
    if msg:
      print(msg)

