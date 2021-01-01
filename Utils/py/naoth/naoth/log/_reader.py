import os as _os
import io as _io
import struct as _struct
from ._parser import Parser


class Reader:
    class Frame:
        def __init__(self, reader, number):
            self.reader = reader
            self.number = number
            self.messages = {}

        def __getitem__(self, name):
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

    def __init__(self, path, parser=Parser(), filter=lambda x: x):
        self.__open(path)
        self.log_file = self.log.name
        self.parser = parser
        self.filter = filter

        self.frames = []
        self.names = []

        self.scanPosition = 0
        # get the first frame
        self.__scanFrame()

    def __open(self, path):
        if isinstance(path, _io.IOBase):
            self.size = _os.stat(path.name).st_size
            if path.mode != "rb":
                raise Exception("File opened with the wrong mode! is {}, should be 'rb'".format(path.mode))
            self.log = path
        else:
            statinfo = _os.stat(path)
            self.size = statinfo.st_size
            self.log = open(path, "rb")

    def __getstate__(self):
        """ This is called before pickling. """
        state = self.__dict__.copy()
        del state['log']
        # NOTE: currently its not possible to preserve the filter
        del state['filter']
        return state

    def __setstate__(self, state):
        """ This is called while unpickling. """
        self.__dict__.update(state)
        self.__open(self.log_file)
        # NOTE: currently its not possible to restore the filter
        self.filter = lambda x: x

    def __scan(self):
        try:
            while True:
                self.__scanFrame()
                # todo: make it more efficient
                # if name not in self.names:
                #  self.names.append(name)

        except StopIteration as ex:
            print(ex)

    def __scanFrame(self):
        if self.scanPosition == -1:
            return False
        else:
            self.log.seek(self.scanPosition)

        # if self.idx > 30*100:
        #  return False

        last = (self.scanPosition * 100) / self.size

        try:
            currentFrame = None
            while True:
                # read message header
                frameNumber = self.readLong()

                if currentFrame is None:
                    currentFrame = Reader.Frame(self, frameNumber)

                if currentFrame.number != frameNumber:
                    self.log.seek(-4, 1)
                    self.scanPosition = self.log.tell()
                    self.frames.append(currentFrame)
                    break

                name = self.readString()
                message_size = self.readLong()

                currentFrame.messages[name] = (self.log.tell(), message_size, None)
                # skip message data
                self.log.seek(message_size, 1)

                # todo: make it more efficient
                if name not in self.names:
                    self.names.append(name)

            new = (self.scanPosition * 100) / self.size
            # if new > last:
            # print new

            # if new > 3:
            #  raise StopIteration

            return True

        except StopIteration as ex:
            self.scanPosition = -1
            return False

    def readBytes(self, bytes):
        if self.log is None:
            raise StopIteration

        if bytes == 0:
            return b''

        data = self.log.read(bytes)
        if len(data) != bytes:
            raise StopIteration

        return data

    def readLong(self):
        return _struct.unpack("=l", self.readBytes(4))[0]

    def readChar(self):
        return _struct.unpack("=c", self.readBytes(1))[0]

    # read a '\0' terminated string
    def readString(self):
        str = ''
        c = self.readChar().decode("utf-8")
        while c != "\0":
            str += c
            c = self.readChar().decode("utf-8")
        return str

    def __getitem__(self, i):
        while i + 1 >= len(self.frames):
            if not self.__scanFrame():
                break

        if i + 1 < len(self.frames):
            return self.filter(self.frames[i])
        else:
            raise StopIteration
