import inspect
import logging
import struct

# protobuf
from naoth.messages import CommonTypes_pb2, Framework_Representations_pb2, Messages_pb2, Representations_pb2, \
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

    def set_scan_position(self, frame_start_position):
        """
        Set the position of the frame scanner. BEWARE: Must be where a frame member starts
        :param frame_start_position: position in log file where a frame member starts
        """
        self._scan_position = frame_start_position

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
        if size == 0:
            return b''

        self.log_file.seek(position)
        data = self._read_bytes(size)
        return data


class FrameParseException(Exception):
    pass


class Frame:
    def __init__(self, frame_start, frame_number, scanner, parser, cache=False):
        """
        NaoTH log file frame
        :param frame_start: in log file
        :param frame_number
        :param scanner: LogScanner of current log file
        :param parser: LogParser of current log file
        :param cache: if True cache already parsed members, may increase performance but affects memory usage
        """
        self.scanner = scanner
        self.parser = parser

        # frame position in the log file
        self.start = frame_start
        self.size = 0

        self.number = frame_number
        # members of this frame, contains either (position, size) tuple of the member data in the log file
        # or already parsed protobuf classes
        self.members = {}

        self.cache = cache

    def add_member(self, name, position, size):
        """
        Add a member to the frame
        :param name: of member
        :param position: of payload data in the log file
        :param size: of payload data in the log file
        """
        if name in self.members:
            raise ValueError(f'Frame already contains member {name}.')

        self.members[name] = position, size
        self.size += LogScanner.frame_member_size(name, size)

    def add_message_member(self, name, member):
        """
        Add a protobuf message as member to the frame
        :param name: of member
        :param member: protobuf message
        """
        self.size += LogScanner.frame_member_size(name, len(member.SerializeToString()))
        # add new member
        self.members[name] = member

    def remove(self, name):
        """
        Remove a member from this frame
        :param name: of member
        """
        member = self.members.pop(name)
        if isinstance(member, tuple):
            position, size = member
            self.size -= LogScanner.frame_member_size(name, size)
        else:
            self.size -= LogScanner.frame_member_size(name, len(member.SerializeToString()))

    def replace_message_member(self, name, member):
        """
        Replace a member of this frame with a protobuf message object
        :param name: of member
        :param member: protobuf message
        """
        if name not in self.members:
            raise ValueError(f'Frame does not contain member {name}.')

        # remove old member
        self.remove(name)

        self.add_message_member(name, member)

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
        member = self.members[name]
        if isinstance(member, tuple):
            # member is not parsed yet, get payload from log file frame
            position, size = member
            member = self._payload(name, position, size)
            if self.cache:
                self.members[name] = member
            return member
        else:
            return member

    def _payload(self, name, position, size):
        """
        Parse payload of a frame member.
        :param name: of frame member
        :returns parsed message
        """
        # parse payload
        try:
            payload = self.scanner.read_data(position, size)
        except EOFError:
            raise FrameParseException(f'Failed reading data of {name}, file ended unexpectedly.')

        if payload:
            return self.parser.parse(name, payload)
        else:
            return None

    def names(self):
        """
        :returns names of frame members part of the frame
        """
        return self.members.keys()

    def raw_members(self):
        for name, member in self.members.items():
            if isinstance(member, tuple):
                position, size = member
                data = self.scanner.read_data(position, size)
                # member is not parsed yet, get payload from log file frame
                yield name, data
            else:
                yield name, member.SerializeToString()

    def __len__(self):
        """
        :returns size of the frame in the log file
        """
        return self.size

    def __copy__(self):
        frame = Frame(self.start, self.number, self.scanner, self.parser)
        for name, member in self.members.items():
            if isinstance(member, tuple):
                position, size = member
                frame.add_member(name, position, size)
            else:
                frame.add_message_member(name, member)
        return frame

    @staticmethod
    def bytes_from_char(char):
        return struct.pack('=c', bytes(char, 'utf-8'))

    @staticmethod
    def bytes_from_str(string):
        return b''.join(Frame.bytes_from_char(char) for char in string) + Frame.bytes_from_char('\0')

    @staticmethod
    def long_from_int(integer):
        return struct.pack('=l', integer)

    def __bytes__(self):
        """
        :returns bytes representation of frame ready to write to log file
        """
        frame_number = self.long_from_int(self.number)
        _bytes = b''.join(
            frame_number
            + self.bytes_from_str(name)
            + self.long_from_int(len(data))
            + data
            for name, data in self.raw_members()
        )
        assert self.size == len(_bytes)
        return _bytes


class LogReader:
    def __init__(self, path, parser=LogParser()):
        self.log_file = open(path, 'rb')
        self.scanner = LogScanner(self.log_file)
        self.parser = parser

        self.frames = []
        self.furthest_scan_position = 0
        self.eof_reached = False

    def __enter__(self):
        return self

    def __exit__(self, _type, value, traceback):
        self.close()

    def close(self):
        """
        Close underlying file handle
        """
        self.log_file.close()

    def index_of(self, frame_number):
        """
        Get log reader index to the corresponding frame number

        :param frame_number: number of log file frame
        :return: log reader index or -1 of the frame doesn't exist
        """
        # TODO: optimize search
        for i, frame in enumerate(self.read()):
            if frame.number == frame_number:
                return i
        else:
            return -1

    def read_from(self, start_idx):
        """
        Generator yielding log file frames starting from frame i.
        """
        if start_idx < 0:
            raise ValueError(f'Start index <{start_idx}> must be positive!')

        # yield frames already read
        while start_idx < len(self.frames):
            yield self.frames[start_idx]
            start_idx += 1

        if self.eof_reached:
            # got nothing left to read
            return

        # read more frames from the log file
        idx = len(self.frames)
        self.scanner.set_scan_position(self.furthest_scan_position)
        frame = None
        while True:
            # read next frame member
            try:
                frame_member_start, current_frame_number, name, payload_pos = self.scanner.scan_frame_member()
            except EOFError:
                self.eof_reached = True
                break

            # check if frame member is part of current frame or create new one
            if frame is None:
                frame = Frame(frame_member_start, current_frame_number, self.scanner, self.parser)
            elif current_frame_number != frame.number:
                # finished reading frame
                self.frames.append(frame)
                self.furthest_scan_position = frame_member_start
                idx += 1

                frame_to_yield = frame
                # create new frame
                frame = Frame(frame_member_start, current_frame_number, self.scanner, self.parser)

                # only yield frames from the start
                if start_idx < len(self.frames):
                    yield frame_to_yield

                    # Check if we already got new frames, since we yielded and
                    #       other iterators could have advanced further by now
                    if idx < len(self.frames):
                        while idx < len(self.frames):
                            yield self.frames[idx]
                            idx += 1
                                
                        # discard started frame
                        frame = None
                        self.scanner.set_scan_position(self.furthest_scan_position)

            # add frame member
            position, size = payload_pos
            frame.add_member(name, position, size)

        # End of file is reached, yield the last log file frame
        if frame is not None and frame.members:
            self.frames.append(frame)

            # only yield frames from the start
            if start_idx < len(self.frames):
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

    def file_path(self):
        """
        :returns log file path
        """
        return self.log_file.name


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
            if 'FrameInfo' in _frame:
                print(_frame.number, _frame['FrameInfo'], end='')
        print()

        print('Number of edgels of 3 frames after the 10th frame:')
        for _i, _frame in enumerate(log_reader.read_from(11)):
            if 'ScanLineEdgelPerceptTop' in _frame:
                edgels = _frame['ScanLineEdgelPerceptTop'].edgels
                print('ScanLineEdgelPerceptTop of frame', _frame.number, 'contains', len(edgels), 'edgels')
            if _i == 2:
                break
