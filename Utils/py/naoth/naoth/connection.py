import asyncio
import struct

from naoth.messages.Messages_pb2 import CMD


class Relay:
    """
    Relay a connection to the robot to inspect the control flow
    """

    def __init__(self, sink_port, robot_addr):
        self.robot_addr = robot_addr

        server = asyncio.start_server(self._relay_commander, '127.0.0.1', sink_port, loop=asyncio.get_event_loop())
        asyncio.ensure_future(server)

    async def _relay_commander(self, commander_reader, commander_writer):
        robot_writer = None
        while True:
            # awaiting command
            raw_id = await commander_reader.read(4)

            # create relaying connection
            if robot_writer is None:
                robot_reader, robot_writer = await asyncio.open_connection(host=self.robot_addr[0],
                                                                           port=self.robot_addr[1],
                                                                           loop=asyncio.get_event_loop())
                asyncio.ensure_future(self._relay_robot(robot_reader, commander_writer))

            # relay and parse command id
            robot_writer.write(raw_id)
            command_id = read_long(raw_id)

            # check if command is not just a heart beat
            if command_id != -1:
                # relay and parse command length
                raw_length = await commander_reader.read(4)
                robot_writer.write(raw_length)

                command_length = read_long(raw_length)

                # relay and parse command
                command_bytes = await commander_reader.read(command_length)
                robot_writer.write(command_bytes)

                # parse command bytes
                command = CMD()
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

            size = read_long(raw_size)
            print('Size', size)

            # relay and parse response data
            raw_data = await robot_reader.read(size)
            command_writer.write(raw_data)
            try:
                data = raw_data.decode('utf-8')
                print(data)
            except UnicodeDecodeError:
                pass


class Command:
    def __init__(self, _id, name, *args, **vargs):
        self._id = _id
        self.name = name
        self.args = {}

        map(self.add_arg, args)
        map(self.add_arg, vargs.keys(), vargs.values())

    def add_arg(self, name, value=None):
        if isinstance(value, str):
            value = value.encode()
        if isinstance(value, bytes) or value is None:
            self.args[name] = value
        else:
            raise ValueError('Argument value must be string, bytes or None')

    def write(self, sink):
        sink.write(self.__bytes__())

    def __bytes__(self):
        cmd = CMD()
        cmd.name = self.name
        for name, value in self.args.items():
            arg = cmd.args.add()
            arg.name = name
            if value is not None:
                arg.bytes = value
        raw_command = cmd.SerializeToString()
        return long_from_int(self._id) + long_from_int(len(raw_command)) + raw_command


class Response:
    def __init__(self):
        self._id = None
        self.data = None

    async def read(self, reader):
        # read command id
        raw_id = await reader.read(4)
        self._id = read_long(raw_id)

        # read response size
        raw_size = await reader.read(4)

        size = read_long(raw_size)
        # read response data
        self.data = await reader.read(size)

    def get_id(self):
        return self._id


def long_from_int(integer):
    return struct.pack('=l', integer)


def read_long(_bytes):
    return struct.unpack('=l', _bytes)[0]


if __name__ == '__main__':
    loop = asyncio.get_event_loop()

    relay = Relay(7777, ('127.0.0.1', 5401))

    try:
        loop.run_forever()
    except KeyboardInterrupt:
        pass

    loop.close()
