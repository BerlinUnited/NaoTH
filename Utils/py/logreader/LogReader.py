# protobuf
from CommonTypes_pb2 import *
from Framework_Representations_pb2 import *
from Messages_pb2 import *
from Representations_pb2 import *
from google.protobuf import text_format

import mmap

import os
import struct


class LogParser:
    def __init__(self):
        self.nameToType = {}

    def register(self, name, type):
        self.nameToType[name] = type

    def parse(self, name, data):
        if name in self.nameToType:
            name = self.nameToType[name]

        # protobuf
        if name in globals():
            message = globals()[name]()
            message.ParseFromString(data)
            return message
        else:
            return None

class LogScanner:
    def __init__(self, logFile):
        self.logFile = logFile
        self.scanPosition = 0

    def __readBytes(self, N):
        if N == 0:
            return ''
        data = self.logFile.read(N)

        if len(data) != N:
            raise EOFError()

        return data

    def __readLong(self):
        return struct.unpack("=l", self.__readBytes(4))[0]

    def __readChar(self):
        c = struct.unpack("=c", self.__readBytes(1))[0]
        return c.decode("utf-8")

    # read a '\0' terminated string
    def __readString(self):
      string = ''
      c = self.__readChar()
      while c != "\0":
        string += c
        c = self.__readChar()
      return string

    def scanFrameMember(self):
        self.logFile.seek(self.scanPosition)

        # read message header
        frameNumber = self.__readLong()

        name = self.__readString()
        message_size = self.__readLong()
        # TODO: create a more verbose representation
        dataPos = (self.logFile.tell(), message_size, None)

        # skip data
        self.scanPosition = self.logFile.tell() + message_size

        return frameNumber, name, dataPos

    def scanFrame(self, position, size):
        self.logFile.seek(position)
        data = self.__readBytes(size)

        return data

class Frame:
  def __init__(self, frameNumber, scanner, parser):
    self.scanner = scanner
    self.parser = parser

    self.number = frameNumber
    self.messages = {}

  def __getitem__(self,name):
    return self.getMessage(name)

  def getMessage(self, name):
    position, size, message = self.messages[name]

    if not message:
        # parse message
        data = self.scanner.scanFrame(position, size)
        message = self.parser.parse(name, data)
        # cache message
        self.messages[name] = (position, size, message)

    return message

class LogReader:
    def __init__(self, path, parser=LogParser(), filter=lambda x: x):
        self.fileptr = open(path, "rb")

        # TODO: test this on all platforms
        if os.name == 'nt':
          # WINDOWS needs it to be:
          self.logFile = mmap.mmap(self.fileptr.fileno(), 0, access=mmap.ACCESS_READ)
        else:
          self.logFile = mmap.mmap(self.fileptr.fileno(), 0, prot=mmap.PROT_READ)

        self.scanner = LogScanner(self.logFile)
        self.parser = parser

        self.frames = []

        self.filter = filter


    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        # TODO: error handling
        self.close()

    def close(self):
        self.logFile.close()
        self.fileptr.close()

    def read(self):
        frame = None

        i=0
        while 1:
            # TODO: cleanup!
            if i+1 <= len(self.frames):
                yield self.frames[i]
            else:
                try:
                    frameNumber, name, dataPos = self.scanner.scanFrameMember()
                except EOFError as eof:
                    return

                if not frame:
                    frame = Frame(frameNumber, self.scanner, self.parser)
                elif frameNumber != frame.number:
                    self.frames.append(frame)
                    yield self.filter(frame)
                    frame = Frame(frameNumber, self.scanner, self.parser)
                else:
                    frame.messages[name] = dataPos

            i+=1

    def readFrame(self, i):
        if len(self.frames) < i+1:
            for frame in self.read():
                if len(self.frames) == i+1:
                    return self.frames[i]
            else:
                return None
        else:
            return self.filter(self.frames[i])

    def __getitem__(self, i):
        return self.readFrame(i)

if __name__ == "__main__":
    with LogReader("./cognition.log") as log:
        print(log[100]["FrameInfo"])


        for frame in log.read():
            print(frame["FrameInfo"])
