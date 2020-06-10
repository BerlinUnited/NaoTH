import struct as _struct
import asyncio as _as
import threading as _thread
import queue as _queue
import socket
import time
import concurrent.futures
import sys

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


class DebugCommand(concurrent.futures.Future):
    """Class representing a command for a naoth agent."""

    def __init__(self, name, args=None):
        """
        Constructor for the command.

        :param name:    the name of the command
        :param args:    additional arguments of the command
        """
        super().__init__()
        self._id = 0
        self._name = name
        self._args = args

    @property
    def id(self):
        return self._id

    @id.setter
    def id(self, value):
        self._id = value

    @property
    def name(self):
        """Returns the name of this command."""
        return self._name

    def add_arg(self, arg):
        """Adds an argument to this command."""
        self._args.append(arg)

    def serialize(self, cmd_id):
        """
        Serializes the command to a byte representation in order to send it to the agent.

        :param cmd_id:  the id for identifying the command
        :return:        returns the bytes representation of this command
        """
        cmd_args = []
        if self._args:
            for a in self._args:
                if isinstance(a, str):
                    cmd_args.append(_pb.Messages_pb2.CMDArg(name=a))
                else:
                    cmd_args.append(_pb.Messages_pb2.CMDArg(name=a[0], bytes=a[1].encode()))

        proto = _pb.Messages_pb2.CMD(name=self.name, args=cmd_args)
        return _struct.pack("<I", cmd_id) + _struct.pack("<I", proto.ByteSize()) + proto.SerializeToString()


class AgentControllerAsync:
    def __init__(self, host, port, start=True, loop=None):
        self._host = host
        self._port = port
        self._task = None
        self._loop = loop if loop else _as.get_event_loop()
        self._cmd_id = 1
        self._cmd_q = _as.Queue()

        if start: self._start()

    def _start(self):
        self._loop.create_task(self._run(), name=self.__class__.__name__)
        print('Started', self.__class__.__name__)

    def _stop(self):
        pass

    async def _run(self):
        print('Running', self.__class__.__name__)
        while True:
            if self._stream_reader is None:
                try:
                    self._stream_reader, self._stream_writer = await _as.open_connection(host=self._host, port=self._port, loop=self._loop)
                    #print('Established connection to robot!')
                except Exception as e:
                    #print(_as.Task.current_task().get_name(), ':', e, type(e))
                    await _as.sleep(1)
                    # Skip the rest, since we doesn't have a valid connection to work with
                    continue

            await self._send_heart_beat()
            await self._poll_answers()
            await self._send_commands()

    async def _send_heart_beat(self):
        """Send a heart beat to the agent."""
        if self._stream_writer:
            self._stream_writer.write(_struct.pack('!i', -1))
            await self._stream_writer.drain()

    async def _poll_answers(self):
        if self._stream_reader:
            raw_id = await self._stream_reader.read(4)
            cmd_id = _struct.unpack('=l', raw_id)[0]

            raw_size = await self._stream_reader.read(4)
            size = _struct.unpack('=l', raw_size)[0]

            raw_data = await self._stream_reader.read(size)

            # TODO: what to do with the response?!
            try:
                print('Size', size)
                data = raw_data.decode('utf-8')
                print(data)
            except UnicodeDecodeError:
                pass

    async def _send_commands(self):
        if self._stream_writer:
            self._stream_writer.write(_struct.pack('!i', -1))
            await self._stream_writer.drain()

    def send_command(self, cmd:DebugCommand):
        if self._task:
            cmd.id = self._cmd_id
            self._cmd_q.put_nowait(cmd)
            self._cmd_id += 1
            return cmd.id

        return 0

    def debugrequest(self, request:str, enable:bool, type:str='cognition'):
        """
        Enables/Disables a debug request of the agent.

        :param request: the debug request which should be en-/disabled
        :param enable:  True, if debug request should be enabled, False if it should be disabled
        :param type:    the type of the debug request ('cognition' or 'motion')
        :return:        Returns the id of the scheduled command
        """
        if type == 'cognition':
            return self.send_command(DebugCommand('Cognition:debugrequest:set', [(request, ('on' if enable else 'off'))]))
        elif type == 'motion':
            return self.send_command(DebugCommand('Motion:debugrequest:set', [(request, ('on' if enable else 'off'))]))
        else:
            #logging.warning('Unknown debug request type! Allowed: "cognition", "motion"')
            pass
        return 0

    def module(self, name:str, enable:bool, type:str='cognition'):
        """
        Enables/Disables a module of the agent instance.

        :param name:    the module which should be en-/disabled
        :param enable:  True, if module should be enabled, False if it should be disabled
        :param type:    the type of the module ('cognition' or 'motion')
        :return:        Returns the id of the scheduled command
        """
        if type == 'cognition':
            return self.send_command(DebugCommand('Cognition:modules:set', [(name, ('on' if enable else 'off'))]))
        elif type == 'motion':
            return self.send_command(DebugCommand('Motion:modules:set', [(name, ('on' if enable else 'off'))]))
        else:
            #logging.warning('Unknown module type! Allowed: "cognition", "motion"')
            pass
        return 0

    def representation(self, name:str, type:str='cognition', binary:bool=False):
        """
        Schedules a command for retrieving a representation.

        :param name:    the name of the representation which should be retrieved.
        :param type:    the type of the representation ('cognition' or 'motion')
        :param binary:  whether the result should be binary (protobuf) or as string
        :return:        Returns the id of the scheduled command
        """
        if type == 'cognition':
            if binary:
                return self.send_command(DebugCommand('Cognition:representation:get', [name]))
            else:
                return self.send_command(DebugCommand('Cognition:representation:print', [name]))
        elif type == 'motion':
            if binary:
                return self.send_command(DebugCommand('Motion:representation:get', [name]))
            else:
                return self.send_command(DebugCommand('Motion:representation:print', [name]))
        else:
            #logging.warning('Unknown representation type! Allowed: "cognition", "motion"')
            pass
        return 0

    def agent(self, name:str):
        """
        Selects an named agent for execution.

        :param name: the name of the agent (behavior), which should be executed
        :return:    Returns the id of the scheduled command
        """
        return self.send_command(DebugCommand('Cognition:behavior:set_agent', [('agent', name)]))

    def behavior(self, complete=False):
        """
        Schedules a command for retrieving the current behavior of the agent.

        :param complete: True, if the complete behavior tree should be retrieved, False otherwise (sparse)
        :return:    Returns the id of the scheduled command
        """
        if complete:
            return self.representation('BehaviorStateComplete', binary=True)
        else:
            return self.representation('BehaviorStateSparse', binary=True)


class AgentControllerThread(_thread.Thread):
    def __init__(self, host, port, start=True):
        super().__init__()
        self._host = host
        self._port = port

        self._socket = None
        self._thread = None
        self._running = _thread.Event()
        self._connected = _thread.Event()

        self._cmd_id = 1
        self._cmd_q = _queue.Queue()

        self._last_heart_beat = time.monotonic()

        if start: self.start()

    def stop(self):
        print('Stopping ...')
        self._running.clear()

    def run(self):
        self._running.set()
        print('Running', self.__class__.__name__)
        while self._running.is_set():
            if not self._socket:
                try:
                    self._socket = socket.create_connection((self._host, self._port))
                    #self._socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

                    self._connected.set()
                    print('Connected')
                    #print('Established connection to robot!')
                except Exception as e:
                    self._connected.clear()
                    time.sleep(1)
                    # Skip the rest, since we doesn't have a valid connection to work with
                    continue

            self._send_heart_beat()
            #self._poll_answers()
            #self._send_commands()
            #time.sleep(0.1)

        print('Closing socket ...')
        if self._socket:
            self._socket.setblocking(False)
            self._socket.settimeout(0.2)
            self._socket.recv(0)
            #self._socket.shutdown(socket.SHUT_RDWR)
            self._socket.close()
            #print(self._socket)
            self._socket = None
        self._connected.clear()

        print('Done.')

    def _send_heart_beat(self):
        """Send heart beat to agent."""
        if self._socket and self._last_heart_beat + 1 < time.monotonic():
            self._last_heart_beat = time.monotonic()
            self._socket.setblocking(True)
            self._socket.sendall(_struct.pack('!i', -1))
            print('.', end='')

    def _poll_answers(self):
        """Retrieves the answers of commands send to the agent."""
        if self._socket:
            try:
                self._socket.setblocking(False)
                id = _struct.unpack("<I", self._socket.recv(4))[0]
                if id > 0:
                    self._socket.setblocking(True)
                    length = _struct.unpack("<I", self._socket.recv(4))[0]
                    if length > 0:
                        # The command caller is responsible for retrieving the command result
                        # TODO: what happens/todo, if the result is never retrieved?!
                        data = self._socket.recv(length)

                        try:
                            print('Size', length)
                            print(data.decode('utf-8'))
                        except UnicodeDecodeError:
                            pass
                    else:
                        #logging.warning('Got invalid command data length! (%d)', id)
                        pass
                else:
                    #logging.warning('Got invalid command id!')
                    pass

            except BlockingIOError:
                # no data available - ignore
                pass

    def _send_commands(self):
        """Sends a command to the agent. The command is retrieved of the command queue."""
        self._socket.setblocking(True)
        while not self._cmd_q.empty():
            cmd = self._cmd_q.get()
            #logging.info('Send command %d: "%s"', cmd.id, cmd.get_name())
            self._socket.sendall(cmd.serialize(cmd.id))
            self._cmd_q.task_done()

    def is_connected(self):
        return self._connected.is_set()

    def send_command(self, cmd:DebugCommand):
        if self._connected.is_set():
            cmd.id = self._cmd_id
            self._cmd_q.put_nowait(cmd)
            self._cmd_id += 1
            return cmd.id

        raise Exception('Not connected to the agent!')

    def debugrequest(self, request:str, enable:bool, type:str='cognition'):
        """
        Enables/Disables a debug request of the agent.

        :param request: the debug request which should be en-/disabled
        :param enable:  True, if debug request should be enabled, False if it should be disabled
        :param type:    the type of the debug request ('cognition' or 'motion')
        :return:        Returns the id of the scheduled command
        """
        if type == 'cognition':
            return self.send_command(DebugCommand('Cognition:debugrequest:set', [(request, ('on' if enable else 'off'))]))
        elif type == 'motion':
            return self.send_command(DebugCommand('Motion:debugrequest:set', [(request, ('on' if enable else 'off'))]))
        else:
            #logging.warning('Unknown debug request type! Allowed: "cognition", "motion"')
            pass
        return 0

    def module(self, name:str, enable:bool, type:str='cognition'):
        """
        Enables/Disables a module of the agent instance.

        :param name:    the module which should be en-/disabled
        :param enable:  True, if module should be enabled, False if it should be disabled
        :param type:    the type of the module ('cognition' or 'motion')
        :return:        Returns the id of the scheduled command
        """
        if type == 'cognition':
            return self.send_command(DebugCommand('Cognition:modules:set', [(name, ('on' if enable else 'off'))]))
        elif type == 'motion':
            return self.send_command(DebugCommand('Motion:modules:set', [(name, ('on' if enable else 'off'))]))
        else:
            #logging.warning('Unknown module type! Allowed: "cognition", "motion"')
            pass
        return 0

    def representation(self, name:str, type:str='cognition', binary:bool=False):
        """
        Schedules a command for retrieving a representation.

        :param name:    the name of the representation which should be retrieved.
        :param type:    the type of the representation ('cognition' or 'motion')
        :param binary:  whether the result should be binary (protobuf) or as string
        :return:        Returns the id of the scheduled command
        """
        if type == 'cognition':
            if binary:
                return self.send_command(DebugCommand('Cognition:representation:get', [name]))
            else:
                return self.send_command(DebugCommand('Cognition:representation:print', [name]))
        elif type == 'motion':
            if binary:
                return self.send_command(DebugCommand('Motion:representation:get', [name]))
            else:
                return self.send_command(DebugCommand('Motion:representation:print', [name]))
        else:
            #logging.warning('Unknown representation type! Allowed: "cognition", "motion"')
            pass
        return 0

    def agent(self, name:str):
        """
        Selects an named agent for execution.

        :param name: the name of the agent (behavior), which should be executed
        :return:    Returns the id of the scheduled command
        """
        return self.send_command(DebugCommand('Cognition:behavior:set_agent', [('agent', name)]))

    def behavior(self, complete=False):
        """
        Schedules a command for retrieving the current behavior of the agent.

        :param complete: True, if the complete behavior tree should be retrieved, False otherwise (sparse)
        :return:    Returns the id of the scheduled command
        """
        if complete:
            return self.representation('BehaviorStateComplete', binary=True)
        else:
            return self.representation('BehaviorStateSparse', binary=True)


class AgentController(_thread.Thread):

    def __init__(self, host, port, start = True):
        super().__init__()

        self._host = host
        self._port = port

        self._stream_reader = None
        self._stream_writer = None

        self._tasks = []
        self._loop = _as.new_event_loop()

        self._cmd_id = 1
        self._cmd_q = _as.Queue(loop=self._loop)
        self._cmd_m = {}

        self._connected = _thread.Event()
        self._connected_internal = _as.Event(loop=self._loop)

        if start: self.start()

    def run(self):
        # set the event loop to this thread
        _as.set_event_loop(self._loop)

        # schedule tasks
        self._tasks.append(self._loop.create_task(self._connect(), name='Connection listener'))
        self._tasks.append(self._loop.create_task(self._send_heart_beat(), name='Heart beat'))
        self._tasks.append(self._loop.create_task(self._poll_answers(), name='Poll answers'))
        self._tasks.append(self._loop.create_task(self._send_commands(), name='Send commands'))

        # run tasks cooperatively and wait 'till loop is stopped
        self._loop.run_forever()

        self._set_connected(False)

    def stop(self, timeout=None):
        if self._loop.is_running():
            self._loop.call_soon_threadsafe(lambda: self._loop.create_task(self._stop_internal()))
            self.join(timeout)

    async def _stop_internal(self):
        for task in reversed(self._tasks):
            task.cancel()
            await task

        self._loop.stop()

    def is_connected(self):
        return self._connected.is_set()

    def wait_connected(self, timeout=None):
        self._connected.wait(timeout)

    def _set_connected(self, state: bool):
        if state:
            self._connected_internal.set()
            self._connected.set()
        else:
            self._connected.clear()
            self._connected_internal.clear()
            if self._stream_writer:
                self._stream_writer.close()

    async def _connect(self):
        while True:
            try:
                # (try to) establish connection or raise exception
                self._stream_reader, \
                self._stream_writer = await _as.open_connection(host=self._host, port=self._port)

                # update internal & external connection state
                self._set_connected(True)

                # wait 'till the connection is 'closed' (lost?)
                await self._stream_writer.wait_closed()

                # reset the streams
                self._stream_reader = None
                self._stream_writer = None
            except _as.CancelledError:
                break
            except Exception:
                # task can be cancelled while sleeping ...
                try:
                    # connection failed, wait before next connection attempt
                    await _as.sleep(1)
                except _as.CancelledError:
                    break
            finally:
                # empty queue and set exception – since we doesn't have a connection
                while not self._cmd_q.empty():
                    self._cmd_q.get_nowait().set_exception(Exception('Not connected to the agent!'))

        if self._stream_writer:
            self._stream_writer.close()
            #await self._stream_writer.wait_closed()  # NOTE: this doesn't finish?!

    async def _send_heart_beat(self):
        """Send a heart beat to the agent."""
        while True:
            try:
                await self._connected_internal.wait()

                self._stream_writer.write(_struct.pack('!i', -1))
                await self._stream_writer.drain()

                await _as.sleep(1)
            except _as.CancelledError:  # task cancelled
                break
            except OSError:  # connection lost
                self._set_connected(False)
            except Exception as e:  # unexpected exception
                print(e, file=sys.stderr)

    async def _poll_answers(self):
        # helper function to determine, if the connection was lost
        def lost_connection(d):
            if d == b'':
                self._set_connected(False)
                return True
            return False

        while True:
            try:
                await self._connected_internal.wait()

                raw_id = await self._stream_reader.read(4)
                if lost_connection(raw_id): continue
                cmd_id = _struct.unpack('=l', raw_id)[0]

                raw_size = await self._stream_reader.read(4)
                if lost_connection(raw_size): continue
                size = _struct.unpack('=l', raw_size)[0]

                raw_data = await self._stream_reader.read(size)
                if lost_connection(raw_data): continue

                if cmd_id in self._cmd_m:
                    cmd = self._cmd_m.pop(cmd_id)
                    if not cmd.cancelled():
                        cmd.set_result(raw_data)
                else:
                    print('Unknown command id:', cmd_id, file=sys.stderr)
            except _as.CancelledError:  # task cancelled
                break

    async def _send_commands(self):
        # helper function, if an exception occurred and the command couldn't be send
        def cancel_cmd(cmd, ex=None):
            self._cmd_m.pop(cmd.id)
            cmd.set_exception(ex if ex else Exception('Lost connection to the agent!'))

        while True:
            try:
                await self._connected_internal.wait()
                # get next command
                cmd = await self._cmd_q.get()
                # set command to running
                if cmd.set_running_or_notify_cancel():
                    # store command for later response
                    self._cmd_m[cmd.id] = cmd
                    try:
                        # send command
                        self._stream_writer.write(cmd.serialize(cmd.id))
                        await self._stream_writer.drain()
                    except _as.CancelledError:  # task cancelled
                        cancel_cmd(cmd)
                        break
                    except OSError:  # connection lost
                        self._set_connected(False)
                        cancel_cmd(cmd)
                    except Exception as e:  # unexpected exception
                        print(e, file=sys.stderr)
                        cancel_cmd(cmd, e)
                    finally:
                        self._cmd_q.task_done()  # mark as done
                else:
                    self._cmd_q.task_done()  # mark as done

            except _as.CancelledError:  # task cancelled
                break

    def send_command(self, cmd: DebugCommand):
        """Schedules the given command in the command queue and returns the command."""
        if self.is_connected():
            cmd.id = self._cmd_id

            self._cmd_q.put_nowait(cmd)
            self._cmd_id += 1
            return cmd

        raise Exception('Not connected to the agent!')

    def debugrequest(self, request: str, enable: bool, type: str = 'cognition'):
        """
        Enables/Disables a debug request of the agent.

        :param request: the debug request which should be en-/disabled
        :param enable:  True, if debug request should be enabled, False if it should be disabled
        :param type:    the type of the debug request ('cognition' or 'motion')
        :return:        Returns the the scheduled command (future)
        """
        if type == 'cognition':
            return self.send_command(DebugCommand('Cognition:debugrequest:set', [(request, ('on' if enable else 'off'))]))
        elif type == 'motion':
            return self.send_command(DebugCommand('Motion:debugrequest:set', [(request, ('on' if enable else 'off'))]))

        raise Exception('Unknown debug request type! Allowed: "cognition", "motion"')

    def module(self, name: str, enable: bool, type: str = 'cognition'):
        """
        Enables/Disables a module of the agent instance.

        :param name:    the module which should be en-/disabled
        :param enable:  True, if module should be enabled, False if it should be disabled
        :param type:    the type of the module ('cognition' or 'motion')
        :return:        Returns the the scheduled command (future)
        """
        if type == 'cognition':
            return self.send_command(DebugCommand('Cognition:modules:set', [(name, ('on' if enable else 'off'))]))
        elif type == 'motion':
            return self.send_command(DebugCommand('Motion:modules:set', [(name, ('on' if enable else 'off'))]))

        raise Exception('Unknown module type! Allowed: "cognition", "motion"')

    def representation(self, name: str, type: str = 'cognition', binary: bool = False):
        """
        Schedules a command for retrieving a representation.

        :param name:    the name of the representation which should be retrieved.
        :param type:    the type of the representation ('cognition' or 'motion')
        :param binary:  whether the result should be binary (protobuf) or as string
        :return:        Returns the the scheduled command (future)
        """
        if type == 'cognition':
            if binary:
                return self.send_command(DebugCommand('Cognition:representation:get', [name]))
            else:
                return self.send_command(DebugCommand('Cognition:representation:print', [name]))
        elif type == 'motion':
            if binary:
                return self.send_command(DebugCommand('Motion:representation:get', [name]))
            else:
                return self.send_command(DebugCommand('Motion:representation:print', [name]))

        raise Exception('Unknown representation type! Allowed: "cognition", "motion"')

    def agent(self, name: str):
        """
        Selects an named agent for execution.

        :param name:    the name of the agent (behavior), which should be executed
        :return:        Returns the the scheduled command (future)
        """
        return self.send_command(DebugCommand('Cognition:behavior:set_agent', [('agent', name)]))

    def behavior(self, complete=False):
        """
        Schedules a command for retrieving the current behavior of the agent.

        :param complete: True, if the complete behavior tree should be retrieved, False otherwise (sparse)
        :return:        Returns the the scheduled command (future)
        """
        if complete:
            return self.representation('BehaviorStateComplete', binary=True)
        else:
            return self.representation('BehaviorStateSparse', binary=True)


