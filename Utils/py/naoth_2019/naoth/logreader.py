import inspect
import logging
import struct

# protobuf
from naoth.protobuf import CommonTypes_pb2, Framework_Representations_pb2, Messages_pb2, Representations_pb2, \
    TeamMessage_pb2, AudioData_pb2

logger = logging.getLogger(__name__)


class LogParser:
    """
    Parses raw log file frame member bytes using protobuf.
    If a name of a frame member does not match a class defined in protobuf,
    the corresponding class name needs to be defined using the register function.

    Ex: parser.register("ImageTop", "Image") where Image is the corresponding protobuf class.
    """

    def __init__(self):
        self.protobuf_classes = {}

        # load protobuf classes
        for module in [CommonTypes_pb2, Framework_Representations_pb2, Messages_pb2, Representations_pb2,
                       TeamMessage_pb2, AudioData_pb2]:
            self.protobuf_classes.update(LogParser._load_classes_from(module))

        self.name_to_type = {}

    @staticmethod
    def _load_classes_from(module):
        return {cls[0]: cls[1] for cls in inspect.getmembers(module, inspect.isclass)}

    def register(self, name: str, _type: str):
        """
        Register name corresponding protobuf type.
        :param name: frame member name
        :param _type: class in protobuf
        """
        if _type not in self.protobuf_classes:
            raise ValueError(f'Class {_type} is not part of the protobuf defined types.')

        self.name_to_type[name] = _type

    def parse(self, name, data):
        """
        Parse frame member bytes using protobuf.
        :param name: of frame member
        :param data: payload bytes
        :returns member class
        """
        if name in self.name_to_type:
            name = self.name_to_type[name]
        # protobuf
        if name in self.protobuf_classes:
            message = self.protobuf_classes[name]()
            message.ParseFromString(data)
            return message
        else:
            raise ValueError(f'Class {name} is not part of the protobuf defined types.')


class LogScanner:
    """
    Scans log file for frame member positions
    """

    def __init__(self, log_file):
        self.log_file = log_file
        self._scan_position = 0

    def _read_bytes(self, n):
        """
        Read n bytes from the log file. If there are not enough bytes left an EOFError is raised.
        :param n: number of bytes to read
        :returns byte string of size n
        """
        if n == 0:
            return b''
        data = self.log_file.read(n)
        if len(data) != n:
            raise EOFError()
        return data

    def _read_long(self):
        return struct.unpack('=l', self._read_bytes(4))[0]

    def _read_char(self):
        char = struct.unpack('=c', self._read_bytes(1))[0]
        return char.decode("utf-8")

    def _read_string(self):
        """
        Read a '\0' terminated string from the log file.
        """
        chars = []
        c = self._read_char()
        while c != '\0':
            chars.append(c)
            c = self._read_char()
        return ''.join(chars)

    @staticmethod
    def frame_member_size(name, payload_size):
        """
        Calculates the byte string size of a frame member in the log file
        :param name: of member
        :param payload_size
        :returns sizeOf(frame number) + sizeOf(name + '\0') + sizeOf(payload size) + payload size
        """
        return struct.calcsize('=l') + (len(name) + 1) * struct.calcsize('=c') + struct.calcsize('=l') + payload_size

    def scan_frame_member(self):
        """
        Reads name and payload positions of the next frame member
        :returns frame member start position, corresponding frame number, payload position (position, size)
        """
        frame_member_start = self._scan_position
        self.log_file.seek(frame_member_start)

        # read message header
        frame_number = self._read_long()

        name = self._read_string()
        message_size = self._read_long()
        data_pos = (self.log_file.tell(), message_size)

        # calculate next frame member position (skipping payload of previous one)
        self._scan_position = self.log_file.tell() + message_size

        return frame_member_start, frame_number, name, data_pos

    def read_data(self, position, size):
        """
        Reads data bytes from the log file.
        :param position: starting position of the data in the log file
        :param size: size of the data
        :returns data bytes
        """
        self.log_file.seek(position)
        data = self._read_bytes(size)
        return data


class FrameParseException(Exception):
    pass


class Frame:
    def __init__(self, frame_start, frame_number, scanner, parser):
        self.scanner = scanner
        self.parser = parser

        # frame position in the log file
        self.start = frame_start
        self.size = 0

        self.number = frame_number
        self.members = {}

    def add_member(self, name, position, size):
        """
        Add a member to the frame
        :param name: of member
        :param position: of payload data in the log file
        :param size: of payload data in the log file
        """
        if name in self.members:
            raise ValueError(f'Frame already contains member {name}.')
        self.members[name] = (position, size)

        self.size += LogScanner.frame_member_size(name, size)

    def __contains__(self, name):
        """
        Check if member of frame
        :param name: of member
        :returns True if member of frame
        """
        return name in self.members

    def __getitem__(self, name):
        """
        Get message of a frame member
        :param name: of frame member
        :returns message
        """
        return self.payload(name)

    def payload(self, name):
        """
        Parse payload of a frame member.
        :param name: of frame member
        :returns parsed message
        """
        position, size = self.members[name]

        # parse payload
        try:
            payload = self.scanner.read_data(position, size)
        except EOFError:
            raise FrameParseException(f'Failed reading data of {name}, file ended unexpectedly.')

        message = self.parser.parse(name, payload)

        return message

    def names(self):
        """
        :returns names of frame members part of the frame
        """
        return self.members.keys()

    def __len__(self):
        """
        :returns size of the frame in the log file
        """
        return self.size

    @staticmethod
    def _bytes_from_char(char):
        return struct.pack('=c', bytes(char, 'utf-8'))

    @staticmethod
    def _bytes_from_str(string):
        return b''.join(Frame._bytes_from_char(char) for char in string) + Frame._bytes_from_char('\0')

    @staticmethod
    def _long_from_int(integer):
        return struct.pack('=l', integer)

    def __bytes__(self):
        """
        :returns bytes representation of frame ready to write to log file
        """
        frame_number = self._long_from_int(self.number)
        _bytes = b''.join(
            frame_number
            + self._bytes_from_str(name)
            + self._long_from_int(size)
            + self.scanner.read_data(position, size)
            for name, (position, size) in self.members.items()
        )
        assert self.size == len(_bytes)
        return _bytes


class LogReader:
    def __init__(self, path, parser=LogParser()):
        self.log_file = open(path, 'rb')
        self.scanner = LogScanner(self.log_file)
        self.parser = parser

        self.frames = []

    def __enter__(self):
        return self

    def __exit__(self, _type, value, traceback):
        self.close()

    def close(self):
        """
        Close underlying file handle
        """
        self.log_file.close()

    def read_from(self, index):
        """
        Generator yielding log file frames starting from frame i.
        """
        # yield frames already read
        while index < len(self.frames):
            yield self.frames[index]
            index += 1

        # read more frames from the log file
        frame = None
        while True:
            # read next frame member
            try:
                frame_member_start, current_frame_number, name, payload_pos = self.scanner.scan_frame_member()
            except EOFError:
                break

            # check if frame member is part of current frame or create new one
            if frame is None:
                frame = Frame(frame_member_start, current_frame_number, self.scanner, self.parser)
            elif current_frame_number != frame.number:
                # finished reading frame
                self.frames.append(frame)

                # only yield frame if it comes after the ith frame
                if index < len(self.frames):
                    yield frame

                # create new frame
                frame = Frame(frame_member_start, current_frame_number, self.scanner, self.parser)

            # add frame member
            position, size = payload_pos
            frame.add_member(name, position, size)

        # End of file is reached, yield the last log file frame
        if frame is not None and frame.members:
            self.frames.append(frame)

            # only yield frame if it comes after the ith frame
            if index < len(self.frames):
                yield frame

    def read(self):
        """
        Generator yielding frames from log file.
        """
        return self.read_from(0)

    def __getitem__(self, i):
        """
        Get ith frame from log file.
        """
        if i >= len(self.frames):
            try:
                next(self.read_from(i))
            except StopIteration:
                return None
        return self.frames[i]


if __name__ == "__main__":
    import argparse

    arg_parser = argparse.ArgumentParser(description='Log file parsing example.')
    arg_parser.add_argument('logfile', help='naoth log file')
    args = arg_parser.parse_args()

    log_parser = LogParser()
    # register corresponding protobuf type of the top camera
    log_parser.register('ScanLineEdgelPerceptTop', 'ScanLineEdgelPercept')

    with LogReader(args.logfile, parser=log_parser) as log_reader:
        _frame = log_reader[10]
        print('10th frame has image?', 'Image' in _frame)
        print('Members of the 10th frame:', list(_frame.names()))
        print()

        print('Print all frame infos')
        for _frame in log_reader.read():
            print(_frame['FrameInfo'], end='')
        print()

        print('Number of edgels of 3 frames after the 10th frame:')
        for i, _frame in enumerate(log_reader.read_from(11)):
            if 'ScanLineEdgelPerceptTop' in _frame:
                edgels = _frame['ScanLineEdgelPerceptTop'].edgels
                print('ScanLineEdgelPerceptTop of frame', _frame.number, 'contains', len(edgels), 'edgels')
            if i == 2:
                break
