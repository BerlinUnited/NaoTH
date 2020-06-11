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
    Debugging class, creating a connection between RobotControlGui and the Nao to inspect messages and the control flow.
    """

    def __init__(self, agent_host, agent_port=5401, dest_port=7777, print_cmd=False, start=True):
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

    def run(self):
        # set the event loop to this thread
        asyncio.set_event_loop(self._loop)
        # start host listener server and 'wait' until the server ist started
        self._host_listener = self._loop.run_until_complete(asyncio.start_server(self._host, self._host_host, self._host_port))
        # run until cancelled
        self._loop.run_forever()

    def stop(self, timeout=None):
        if self._loop.is_running():
            self._loop.call_soon_threadsafe(lambda: self._loop.create_task(self._stop_internal()))
            self.join(timeout)

    async def _stop_internal(self):
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

    def _register_host(self):
        self._host_connection_cnt += 1
        asyncio.Task.current_task().set_name('Host-{}'.format(self._host_connection_cnt))
        self._hosts.append(asyncio.Task.current_task())

        if self._robot is None:
            self._robot = AgentController(self._robot_host, self._robot_port)
            self._robot.wait_connected()  # TODO: is this reasonable???

    def _unregister_host(self):
        self._hosts.remove(asyncio.Task.current_task())
        if len(self._hosts) == 0:
            if self._robot is not None:
                self._robot.stop()
                self._robot = None

    async def _host(self, stream_reader, stream_writer):
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
                print(asyncio.Task.current_task().get_name(), ':', e)

        # close the connection to the host before exiting
        stream_writer.close()
        await stream_writer.wait_closed()

        self._unregister_host()

    def _response_handler(self, stream, cmd):
        # transfer command from agent thread back to 'this' thread
        # this can 'causes a delay of ~0.5ms
        self._loop.call_soon_threadsafe(lambda: self._response_writer(stream, cmd))

    def _response_writer(self, stream, cmd):
        # TODO: what to todo, if the command got cancelled?!?
        if not cmd.cancelled():
            stream.write(struct.pack("<I", cmd.id) + struct.pack("<I", len(cmd.result())) + cmd.result())

            if self._print:
                print(cmd)


class DebugCommand(Future):
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
        self._args = args if args else []

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

    def serialize(self):
        """
        Serializes the command to a byte representation in order to send it to the agent.

        :return:        returns the bytes representation of this command
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
    def deserialize(data):
        proto = pb.Messages_pb2.CMD()
        proto.ParseFromString(data)

        return DebugCommand(proto.name, [(arg.name, arg.bytes.decode()) for arg in proto.args])

    def __str__(self):
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
    def _str_args_helper(arg):
        if isinstance(arg, str):
            return arg
        return arg[0] + ('' if len(arg[1]) == 0 else ': ' + repr(arg[1]))


class AgentController(threading.Thread):

    def __init__(self, host, port, start = True):
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

    def run(self):
        # set the event loop to this thread
        asyncio.set_event_loop(self._loop)

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
                self._stream_writer = await asyncio.open_connection(host=self._host, port=self._port)

                # update internal & external connection state
                self._set_connected(True)

                # wait 'till the connection is 'closed' (lost?)
                await self._stream_writer.wait_closed()

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
                    print(asyncio.Task.current_task().get_name(), ':', e, file=sys.stderr)
            except Exception as e:  # unexpected exception
                print(asyncio.Task.current_task().get_name(), ':', e, file=sys.stderr)
            finally:
                # empty queue and set exception – since we doesn't have a connection
                while not self._cmd_q.empty():
                    self._cmd_q.get_nowait().set_exception(Exception('Not connected to the agent!'))

        if self._stream_writer:
            self._stream_writer.close()
            #await self._stream_writer.wait_closed()  # NOTE: this doesn't complete?!

    async def _send_heart_beat(self):
        """Send a heart beat to the agent."""
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
                print(asyncio.Task.current_task().get_name(), ':', e, file=sys.stderr)

    async def _poll_answers(self):

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

                if cmd_id in self._cmd_m:
                    cmd, _id = self._cmd_m.pop(cmd_id)
                    if not cmd.cancelled():
                        cmd.id = _id
                        cmd.set_result(raw_data)
                else:
                    print('Unknown command id:', cmd_id, file=sys.stderr)
            except asyncio.CancelledError:  # task cancelled
                break
            except Exception as e:
                print(asyncio.Task.current_task().get_name(), ':', e, file=sys.stderr)

    async def _send_commands(self):

        def cancel_cmd(cmd, ex=None):
            """Helper function, if an exception occurred and the command couldn't be send."""
            _, _id = self._cmd_m.pop(cmd.id)
            cmd.set_exception(ex if ex else Exception('Lost connection to the agent!'))
            cmd.id = _id  # replace internal id with the original

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
                        cancel_cmd(cmd)
                        break
                    except OSError:  # connection lost
                        self._set_connected(False)
                        cancel_cmd(cmd)
                    except Exception as e:  # unexpected exception
                        print(asyncio.Task.current_task().get_name(), ':', e, file=sys.stderr)
                        cancel_cmd(cmd, e)
                    finally:
                        self._cmd_q.task_done()  # mark as done
                else:
                    self._cmd_q.task_done()  # mark as done

            except asyncio.CancelledError:  # task cancelled
                break
            except Exception as e:  # unexpected exception
                print(asyncio.Task.current_task().get_name(), ':', e, file=sys.stderr)

    def _store_cmd(self, cmd):
        """Replaces the command id with an internal id and store command+id for later response."""
        self._cmd_m[self._cmd_id] = (cmd, cmd.id)
        cmd.id = self._cmd_id
        self._cmd_id += 1

    def send_command(self, cmd: DebugCommand):
        """Schedules the given command in the command queue and returns the command."""
        if self.is_connected():
            # command queue is not thread safe - make sure we're add it in the correct thread
            # this can 'causes a delay of ~0.5ms
            self._loop.call_soon_threadsafe(functools.partial(self._cmd_q.put_nowait, cmd))
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
            return self.send_command(DebugCommand('Cognition:representation:set', [(request, ('on' if enable else 'off'))]))
        elif type == 'motion':
            return self.send_command(DebugCommand('Motion:representation:set', [(request, ('on' if enable else 'off'))]))

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
