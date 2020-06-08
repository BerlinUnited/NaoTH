import struct as _struct
import asyncio as _as
import threading as _thread
import queue as _queue
import socket

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
        '''
        q = _queue.Queue()
        dest_thread = _thread.Thread(target=DebugProxy.destinationThread, args=('localhost', dest_port, q))
        robot_thread = _thread.Thread(target=DebugProxy.robotThread, args=(src_host, src_port, q))

        dest_thread.start()
        robot_thread.start()

        dest_thread.join()
        robot_thread.join()
        '''

        loop = _as.new_event_loop()
        proxy = DebugProxy(src_host, src_port, dest_port, loop)

        try:
            loop.run_forever()
        except KeyboardInterrupt:
            print('Got interrupted!')
            pass
        finally:
            proxy.running = False
            loop.stop()

        loop.close()

    @staticmethod
    def destinationThread(host, port, q: _queue.Queue):
        s = socket.create_connection((host, port))
        while True:
            raw_id = s.recv(4)
            q.put(raw_id)

            command_id = _struct.unpack('=l', raw_id)[0]
            if command_id != -1:
                raw_length = s.recv(4)
                q.put(raw_length)

                command_length = _struct.unpack('=l', raw_length)[0]

                # relay and parse command
                command_bytes = s.recv(command_length)
                q.put(command_bytes)

                # parse command bytes
                command = _pb.Messages_pb2.CMD()
                command.ParseFromString(command_bytes)
                print(command)
            pass

    @staticmethod
    def robotThread(host, port, q: _queue.Queue):
        s = None
        while True:
            raw_data = q.get()

            if s is None:
                try:
                    s = socket.create_connection((host, port), 1)
                except:
                    s = None

            if s is not None:
                s.sendall(raw_data)




            pass

    def __init__(self, src_host, src_port, dest_port, loop = None):
        self.robot_addr = (src_host, src_port)

        self.robot_host = src_host
        self.robot_port = src_port
        self._robot_reader = None
        self._robot_writer = None

        self.host_host = 'localhost'
        self.host_port = dest_port
        self._host_reader = None
        self._host_writer = None

        self.loop = _as.get_event_loop() if loop is None else loop
        self.running = True

        self.cmd_id = 1
        self.cmd_map = {}

        self.connected = _as.Event(loop=self.loop)
        #self.loop.create_task(self._host(), name='Host Task')
        self.loop.create_task(self._robot(), name='Robot Task')

        self.hosts = []
        self.host_listener = self.loop.create_task(_as.start_server(self._host, self.host_host, self.host_port), name='Host Listener')
        self.host_connection_cnt = 0

        print(self.host_host, self.host_port)

    def _register_host(self):
        self.connected.set()
        self.host_connection_cnt += 1
        _as.Task.current_task().set_name('Host-{}'.format(self.host_connection_cnt))
        self.hosts.append(_as.Task.current_task())

        print('Got connection for', _as.Task.current_task().get_name())

    def _unregister_host(self):
        self.hosts.remove(_as.Task.current_task())
        if len(self.hosts) == 0:
            self.connected.clear()

    async def _host(self, stream_reader, stream_writer):
        self._register_host()

        while self.running:
            try:
                raw_id = await stream_reader.read(4)

                if raw_id == b'': break

                cmd_id = _struct.unpack('=l', raw_id)[0]

                # check if command is not just a heart beat
                if cmd_id != -1:
                    # relay and parse command length
                    raw_length = await stream_reader.read(4)
                    cmd_length = _struct.unpack('=l', raw_length)[0]

                    # relay and parse command
                    raw_data = await stream_reader.read(cmd_length)

                    await self.send_to_robot(cmd_id, raw_length + raw_data, stream_writer)


                    # parse command bytes
                    command = _pb.Messages_pb2.CMD()
                    command.ParseFromString(raw_data)
                    print(command)
            except Exception as e:
                print(_as.Task.current_task().get_name(), ':', e)

        self._unregister_host()

    async def _robot(self):
        print('Started robot task ...')
        while self.running:
            if not self.connected.is_set():
                # TODO: close connection before waiting
                print('wait for connection ...')
                await self.connected.wait()
                print('continue robot task ...')

            if self._robot_reader is None:
                try:
                    self._robot_reader, self._robot_writer = await _as.open_connection(host=self.robot_host, port=self.robot_port, loop=self.loop)
                    print('Established connection to robot!')
                except Exception as e:
                    #print(_as.Task.current_task().get_name(), ':', e, type(e))
                    await _as.sleep(1)
                    # Skip the rest, since we doesn't have a valid connection to work with
                    continue
            # relay and parse command id
            raw_id = await self._robot_reader.read(4)
            cmd_id = _struct.unpack('=l', raw_id)[0]

            # relay and parse response size
            raw_size = await self._robot_reader.read(4)
            size = _struct.unpack('=l', raw_size)[0]

            # relay and parse response data
            raw_data = await self._robot_reader.read(size)

            await self.send_to_host(cmd_id, raw_size + raw_data)

            try:
                print('Size', size)
                data = raw_data.decode('utf-8')
                print(data)
            except UnicodeDecodeError:
                pass

    async def send_to_robot(self, cmd_id, cmd_raw_data, return_stream):
        print('Send to robot', cmd_id, self._robot_writer)
        if self._robot_writer is not None:
            self.cmd_map[self.cmd_id] = (cmd_id, return_stream)
            self._robot_writer.write(_struct.pack("<I", self.cmd_id) + cmd_raw_data)
            self.cmd_id += 1
            await self._robot_writer.drain()

    async def send_to_host(self, cmd_id, cmd_raw_data):
        print('Send to host', cmd_id, len(self.cmd_map))
        if cmd_id in self.cmd_map:
            host_cmd_id, host_writer = self.cmd_map.pop(cmd_id)
            try:
                host_writer.write(_struct.pack("<I", host_cmd_id) + cmd_raw_data)
                await host_writer.drain()
            except:
                pass


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
