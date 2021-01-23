import sys
import struct
import asyncio
import threading
import functools
from concurrent.futures import Future

from .. import pb

__all__ = ['DebugProxy', 'DebugCommand', 'AgentController']


class DebugProxy(threading.Thread):
    """
    Debugging class, creating a connection between RobotControl and the Nao (naoth agent) which can be used to

    * print out the communication between the naoth agent and the controlling instance (eg. RobotControl) -- set
      :param:`print_cmd` to True
    * create a connection which never 'dies'; even when the naoth agent restarts/dies, the connection to the controlling
      instance is kept
    * accepting multiple connections of controlling instances to the naoth agent, allowing -- for example -- to connect
      two RobotControl instances to the robot, each command is distributed accordingly

    A extra module is also available in order to start the proxy directly from the command line::

        python -m naoth.utils.DebugProxy host [port] [--target port] [--print]

    It is also possible to instantiate the proxy in the interactive shell or another python script to get slightly more
    control over when the proxy ist started and stopped::
        >>> import naoth
        >>> p = naoth.utils.DebugProxy('localhost', print_cmd=True, start=False)
        >>> # ...
        >>> p.start()
        >>> # ...
        >>> p.stop()

    """

    def __init__(self, agent_host, agent_port=5401, dest_port=7777, print_cmd=False, start=True):
        """
        Initializes the class variables and starts the thread immediately if :param:`start` is set to True (default).

        :param agent_host: the host name or ip address of naoth agent (robot, dummysimulator, ...); eg. "localhost"
        :param agent_port: the debug port of the naoth agent; default is 5401
        :param dest_port:  the port which should be opened to allow controlling applications to connect to (eg. RC)
        :param print_cmd:  set to True to print out all commands and responses going through the proxy.
        :param start:      whether the thread should start immediately (default) or not
        """
        super().__init__()

        # the agent thread is only started, if there's at least one connected host
        self._robot = None
        self._robot_host = agent_host
        self._robot_port = agent_port

        self._host_host = 'localhost'
        self._host_port = dest_port

        self._print = print_cmd
        self._loop = asyncio.new_event_loop()

        self._hosts = []
        self._host_listener = None
        self._host_connection_cnt = 0

        # start thread immediately
        if start: self.start()

    def run(self) -> None:
        """
        The thread main loop.
        Sets the asyncio loop of the thread, starts a listener server on the dest_port and runs until cancelled.
        """

        # set the event loop to this thread
        asyncio.set_event_loop(self._loop)
        # start host listener server and 'wait' until the server ist started
        self._host_listener = self._loop.run_until_complete(start_server(self._host, self._host_host, self._host_port))
        # run until cancelled
        self._loop.run_forever()

    def stop(self, timeout=None) -> None:
        """
        Stops the (running) thread and blocks until finished or until the optional timeout occurs.

        Since this method is called from another thread, the stop request is scheduled as task on the main loop and
        executed some ms later. Also `join` is called on the thread to wait until the thread is actually finished.

        The timeout can be used to make sure the main program continues, if an error prevents terminating this
        thread -- though that shouldn't happen.
        """
        if self._loop.is_running():
            self._loop.call_soon_threadsafe(lambda: self._loop.create_task(self._stop_internal()))
            self.join(timeout)

    async def _stop_internal(self) -> None:
        """
        The (internal) scheduled stop request task called by `stop()`.

        It stops the listener server, closes all open connections and stops the main loop, which causes the thread to
        finish.
        """

        # shutdown host listener server to prevent new connections
        if self._host_listener:
            self._host_listener.close()
            await self._host_listener.wait_closed()

        # cancel all established connections
        # NOTE: the connection to the agent is stopped with the last host connection
        for task in self._hosts:
            task.cancel()
            await task

        self._loop.stop()

    def _register_host(self) -> None:
        """
        Registers a new host connection and sets the name of the task.

        The connection to the naoth instance is started only, if there is a host connected to the proxy. This prevents
        blocking the naoth instance unnecessarily from direct connection, when the proxy doesn't have something to do.
        """
        self._host_connection_cnt += 1
        #asyncio.Task.current_task().set_name('Host-{}'.format(self._host_connection_cnt))  # 3.8+
        self._hosts.append(current_task())

        if self._robot is None:
            self._robot = AgentController(self._robot_host, self._robot_port)
            self._robot.wait_connected()  # TODO: is this reasonable???

    def _unregister_host(self) -> None:
        """
        Unregisters the host from the proxy.

        if there are no other active host connections, the naoth agent controller/connection is stopped -- to prevent
        blocking unused resources (naoth instance).
        """
        self._hosts.remove(current_task())
        if len(self._hosts) == 0:
            if self._robot is not None:
                self._robot.stop()
                self._robot = None

    async def _host(self, stream_reader, stream_writer) -> None:
        """
        The actual task, which handles the host connection to the proxy.

        Therefore is reads all debug commands send from the host and relays it to the connected naoth instance. The
        response is returned to the host.

        If required, the received command and the response if printed out to the terminal.

        The task runs as long as the connection to the host is active or until the proxy thread is stopped.
        """
        self._register_host()

        while True:
            try:
                raw_id = await stream_reader.read(4)
                # connection is closed/lost
                if raw_id == b'': break

                cmd_id = struct.unpack('=l', raw_id)[0]

                # check if command is not just a heart beat
                if cmd_id != -1:
                    raw_length = await stream_reader.read(4)
                    cmd_length = struct.unpack('=l', raw_length)[0]

                    raw_data = await stream_reader.read(cmd_length)
                    cmd = DebugCommand.deserialize(raw_data)
                    cmd.id = cmd_id

                    # NOTE: the callback is executed in the agent thread!
                    cmd.add_done_callback(functools.partial(self._response_handler, stream_writer))

                    self._robot.send_command(cmd)

                    if self._print:
                        print(cmd)

            except asyncio.CancelledError:  # task cancelled
                break
            except Exception as e:
                print('Host-Task:', e)

        # close the connection to the host before exiting
        stream_writer.close()
        await stream_writer._protocol._get_close_waiter(stream_writer)  # HACK: in order to work with < 3.7

        self._unregister_host()

    def _response_handler(self, stream, cmd) -> None:
        """
        Helper method in order to transfer the command from agent thread back to 'this' thread -- since the callback is
        called in the agent thread. This can 'causes a delay of ~0.5ms.
        """
        self._loop.call_soon_threadsafe(lambda: self._response_writer(stream, cmd))

    def _response_writer(self, stream, cmd) -> None:
        """Writes the response of the command back to the requesting host."""
        # TODO: what to todo, if the command got cancelled?!?
        if stream and not cmd.cancelled():
            stream.write(struct.pack("<I", cmd.id) + struct.pack("<I", len(cmd.result())) + cmd.result())

            if self._print:
                print(cmd)


class DebugCommand(Future):
    """
    Class representing a debug command for a naoth agent.

    It is a Future and can be waited for the response.
    """

    def __init__(self, name: str, args=None):
        """
        Constructor for the command.

        :param name:    the name of the command
        :param args:    additional argument(s) of the command as string or list of string/tuples (name and value)
        """
        super().__init__()
        self._id = 0
        self._name = name
        self._args = []
        # args can be a string, tuple or a list
        if isinstance(args, (str, tuple)):
            self._args.append(args)
        elif isinstance(args, list):
            self._args.extend(args)

    @property
    def id(self) -> int:
        """Returns command id."""
        return self._id

    @id.setter
    def id(self, value: int) -> None:
        """Sets the command id."""
        self._id = value

    @property
    def name(self) -> str:
        """Returns the name of this command."""
        return self._name

    def add_arg(self, arg) -> None:
        """
        Adds an argument to this command.

        :param arg: this can be a simple string or a tuple of two strings (argument name and value).
        """
        self._args.append(arg)

    def serialize(self) -> bytes:
        """
        Serializes the command to a byte representation in order to send it to the agent.

        :return: returns the bytes representation of this command
        """
        cmd_args = []
        if self._args:
            for a in self._args:
                if isinstance(a, str):
                    cmd_args.append(pb.Messages_pb2.CMDArg(name=a))
                else:
                    cmd_args.append(pb.Messages_pb2.CMDArg(name=a[0], bytes=a[1].encode()))

        proto = pb.Messages_pb2.CMD(name=self.name, args=cmd_args)
        return struct.pack("<I", self.id) + struct.pack("<I", proto.ByteSize()) + proto.SerializeToString()

    @staticmethod
    def deserialize(data) -> 'DebugCommand':
        """
        Parses the given data and returns an instance of DebugCommand.

        :param data: byte string of a serialized debug command
        """
        proto = pb.Messages_pb2.CMD()
        proto.ParseFromString(data)

        return DebugCommand(proto.name, [(arg.name, arg.bytes.decode()) for arg in proto.args])

    def __str__(self) -> str:
        """Returns the string representation of this command."""
        str_builder = [self.__class__.__name__, '-', str(self.id), ' [', self._state, ']: ', self.name]
        str_args = ", ".join(map(lambda a: self._str_args_helper(a), self._args))
        if str_args:
            str_builder.append(' ( ')
            str_builder.append(str_args)
            str_builder.append(' )')
        if self.done() and len(self.result()) > 0:
            str_builder.append(' {\n')
            try:
                str_builder.append(self.result().decode('utf-8').strip())
            except:
                str_builder.append(str(self.result()))
            str_builder.append('\n}')

        return ''.join(str_builder)

    @staticmethod
    def _str_args_helper(arg) -> str:
        """Helper method to format the command arguments for the `__str__` method."""
        if isinstance(arg, str):
            return arg
        return arg[0] + ('' if len(arg[1]) == 0 else ': ' + repr(arg[1]))


class AgentController(threading.Thread):
    """
    Class to establish a connection to a naoth agent and sending DebugRequests to it - like doing it via RobotControl.

    An instance can be created in an interactive shell or script and sending debug requests to the robot:

        >>> import naoth, time
        >>> a = naoth.utils.AgentController('localhost', 5401)
        >>> a.wait_connected()
        >>> a.representation('PlayerInfo').add_done_callback(print)

        >>> c = a.agent('soccer_agent')
        >>> c.add_done_callback(print)

        >>> c = a.debugrequest('gamecontroller:blow_whistle', True)  # debug request for cognition
        >>> r = c.result()  # blocks until result is available

        >>> a.debugrequest('Plot:Motion.Cycle', True, 'motion')  # debug request for motion
        >>> a.debugrequests([ \
                ('gamecontroller:gamephase:normal', True), \
                ('gamecontroller:game_state:penalized', True), \
                ('gamecontroller:set_play:pushing_free_kick', True), \
                ('gamecontroller:secondaryde:30', True) \
            ])

        >>> a.behavior()  # BehaviorStateSparse
        >>> a.behavior(True)  # BehaviorStateComplete

        >>> a.module('FakeBallDetector', True)
        >>> a.module('ArmCollisionDetector2018', True, 'motion')

        >>> a.send_command(naoth.utils.DebugCommand('help', 'ping')).add_done_callback(print)

        >>> c = naoth.utils.DebugCommand('Cognition:representation:list')
        >>> a.send_command(c)
        >>> c.result()  # blocks until result is available

        >>> a.stop()  # stop the agent gracefully
    """

    def __init__(self, host, port=5401, start=True):
        """
        Initializes the class variables and starts the thread immediately if :param:`start` is set to True (default).

        :param host:  the host name or ip address of naoth agent (robot, dummysimulator, ...); eg. "localhost"
        :param port:  the debug port of the naoth agent; default is 5401
        :param start: whether the thread should start immediately (default) or not
        """
        super().__init__()

        self._host = host
        self._port = port

        self._stream_reader = None
        self._stream_writer = None

        self._tasks = []
        self._loop = asyncio.new_event_loop()

        self._cmd_id = 1
        self._cmd_q = asyncio.Queue(loop=self._loop)
        self._cmd_m = {}

        self._connected = threading.Event()
        self._connected_internal = asyncio.Event(loop=self._loop)

        # start thread immediately
        if start: self.start()

    def run(self) -> None:
        """
        The thread main loop.

        Sets the asyncio loop of the thread, starts all necessary tasks and runs until cancelled.
        """

        # set the event loop to this thread
        asyncio.set_event_loop(self._loop)

        # schedule tasks
        self._tasks.append(self._loop.create_task(self._connect()))  # name='Connection listener'
        self._tasks.append(self._loop.create_task(self._send_heart_beat()))  # name='Heart beat'
        self._tasks.append(self._loop.create_task(self._poll_answers()))  # name='Poll answers'
        self._tasks.append(self._loop.create_task(self._send_commands()))  # name='Send commands'

        # run tasks cooperatively and wait 'till loop is stopped
        self._loop.run_forever()

        self._set_connected(False)

    def stop(self, timeout=None) -> None:
        """
        Stops the (running) thread and blocks until finished or until the optional timeout occurs.

        Since this method is called from another thread, the stop request is scheduled as task on the main loop and
        executed some ms later. Also `join` is called on the thread to wait until the thread is actually finished.

        The timeout can be used to make sure the main program continues, if an error prevents terminating this
        thread -- though that shouldn't happen.
        """
        if self._loop.is_running():
            self._loop.call_soon_threadsafe(lambda: self._loop.create_task(self._stop_internal()))
            self.join(timeout)

    async def _stop_internal(self) -> None:
        """
        The (internal) scheduled stop request task called by `stop()`.

        It stops all scheduled tasks in reverse order and stops the main loop, which causes the thread to finish.
        """
        for task in reversed(self._tasks):
            try:
                task.cancel()
                await task
            except Exception as e:
                print('Stop agent:', task.get_name() if hasattr(task, 'get_name') else task, e, file=sys.stderr)

        self._loop.stop()

    def is_connected(self) -> bool:
        """Returns True, if the thread is connected to the naoth instance, False otherwise."""
        return self._connected.is_set()

    def _assert_is_alive(self):
        """Asserts, that this thread is alive, otherwise a runtime error is raised."""
        if not self.is_alive():
            raise RuntimeError(self.__class__.__name__ + " must be alive and running!")

    def wait_connected(self, timeout=None) -> None:
        """
        Blocks until the thread is connected to the naoth agent or until the optional timeout occurs.
        If the thread wasn't started or isn't alive anymore, a runtime error is raised.
        """
        self._assert_is_alive()
        self._connected.wait(timeout)

    def _set_connected(self, state: bool) -> None:
        """Internal helper method to handle the connection state."""
        if state:
            self._connected_internal.set()
            self._connected.set()
        else:
            self._connected.clear()
            self._connected_internal.clear()
            if self._stream_writer:
                self._stream_writer.close()

    async def _connect(self) -> None:
        """Connection task, which is used to (re-)establish the connection to the naoth agent."""
        while True:
            try:
                # (try to) establish connection or raise exception
                self._stream_reader, \
                self._stream_writer = await open_connection(host=self._host, port=self._port)

                # update internal & external connection state
                self._set_connected(True)

                # wait 'till the connection is 'closed' (lost?)
                await self._stream_writer._protocol._get_close_waiter(self._stream_writer)  # HACK: in order to work with < 3.7

                # reset the streams
                self._stream_reader = None
                self._stream_writer = None
            except asyncio.CancelledError:
                break
            except OSError:
                # task can be cancelled while sleeping ...
                try:
                    # connection failed, wait before next connection attempt
                    await asyncio.sleep(1)
                except asyncio.CancelledError:
                    break
                except Exception as e:  # unexpected exception
                    print('Connection listener:', e, file=sys.stderr)
            except Exception as e:  # unexpected exception
                print('Connection listener:', e, file=sys.stderr)
            finally:
                # empty queue and set exception – since we doesn't have a connection
                while not self._cmd_q.empty():
                    self._cmd_q.get_nowait().set_exception(Exception('Not connected to the agent!'))
                # cancels the already scheduled commands, since we do not get a response after a lost connection!
                _ids = list(self._cmd_m.keys())  # copy the scheduled ids - prevent modification during iteration!
                for _id in _ids:
                    self._cancel_cmd(_id)

        if self._stream_writer:
            self._stream_writer.close()
            #await self._stream_writer.wait_closed()  # NOTE: this doesn't complete?!
            #await self._stream_writer._protocol._get_close_waiter(self._stream_writer)  # HACK: in order to work with < 3.7

    async def _send_heart_beat(self) -> None:
        """Task to regularly (1s) send a heart beat to the agent."""
        while True:
            try:
                await self._connected_internal.wait()

                self._stream_writer.write(struct.pack('!i', -1))
                await self._stream_writer.drain()

                await asyncio.sleep(1)
            except asyncio.CancelledError:  # task cancelled
                break
            except OSError:  # connection lost
                self._set_connected(False)
            except Exception as e:  # unexpected exception
                print('Heart beat:', e, file=sys.stderr)

    async def _poll_answers(self) -> None:
        """Task to receive the response of a previous command and set the result to that command."""

        def lost_connection(d):
            """Helper function to determine, if the connection was lost."""
            if d == b'':
                self._set_connected(False)
                return True
            return False

        while True:
            try:
                await self._connected_internal.wait()

                raw_id = await self._stream_reader.read(4)
                if lost_connection(raw_id): continue
                cmd_id = struct.unpack('=l', raw_id)[0]

                raw_size = await self._stream_reader.read(4)
                if lost_connection(raw_size): continue
                size = struct.unpack('=l', raw_size)[0]

                raw_data = await self._stream_reader.read(size)
                if size > 0 and lost_connection(raw_data): continue

                while len(raw_data) < size:
                    new_data = await self._stream_reader.read(size - len(raw_data))
                    if lost_connection(new_data):
                        break
                    raw_data += new_data

                if not self._connected.is_set():
                    continue

                if cmd_id in self._cmd_m:
                    cmd, _id = self._cmd_m.pop(cmd_id)
                    if not cmd.cancelled():
                        cmd.id = _id
                        cmd.set_result(raw_data)
                else:
                    print('Unknown command id:', cmd_id, file=sys.stderr)
            except asyncio.CancelledError:  # task cancelled
                break
            except OSError:  # connection lost
                self._set_connected(False)
            except Exception as e:  # unexpected exception
                print('Poll answers:', e, file=sys.stderr)

    async def _send_commands(self) -> None:
        """Task to send scheduled commands."""

        while True:
            try:
                await self._connected_internal.wait()
                # get next command
                cmd = await self._cmd_q.get()
                # set command to running
                if cmd.set_running_or_notify_cancel():
                    self._store_cmd(cmd)
                    try:
                        # send command
                        self._stream_writer.write(cmd.serialize())
                        await self._stream_writer.drain()
                    except asyncio.CancelledError:  # task cancelled
                        _cancel_cmd(cmd.id)
                        break
                    except OSError:  # connection lost
                        self._set_connected(False)
                        _cancel_cmd(cmd.id)
                    except Exception as e:  # unexpected exception
                        print('Send commands:', e, file=sys.stderr)
                        _cancel_cmd(cmd.id, e)
                    finally:
                        self._cmd_q.task_done()  # mark as done
                else:
                    self._cmd_q.task_done()  # mark as done

            except asyncio.CancelledError:  # task cancelled
                break
            except OSError:  # connection lost
                self._set_connected(False)
            except Exception as e:  # unexpected exception
                print('Send commands:', e, file=sys.stderr)

    def _store_cmd(self, cmd) -> None:
        """Replaces the command id with an internal id and store command+id for later response."""
        self._cmd_m[self._cmd_id] = (cmd, cmd.id)
        cmd.id = self._cmd_id
        self._cmd_id += 1

    def _cancel_cmd(self, cmd_id, ex=None):
        """Helper function, if an exception occurred and the command couldn't be send or a send command must be canceled."""
        cmd, _id = self._cmd_m.pop(cmd_id)
        cmd.set_exception(ex if ex else Exception('Lost connection to the agent!'))
        cmd.id = _id  # replace internal id with the original

    def send_command(self, cmd: DebugCommand) -> DebugCommand:
        """
        Schedules the given command in the command queue and returns the command.

        :raises Exception: if not connected to a naoth agent or the given command was already executed
        """
        if not self.is_connected():
            raise Exception('Not connected to the agent!')

        if cmd.done():
            raise Exception('This command has already been executed!')

        # command queue is not thread safe - make sure we're add it in the correct thread
        # this can 'causes a delay of ~0.5ms
        self._loop.call_soon_threadsafe(functools.partial(self._cmd_q.put_nowait, cmd))
        return cmd

    def debugrequest(self, request: str, enable: bool, type: str = 'cognition') -> DebugCommand:
        """
        Enables/Disables a debug request of the agent.

        :param request: the debug request which should be en-/disabled
        :param enable:  True, if debug request should be enabled, False if it should be disabled
        :param type:    the type of the debug request ('cognition' or 'motion')
        :return:        Returns the the scheduled command (future)
        """
        return self.debugrequests([(request, enable)], type)

    def debugrequests(self, requests: list, type: str = 'cognition') -> DebugCommand:
        """
        Enables/Disables a list of debug request of the agent.

        :param requests: a list of tuples ('debug request', True|False) of debug requests which should be en-/disabled
        :param type:    the type of the debug request ('cognition' or 'motion')
        :return:        Returns the the scheduled command (future)
        """
        dbg = pb.Messages_pb2.DebugRequest(requests=[
            pb.Messages_pb2.DebugRequest.Item(name=request, value=enable) for request, enable in requests
        ])
        if type == 'cognition':
            return self.send_command(DebugCommand('Cognition:representation:set', [('DebugRequest', dbg.SerializeToString().decode())]))
        elif type == 'motion':
            return self.send_command(DebugCommand('Motion:representation:set', [('DebugRequest', dbg.SerializeToString().decode())]))

        raise Exception('Unknown debug request type! Allowed: "cognition", "motion"')

    def module(self, name: str, enable: bool, type: str = 'cognition') -> DebugCommand:
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

    def representation(self, name: str, type: str = 'cognition', binary: bool = False) -> DebugCommand:
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

    def agent(self, name: str = None) -> DebugCommand:
        """
        Get or set a named agent for execution.

        :param name:    the name of the agent (behavior), which should be executed or None if the current agent should
                        be returned
        :return:        Returns the the scheduled command (future)
        """
        if name is None:
            return self.send_command(DebugCommand('Cognition:behavior:get_agent'))

        return self.send_command(DebugCommand('Cognition:behavior:set_agent', [('agent', name)]))

    def behavior(self, complete=False) -> DebugCommand:
        """
        Schedules a command for retrieving the current behavior of the agent.

        :param complete: True, if the complete behavior tree should be retrieved, False otherwise (sparse)
        :return:        Returns the the scheduled command (future)
        """
        if complete:
            return self.representation('BehaviorStateComplete', binary=True)
        else:
            return self.representation('BehaviorStateSparse', binary=True)


if sys.version_info < (3, 7):
    # python < 3.7
    current_task = asyncio.Task.current_task

    @asyncio.coroutine
    def open_connection(host=None, port=None, loop=None):

        if loop is None:
            loop = asyncio.get_event_loop()

        reader = asyncio.StreamReader()
        protocol = StreamReaderProtocolCompat(reader)
        transport, _ = yield from loop.create_connection(lambda: protocol, host, port)
        writer = asyncio.StreamWriter(transport, protocol, reader, loop)
        return reader, writer


    @asyncio.coroutine
    def start_server(client_connected_cb, host=None, port=None, loop=None):

        if loop is None:
            loop = asyncio.get_event_loop()

        def factory():
            reader = asyncio.StreamReader(loop=loop)
            protocol = StreamReaderProtocolCompat(reader, client_connected_cb, loop=loop)
            return protocol

        return (yield from loop.create_server(factory, host, port))


    class StreamReaderProtocolCompat(asyncio.StreamReaderProtocol):
        def __init__(self, stream_reader, client_connected_cb=None, loop=None):
            super().__init__(stream_reader, client_connected_cb, loop)
            self._closed = self._loop.create_future()

        def connection_lost(self, exc) -> None:
            super().connection_lost(exc)

            if not self._closed.done():
                if exc is None:
                    self._closed.set_result(None)
                else:
                    self._closed.set_exception(exc)

        def _get_close_waiter(self, stream):
            return self._closed

        def __del__(self):
            if self._closed.done() and not self._closed.cancelled():
                self._closed.exception()
else:
    # python >= 3.7
    open_connection = asyncio.open_connection
    start_server = asyncio.start_server
    current_task = asyncio.current_task