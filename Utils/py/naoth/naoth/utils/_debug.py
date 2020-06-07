import struct as _struct
import asyncio as _as

from .. import pb as _pb


class DebugProxy:
    """
    Debugging class, creating a connection between RobotControlGui and the Nao to inspect messages and the control flow.
    """

    @staticmethod
    def run(src_host, src_port=5401, dest_port=7777):
        """
        Opens a relay between RobotControl and Nao
        :param src_host     the ip/hostname of the naoth instance (eg. 'localhost', '10.0.4.91')
        :param src_port     the debug port of the naoth instance; by default it is 5401
        :param dest_port    the port, where the debug commands should be relayed to; defaults to: 7777
        """
        proxy = DebugProxy(src_host, src_port, dest_port)

        loop = _as.get_event_loop()

        try:
            loop.run_forever()
        except KeyboardInterrupt:
            print('Got interrupted!')
            pass

        loop.close()

    def __init__(self, src_host, src_port, dest_port):
        self.robot_addr = (src_host, src_port)

        server = _as.start_server(self._relay_commander, '127.0.0.1', dest_port, loop=_as.get_event_loop())
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
