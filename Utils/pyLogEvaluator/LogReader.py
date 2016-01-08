import struct

# protobuf
from CommonTypes_pb2 import *
from Framework_Representations_pb2 import *
from Messages_pb2 import *
from Representations_pb2 import *
from google.protobuf import text_format

class Parser:
  def __init__(self):
    self.nameToType = {}
    
  def register(self, name, type):
    self.nameToType[name] = type
    
  def parse(self, name, data):
    message = None
    
    if name in self.nameToType:
      name = self.nameToType[name]
    
    if name in globals():
      message = globals()[name]()
    
    if message is not None:
      message.ParseFromString(data)
    
    return message
      
      

class LogReader:
  def __init__(self, path, parser=Parser()):
    self.log = open(path, "rb")
    self.parser = parser
    
    self.frameNumber = None
    self.name = None
    self.message = None
    self.message_size = 0
    self.message_read = True
    
  def __iter__(self):
    return self
    
  def getMessage(self):
    if self.message is None:
      # read the data
      data = self.log.read(self.message_size)
      self.message_read = True
      
      # find the message and parse
      self.message = self.parser.parse(self.name, data)
    return self.message
  
  def readBytes(self, bytes):
    if self.log is None:
      raise StopIteration
    
    data = self.log.read(bytes)
    if data == '':
      raise StopIteration
    
    return data
    
  def readLong(self):
    return struct.unpack("l", self.readBytes(4))[0]
    
  def readChar(self):
    return struct.unpack("c", self.readBytes(1))[0]
    
  # read a '\0' terminated string
  def readString(self):
    str = ''
    c = self.readChar()
    while c != "\0":
      str += c
      c = self.readChar()
    return str
    
  def next(self):
    try:
      if self.log is None:
        raise StopIteration
        
      # skip the data if it was not retrieved in the last run
      if not self.message_read:
        self.log.seek(self.message_size,1)
      
      # read the header
      self.frameNumber = self.readLong()
      self.name = self.readString()
      self.message_size = self.readLong()
      
      # reset the message data
      self.message_read = False
      self.message = None

      return self
      
    except EOFError as ex:
      raise StopIteration
    
    
if __name__ == "__main__":

  myParser = Parser()
  myParser.register("CameraMatrixTop", "CameraMatrix")

  for i in LogReader("./cognition.log", myParser):
    print i.frameNumber, i.name
