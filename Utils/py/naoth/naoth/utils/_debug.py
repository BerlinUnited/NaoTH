import struct as _struct
import asyncio as _as
import typing as _t

from .. import pb as _pb


class DebugProxy:
    """
    Debugging class, creating a connection between RobotControlGui and the Nao to inspect messages and the control flow.
    """

    @staticmethod
    def run(sink_port: int, nao_addr: _t.Tuple[str, int]):
        """
        Opens a relay between RobotControl and Nao
        :param sink_port: Port, RobotControl connects to
        :param nao_addr: (Address, Port) Tuple of the Nao
        """
        DebugProxy(sink_port, nao_addr)

        loop = _as.get_event_loop()

        try:
            loop.run_forever()
        except KeyboardInterrupt:
            pass

        loop.close()

    def __init__(self, sink_port, robot_addr):
        self.robot_addr = robot_addr

        server = _as.start_server(self._relay_commander, '127.0.0.1', sink_port, loop=_as.get_event_loop())
        _as.ensure_future(server)

    async def _relay_commander(self, commander_reader, commander_writer):
        robot_writer = None
        while True:
            # awaiting command
            raw_id = await commander_reader.read(4)

            # create relaying connection
            if robot_writer is None:
                robot_reader, robot_writer = await _as.open_connection(host=self.robot_addr[0],
                                                                       port=self.robot_addr[1],
                                                                       loop=_as.get_event_loop())
                _as.ensure_future(self._relay_robot(robot_reader, commander_writer))

            # relay and parse command id
            robot_writer.write(raw_id)
            command_id = _struct.unpack('=l', raw_id)[0]

            # check if command is not just a heart beat
            if command_id != -1:
                # relay and parse command length
                raw_length = await commander_reader.read(4)
                robot_writer.write(raw_length)

                command_length = _struct.unpack('=l', raw_length)[0]

                # relay and parse command
                command_bytes = await commander_reader.read(command_length)
                robot_writer.write(command_bytes)

                # parse command bytes
                command = _pb.Messages_pb2.CMD()
                command.ParseFromString(command_bytes)
                print(command)

                # check if python implementation produces the same result
                """
                TODO: fix
                native_command = Command(command.name)
                for arg in command.args:
                    if arg.bytes:
                        native_command.add_arg(arg.name, arg.bytes)
                    else:
                        native_command.add_arg(arg.name)
                assert command_bytes == bytes(native_command)
                """

    @staticmethod
    async def _relay_robot(robot_reader, command_writer):
        while True:
            # relay and parse command id
            raw_id = await robot_reader.read(4)
            command_writer.write(raw_id)

            # relay and parse response size
            raw_size = await robot_reader.read(4)
            command_writer.write(raw_size)

            size = _struct.unpack('=l', raw_size)[0]
            print('Size', size)

            # relay and parse response data
            raw_data = await robot_reader.read(size)
            command_writer.write(raw_data)
            try:
                data = raw_data.decode('utf-8')
                print(data)
            except UnicodeDecodeError:
                pass


class DebugCommand:
    """A DebugCommad used in our naoth framework with name and arguments."""

    def __init__(self, name, *args, **value_args):
        """
        Create a command which can be send to the robot.
        For example:
            DebugCommand('Cognition:behavior:set_agent', agent='soccer_agent')
            DebugCommand('Cognition:behavior:set_agent').add_arg('agent', 'soccer_agent')
            DebugCommand('Cognition:representation:get', 'FrameInfo')
            DebugCommand('Cognition:representation:get').add_arg('FrameInfo')

        :param name: Command name
        :param args: Command arguments (see add_arg function)
        :param value_args: Command arguments with values (see add_arg function)
        """
        self._id = 0
        self._name = name
        self._args = {}

        for name in args:
            self.add_arg(name)

        for name, value in value_args.items():
            self.add_arg(name, value=value)

    @property
    def name(self):
        """Returns the name of this command."""
        return self._name

    @property
    def id(self):
        """Returns the id of this command."""
        return self._id

    @id.setter
    def id(self, value):
        """Sets the id of this command."""
        self._id = value
        return self

    def add_arg(self, name, value=None):
        """
        Add an argument to this command with an optional value
        :param name: argument name
        :param value: optional argument value
        """
        if isinstance(value, str):
            self._args[name] = value.encode()
        elif isinstance(value, bytes) or value is None:
            self._args[name] = value
        else:
            raise ValueError('Argument value must be string, bytes or None')
        return self

    def write(self, sink):
        """
        Send this command.
        :param sink: to write
        """
        sink.write(bytes(self))

    def __bytes__(self):
        """
        :returns bytes representation of this command ready to send
        """
        cmd = _pb.Messages_pb2.CMD()
        cmd.name = self._name
        for name, value in self._args.items():
            arg = cmd.args.add()
            arg.name = name
            if value is not None:
                arg.bytes = value
        raw_command = cmd.SerializeToString()
        return _struct.pack('=l', self._id) + _struct.pack('=l', len(raw_command)) + raw_command


class DebugResponse:
    """Reads a response from the Nao and parses id and size"""

    def __init__(self):
        self._id = None
        self.data = None

    async def read(self, reader):
        # read command id
        raw_id = await reader.read(4)
        self._id = _struct.unpack('=l', raw_id)[0]

        # read response size
        raw_size = await reader.read(4)

        size = _struct.unpack('=l', raw_size)[0]
        # read response data
        self.data = await reader.read(size)

    def get_id(self):
        return self._id
