import os
import struct as _struct
from typing import Iterator

from ._experimental_parser import Parser

# protobuf
from google.protobuf.message import Message as _Message

"""
NaoTH log file parsing implementation in python 3.

Open a NaoTH log file over the Reader class. For easy cleanup, the use of pythons "with" keyword is supported and 
highly recommended:

# open a log file example
with Reader('naoth_logfile.log') as reader:
    # parse log file frames... 
    # Note: All operations on the reader and its parsed log file frames must be contained in this scope
    
If you don't make use of the "with" keyword, make sure to close the reader by calling reader.close()

Log file frames can be parsed using the reader.read() method. This function implements an iterator returning Frame instances.
For more information on log file frames, see the documentation in the Frame class.

# frame parsing example
for frame in reader.read():
    # retrieve some data from the frame...
"""


class IncompleteFrameException(Exception):
    """
    This exception is raised in Reader.read and Reader.diet_read if an incomplete frame was detected,
    except the reader is instanced with skip_incomplete=True (default)
    """
    pass


class Scanner:
    """
    Interface to the log file handle. Allows scanning the log file for positions of NaoTH representations.
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
        return _struct.unpack('=l', self._read_bytes(4))[0]

    def _read_char(self):
        char = _struct.unpack('=c', self._read_bytes(1))[0]
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
    def size_of_field(name, payload_size):
        """
        Calculates the byte string size of a NaoTH representation in the log file
        :param name: of member
        :param payload_size
        :returns sizeOf(frame number) + sizeOf(name + '\0') + sizeOf(payload size) + payload size
        """
        return _struct.calcsize('=l') + (len(name) + 1) * _struct.calcsize('=c') + _struct.calcsize('=l') + payload_size

    def set_scan_position(self, frame_start_position):
        """
        Sets the position of the scanner. BEWARE: Must be the start of a NaoTH representation in the log file.
        :param frame_start_position: position in log file where a NaoTH representation starts
        """
        self._scan_position = frame_start_position

    def scan_field(self):
        """
        Reads name, frame number and payload positions of the next NaoTH representation.
        :returns start position, corresponding frame number, payload position (position, size) tuple
        """
        field_start = self._scan_position
        self.log_file.seek(field_start)

        # read message header
        frame_number = self._read_long()

        name = self._read_string()
        message_size = self._read_long()
        data_pos = (self.log_file.tell(), message_size)

        # calculate next field position (skipping payload of previous one)
        self._scan_position = self.log_file.tell() + message_size

        return field_start, frame_number, name, data_pos

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


class Frame:
    """
    NaoTH log file frame returned by iterating over the Reader.read method.

    A frame is a collection of logged NaoTH representations with a matching frame number (executed time step).
    NaoTH representations are referred to as (name, field) tuples, where "field" corresponds to the NaoTH representation
    as protobuf Message.

    NaoTH Representations can be accessed and are being parsed by calling the __getitem__ method with its name.
        e.g. frame['FrameInfo'] where 'FrameInfo' is the name of the representation
    """

    def __init__(self, frame_start, frame_number, scanner, parser, cache=False):
        """
        NaoTH log file frame containing NaoTH representations as fields.

        :param frame_start: start of the frame in the log file
        :param frame_number: executed time step
        :param scanner: Scanner of current log file
        :param parser: Parser of current log file
        :param cache: if True cache already parsed members, may increase performance but affects memory usage
        """
        self.scanner = scanner
        self.parser = parser

        # Frame position in the log file
        self.start = frame_start

        # Size of the frame, if it would be converted to bytes using the __bytes__ method:
        # frame.size == len(bytes(frame))
        # Note: When NaoTH representations are added externally (e.g. by calling the add_field function),
        #       the size value does not match the size of the originating log file frame.
        self.size = 0

        self.number = frame_number

        """
        Fields (NaoTH representations) of this frame. Maps name of the NaoTH representation to either
            - a (position, size) tuple of NaoTH representation data in the log file
            - a parsed protobuf class representing the NaoTH representation
        """
        self._fields = {}

        self.cache = cache

    def add_field_position(self, name, position, size):
        """
        Add a NaoTH representation to this frame based on its position and size in the log file.
        This will later be parsed when calling the __getitem__ function with the given name.

        This function is usually only called by the log reader while creating this frame.

        :param name: of the NaoTH representation
        :param position: of payload data in the log file
        :param size: of payload data in the log file
        """
        if name in self._fields:
            raise ValueError(f'Frame already contains member {name}.')

        self._fields[name] = position, size
        self.size += Scanner.size_of_field(name, size)

    def add_field(self, name, _field: _Message):
        """
        Add a parsed or new NaoTH representation in form of a protobuf message to this frame.
        This is useful if we are combining log files, like adding camera images which where logged separately.
        e.g. See combine_logs.py example.

        :param name: of the NaoTH representation
        :param _field: protobuf message instance of the NaoTH representation
        """
        self.size += Scanner.size_of_field(name, _field.ByteSize())
        # add new member
        self._fields[name] = _field

    def remove(self, name):
        """
        Remove a member from this frame.
        :param name: of member
        """
        member = self._fields.pop(name)

        # reduce size of this frame
        if isinstance(member, tuple) and len(member) == 2:
            position, size = member
            self.size -= Scanner.size_of_field(name, size)
        elif isinstance(member, _Message):
            self.size -= Scanner.size_of_field(name, member.ByteSize())
        else:
            raise TypeError(f'Type of member {type(member)} must be a (position, size) tuple or protobuf message!')

    def replace_field(self, name, field: _Message):
        """
        Replace a field of this frame with a protobuf message object.
        :param name: of the NaoTH representation
        :param field: protobuf message instance of the NaoTH representation
        """
        if name not in self._fields:
            raise ValueError(f'Frame does not contain member {name}.')

        # remove old member
        self.remove(name)

        self.add_field(name, field)

    def __contains__(self, name):
        """
        Check if the given name matches a NaoTH representation of this frame.

        # e.g. sanity check before accessing the field 'FrameInfo'
        if 'FrameInfo' in frame:
            frame_info = frame['FrameInfo']

        :param name: of a NaoTH representation
        :returns True if a name corresponding NaoTH representation is part of this frame
        """
        return name in self._fields

    def __getitem__(self, name) -> _Message:
        """
        Parse and return the NaoTH representation corresponding to the given name.
        If caching is enabled, save already parsed members for future access.

        # e.g. retrieve 'FrameInfo' representation
        frame_info = frame['FrameInfo']

        :param name: of a NaoTH representation part of this frame
        :returns NaoTH representation as protobuf Message instance
        """
        message = self._fields[name]
        if isinstance(message, tuple):
            if len(message) != 2:
                raise TypeError(f'Type of field {type(message)} must be a (position, size) tuple'
                                f'to be parsed or a protobuf message!')
            # message is not parsed yet, get payload from log file frame
            position, size = message
            try:
                payload = self.scanner.read_data(position, size)
            except EOFError:
                # raise again with fitting error message
                raise EOFError(f'Failed reading data of field {name}, file ended unexpectedly.')

            # parse payload
            if payload:
                message = self.parser.parse(name, payload)
            else:
                message = None

            if self.cache:
                self._fields[name] = message
            return message
        else:
            return message

    def get_names(self):
        """
        :returns names of NaoTH representations part of this frame
        """
        return self._fields.keys()

    def raw_fields(self):
        """
        This function implements an iterator returning the name and NaoTH representation bytes part of this frame
        by using pythons generator functions.

        This is primarily used to serialize this frame in the __bytes__ method.
        """
        for name, message in self._fields.items():
            if isinstance(message, tuple) and len(message) == 2:
                position, size = message
                # message is not parsed yet, get payload from log file frame
                data = self.scanner.read_data(position, size)
                yield name, data
            elif isinstance(message, _Message):
                yield name, message.SerializeToString()
            else:
                raise TypeError(f'Type of field {type(message)} must be a (position, size) tuple or protobuf message!')

    def __len__(self):
        """
        :returns the size of this frame in bytes
        """
        return self.size

    def copy(self):
        """
        :returns a copy of this frame and all its fields.
        """
        frame = Frame(self.start, self.number, self.scanner, self.parser)
        for name, message in self._fields.items():
            if isinstance(message, tuple) and len(message) == 2:
                position, size = message
                frame.add_field_position(name, position, size)
            elif isinstance(message, _Message):
                message_copy = _Message()
                message_copy.CopyFrom(message)
                frame.add_field(name, message_copy)
            else:
                raise TypeError(f'Type of field {type(message)} must be a (position, size) tuple or protobuf message!')
        return frame

    @staticmethod
    def bytes_from_char(char):
        return _struct.pack('=c', bytes(char, 'utf-8'))

    @staticmethod
    def bytes_from_str(string):
        return b''.join(Frame.bytes_from_char(char) for char in string) + Frame.bytes_from_char('\0')

    @staticmethod
    def long_from_int(integer):
        return _struct.pack('=l', integer)

    def __bytes__(self):
        """
        :returns bytes representation of this frame ready be to written to log file

        # log file write example
        with open('naoth_logfile.log', 'wb') as log_file:
            log_file.write(bytes(frame))
        """
        frame_number = self.long_from_int(self.number)
        _bytes = b''.join(
            frame_number
            + self.bytes_from_str(name)
            + self.long_from_int(len(data))
            + data
            for name, data in self.raw_fields()
        )
        assert self.size == len(_bytes)
        return _bytes


class Reader:
    """
    NaoTH log file reader.

    A Nao running the NaoTH Framework logs the state of its representations for every executed time step.
    Representations include camera images, camera matrix data, motion and sensor data, the behavior state and more.

    This log reader combines logged representations to a frame based on their common executed time steps (frame number).

    Create an instance of this reader with a path to a NaoTH Framework generated log file (see def __init__()).
    "with" statement instantiation is supported and highly encouraged to allow the underlying file handle to be closed.
    Otherwise you have to call the close() function yourself.

    To retrieve the frames of a log file call the read() function of an instanced reader.
    This will return an iterator which can be iterated over returning frames in order of their executed time step:

    for frame in reader.read():
        # do something with the frame
    """

    def __init__(self, path, parser=Parser(), skip_incomplete=True):
        """
        Creates a reader instance for parsing a NaoTH log file.

        :param path: path to a log file generated by the NaoTH framework.
        :param parser: optional Parser argument. If logged representations do not have a matching
                       protobuf implementation a specific Parser to handle those cases must be given.
        :param skip_incomplete: Skip incomplete frames and do not raise an IncompleteFrameException (default: True)
        """
        self.log_file = open(path, 'rb')
        self.log_file_size = os.path.getsize(path)

        self.skip_incomplete = skip_incomplete

        # Used to scan the log file for positions of NaoTH representations and to create an index.
        self.scanner = Scanner(self.log_file)
        # Required for parsing the corresponding protobuf classes based on the retrieved NaoTH representation bytes.
        self.parser = parser

        # Index of frames retrieved from the log file
        self.frames = []

        self.furthest_scan_position = 0
        self.eof_reached = False

    def __enter__(self):
        """
        __enter__ function implemented to support the python "with" keyword.
        """
        return self

    def __exit__(self, _type, value, traceback):
        """
        __exit__ function implemented to support the python "with" keyword.
        """
        self.close()

    def close(self):
        """
        Close the underlying file handle.
        """
        self.log_file.close()

    def index_of(self, frame_number):
        """
        Get log reader index position to the corresponding frame number.

        :param frame_number: number of log file frame
        :return: log reader index or None of the frame doesn't exist
        """
        # TODO: optimize search
        for i, frame in enumerate(self.read()):
            if frame.number == frame_number:
                return i
        else:
            return None

    def read(self, start_idx=0) -> Iterator[Frame]:
        """
        Creates an iterator over the log file frames implemented using pythons generator functions.
        With the "yield" keyword, we return the next Frame instance for the iterator.

        # Example iteration using the read method
        for frame in reader.read():
            # do something with the frame

        :param start_idx: First frame for starting the iterator (default: 0 iterate over all frames).
        :raise IncompleteFrameException if an incomplete frame was detected,
               except the reader is instanced with skip_incomplete=True (default)
        """
        if start_idx < 0:
            raise IndexError(f'Start index <{start_idx}> must be positive!')

        # return frames which were already indexed in self.frames
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
            # read next NaoTH representation from log file
            try:
                start_of_field, current_frame_number, name, payload_pos = self.scanner.scan_field()
            except EOFError:
                # stop reading
                self.eof_reached = True
                break

            # Decide which frame the read NaoTH representation is part of
            if frame is None:
                # create a new frame
                frame = Frame(start_of_field, current_frame_number, self.scanner, self.parser)
            elif current_frame_number != frame.number:
                # frame number does not match, read NaoTH representation is part of a new frame

                # before creating a new frame, finish up the last one and return it
                self.frames.append(frame)
                idx += 1
                # only return frames which come after the given start index
                if start_idx < len(self.frames):
                    yield frame

                    if idx < len(self.frames):
                        # After we yielded, another read iterator was advanced further (has read more frames).
                        # This can happen for example, if the user calls the __getitem__ method for a bigger index.
                        # Return already indexed frames and restart the log file scan at the end of that index.
                        while idx < len(self.frames):
                            yield self.frames[idx]
                            idx += 1
                        if self.eof_reached:
                            return
                        # reset scan
                        frame = None
                        self.scanner.set_scan_position(self.furthest_scan_position)
                        continue

                # create new frame
                frame = Frame(start_of_field, current_frame_number, self.scanner, self.parser)

            # Add the read NaoTH representation to its frame
            position, size = payload_pos
            frame.add_field_position(name, position, size)
            self.furthest_scan_position = position + size

        # End of file is reached, return the last log file frame
        if frame is not None and frame.get_names():
            # check if the last frame is complete
            if frame.start + len(frame) <= self.log_file_size:
                self.frames.append(frame)

                # only return frame if it comes after the given start index
                if start_idx < len(self.frames):
                    yield frame
            elif not self.skip_incomplete:
                raise IncompleteFrameException(f'Last log file frame is incomplete, missing '
                                               f'{frame.start + len(frame) - self.log_file_size} bytes.')

    def diet_read(self) -> Iterator[Frame]:
        """
        Same as "read", but does not create a frame index and preserves memory (len(self.frames) will stay 0).
        Useful if the log file is very large and only a single pass through is required.

        :raise IncompleteFrameException if an incomplete frame was detected,
               except the reader is instanced with skip_incomplete=True (default)
        """
        # read frames from the log file
        self.scanner.set_scan_position(0)
        frame = None
        while True:
            # read next NaoTH representation from log file
            try:
                start_of_field, current_frame_number, name, payload_pos = self.scanner.scan_field()
            except EOFError:
                # stop reading
                break

            # Decide which frame the read NaoTH representation is part of
            if frame is None:
                # create a new frame
                frame = Frame(start_of_field, current_frame_number, self.scanner, self.parser)
            elif current_frame_number != frame.number:
                # frame number does not match, read NaoTH representation is part of a new frame

                # before creating a new frame, finish up the last one and return it
                yield frame

                # create new frame
                frame = Frame(start_of_field, current_frame_number, self.scanner, self.parser)

            # Add the read NaoTH representation to its frame
            position, size = payload_pos
            frame.add_field_position(name, position, size)
            self.scanner.set_scan_position(position + size)

        # End of file is reached, return the last log file frame
        if frame is not None and frame.get_names():
            # check if the last frame is complete
            if frame.start + len(frame) <= self.log_file_size:
                yield frame
            elif not self.skip_incomplete:
                raise IncompleteFrameException(f'Last log file frame is incomplete, missing '
                                               f'{frame.start + len(frame) - self.log_file_size} bytes.')

    def __getitem__(self, i):
        """
        Get ith frame from the log file.

        # e.g.
        3rd_frame = reader[2]
        """
        if i >= len(self.frames):
            # frame i was not indexed yet, read more frames
            try:
                return next(self.read(start_idx=i))
            except StopIteration:
                raise IndexError
        return self.frames[i]

    def file_path(self):
        """
        :returns underlying log file path
        """
        return self.log_file.name
