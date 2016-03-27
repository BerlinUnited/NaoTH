import struct, os

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

  class Frame:
    def __init__(self, reader, number):
      self.reader = reader
      self.number = number
      self.messages = {}
    
    def __getitem__(self,name):
      return self.getMessage(name)
    
    def getMessage(self, name):
      position, size, message = self.messages[name]
      
      if message is not None:
        return message
      
      self.reader.log.seek(position)
      data = self.reader.readBytes(size)
      
      message = self.reader.parser.parse(name, data)
      self.messages[name] = (position, size, message)
      return message

      
  class Iterator:
    def __init__(self, reader):
      self.idx = -1
      self.reader = reader
      
    def __iter__(self):
      return self
    
    def next(self):
      self.idx += 1
      return self.reader[self.idx]
      
      
  def __init__(self, path, parser=Parser(), filter = lambda x: x):
    statinfo = os.stat(path)
    self.size = statinfo.st_size
    self.log = open(path, "rb")
    self.parser = parser
    self.filter = filter
    
    self.frames = []
    self.names = []
    
    self.scanPosition = 0
    # get the first frame
    self.__scanFrame()
    
    
  def __iter__(self):
    return LogReader.Iterator(self)
    
  def __scan(self):

    try:
      while True:
        self.__scanFrame()
        # todo: make it more efficient
        #if name not in self.names:
        #  self.names.append(name)
        
    except StopIteration as ex:
      print ex
      
  def __scanFrame(self):
    if self.scanPosition == -1:
      return False
    else:
      self.log.seek(self.scanPosition)
  
    #if self.idx > 30*100:
    #  return False
  
    last = (self.scanPosition*100) / self.size
  
    try:
      currentFrame = None
      while True:
        # read message header
        frameNumber = self.readLong()
        
        if currentFrame is None:
          currentFrame = LogReader.Frame(self, frameNumber)
          
        if currentFrame.number != frameNumber:
          self.log.seek(-4,1)
          self.scanPosition = self.log.tell()
          self.frames.append(currentFrame)
          break
          
        name = self.readString()
        message_size = self.readLong()
        
        currentFrame.messages[name] = (self.log.tell(), message_size, None)
        # skip message data
        self.log.seek(message_size,1)
        
        # todo: make it more efficient
        if name not in self.names:
          self.names.append(name)
      
      new = (self.scanPosition*100) / self.size
      if new > last:
        print new
      
      #if new > 3:
      #  raise StopIteration
        
      return True
        
    except StopIteration as ex:
      self.scanPosition = -1
      return False
      
  def readBytes(self, bytes):
    if self.log is None:
      raise StopIteration
    
    data = self.log.read(bytes)
    if data == '':
      raise StopIteration
    
    return data
    
  def readLong(self):
    return struct.unpack("=l", self.readBytes(4))[0]
    
  def readChar(self):
    return struct.unpack("=c", self.readBytes(1))[0]
    
  # read a '\0' terminated string
  def readString(self):
    str = ''
    c = self.readChar()
    while c != "\0":
      str += c
      c = self.readChar()
    return str
    
  def __getitem__(self, i):
    while i + 1 >= len(self.frames):
      if not self.__scanFrame():
        break
    
    if i + 1 < len(self.frames):
      return self.filter(self.frames[i])
    else:
      raise StopIteration
     
    
    
if __name__ == "__main__":

  myParser = Parser()
  myParser.register("CameraMatrixTop", "CameraMatrix")

  for i in LogReader("./cognition.log", myParser):
    print i.frameNumber, i.name
