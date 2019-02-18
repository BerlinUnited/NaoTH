import ctypes
import logging
import multiprocessing
import os
import re
import socket
import struct
import subprocess
import time
import signal

from naoth import Messages_pb2

class UnableToConnect(Exception):
    pass

class Command:
    """Class representing a command for a simspark agent."""

    def __init__(self, name, args=None):
        """
        Constructor for the command.

        :param name:    the name of the command
        :param args:    additional arguments of the command
        """
        self.id = 0
        self.name = name
        self.args = args

    def serialize(self, cmd_id):
        """
        Serializes the command to a byte representation in order to send it to the agent.

        :param cmd_id:  the id for identifying the command
        :return:        returns the bytes representation of this command
        """
        cmd_args = []
        if self.args:
            for a in self.args:
                if isinstance(a, str):
                    cmd_args.append(Messages_pb2.CMDArg(name=a))
                else:
                    cmd_args.append(Messages_pb2.CMDArg(name=a[0], bytes=a[1].encode()))

        proto = Messages_pb2.CMD(name=self.name, args=cmd_args)
        return struct.pack("<I", cmd_id) + struct.pack("<I", proto.ByteSize()) + proto.SerializeToString()

    def get_name(self):
        """Returns the name of this command."""
        return self.name

    def add_arg(self, arg):
        """Adds an argument to this command."""
        self.args.append(arg)


class AgentController(multiprocessing.Process):
    """Represents the controlling instance of a simspark agent. It serves two purposes, first its starts the agent
    application and second it sends debug and other requests to the instance."""

    def __init__(self, app, cwd, team=None, number=None, sync=True, start_instance=True, connect=True):
        """Constructor of the :AgentController:. Initializes public and private attributes."""
        super().__init__()

        self.app = os.path.abspath(app) if os.path.isfile(app) else app
        self.cwd = cwd
        self.team = team
        self.number = number
        self.sync = sync
        self.port = None
        self.connect_dbg = connect

        self.__p = None
        self.__m = multiprocessing.Manager()
        self.__start_instance = start_instance
        self.__socket = None
        self.__cmd_q = multiprocessing.Queue()
        self.__cmd_id = self.__m.Value(ctypes.c_long, 1)
        self.__cmd_result = self.__m.dict()

        self.__cancel = multiprocessing.Event()
        self.__connected = multiprocessing.Event()
        self.started = multiprocessing.Event()

    def terminate(self):
        """If this process instance gets killed, this is the last chance to kill the agent instance too."""
        self.__stop_agent()
        super(AgentController, self).terminate()

    def __start_agent(self):
        """Starts a agent instance as separate process and waits until it is completely started."""
        if self.__start_instance:
            logging.info('Starting agent')

            args = [self.app]
            if self.sync: args.append('--sync')
            if self.number: args.extend(['-n', str(self.number)])
            if self.team: args.extend(['--team', self.team])

            self.__p = subprocess.Popen(args, cwd=self.cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            logging.debug(' '.join(self.__p.args))
            self.__socket = None

            while True:
                # TODO: retrieve some informations about the agent (Playernumber, Port, ...
                o = self.__p.stdout.readline().decode()
                #[DebugServer:port 5402] is ready for incoming connections.
                if o.startswith('SimSpark Controller runs') or len(o) == 0:
                    break
                elif o.startswith('[DebugServer:port'): # re.match('\[DebugServer:port (\d+)\]', o):
                    self.port = re.match('\[DebugServer:port (\d+)\]', o).group(1)
            self.started.set()
            logging.info('Agent started')
        else:
            logging.info('No instance of agent started!')
            self.port = 5400 + self.number
            self.started.set()

    def __stop_agent(self):
        """Stops a still active agent instance."""
        if self.__start_instance and self.__p.poll() is None:
            logging.info("Quit agent application")
            self.__p.send_signal(signal.SIGHUP)
            time.sleep(0.5)
            # still running?
            if self.__p.poll() is None:
                logging.info("Stop agent application")
                self.__p.terminate()
                time.sleep(0.5)
                # still running?
                if self.__p.poll() is None:
                    logging.info("Kill agent application")
                    self.__p.kill()
                    time.sleep(0.5)

    def cancel(self):
        """Sets the canceled flag of this process."""
        self.__cancel.set()

    def connect(self):
        """Connects to the agent instance"""
        try:
            self.__socket = socket.create_connection(('localhost', self.port))
            self.__connected.set()
            return
        except:
            pass

        raise UnableToConnect("to robot {}".format(self.number))

    def __send_heart_beat(self):
        """Send heart beat to agent."""
        if self.__socket:
            self.__socket.setblocking(True)
            self.__socket.sendall(struct.pack('!i', -1))

    def __poll_answers(self):
        """Retrieves the answers of commands send to the agent."""
        if self.__socket:
            try:
                self.__socket.setblocking(False)
                id = struct.unpack("<I", self.__socket.recv(4))[0]
                if id > 0:
                    self.__socket.setblocking(True)
                    length = struct.unpack("<I", self.__socket.recv(4))[0]
                    if length > 0:
                        # The command caller is responsible for retrieving the command result
                        # TODO: what happens/todo, if the result is never retrieved?!
                        self.__cmd_result[id] = self.__socket.recv(length)
                    else:
                        logging.warning('Got invalid command data length! (%d)', id)
                else:
                    logging.warning('Got invalid command id!')

            except BlockingIOError:
                # no data available - ignore
                pass

    def __send_commands(self):
        """Sends a command to the agent. The command is retrieved of the command queue."""
        self.__socket.setblocking(True)
        while not self.__cmd_q.empty():
            cmd = self.__cmd_q.get()
            logging.info('Send command %d: "%s"', cmd.id, cmd.get_name())
            self.__socket.sendall(cmd.serialize(cmd.id))


    def send_command(self, cmd:Command):
        """
        Schedules a command for execution.

        :param cmd: the command which should be scheduled/executed
        :return:    returns the command id, which can be used to retrieve the result afterwards. See :func:`~AgentController.command_result`
        """
        if self.connect_dbg:
            cmd.id = self.__cmd_id.value
            self.__cmd_q.put_nowait(cmd)
            self.__cmd_id.set(self.__cmd_id.value + 1)
            return cmd.id
        return 0

    def command_result(self, id):
        """
        Returns the result of the command with :id:.
        If the resault isn't available, None is returned.

        :param id:  the id of the command
        :return:    the result of the command, or None if the result isn't available
        """
        return self.__cmd_result.pop(id, None)

    def run(self):
        """The main method of this process. It starts the agent application, connects to it and also sends scheduled
        commands to the agent (debug requests, representation, ...)."""
        self.__start_agent()

        if self.port is None:
            logging.error("Couldn't retrieve agents debug port!")
            return

        while not self.__cancel.is_set() and (not self.__start_instance or self.__p.poll() is None):
            # only if the controller should connect to the debug port
            if self.connect_dbg:
                if not self.__connected.is_set():
                    self.connect()

                self.__send_heart_beat()
                self.__poll_answers()
                self.__send_commands()

            time.sleep(0.1)

        self.__stop_agent()

    def debugrequest(self, request:str, enable:bool, type:str='cognition'):
        """
        Enables/Disables a debug request of the agent instance.

        :param request: the debug request which should be en-/disabled
        :param enable:  True, if debug request should be enabled, False if it should be disabled
        :param type:    the type of the debug request ('cognition' or 'motion')
        :return:        Returns the id of the scheduled command
        """
        if type == 'cognition':
            return self.send_command(Command('Cognition:debugrequest:set', [(request, ('on' if enable else 'off'))]))
        elif type == 'motion':
            return self.send_command(Command('Motion:debugrequest:set', [(request, ('on' if enable else 'off'))]))
        else:
            logging.warning('Unknown debug request type! Allowed: "cognition", "motion"')
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
            return self.send_command(Command('Cognition:modules:set', [(name, ('on' if enable else 'off'))]))
        elif type == 'motion':
            return self.send_command(Command('Motion:modules:set', [(name, ('on' if enable else 'off'))]))
        else:
            logging.warning('Unknown module type! Allowed: "cognition", "motion"')
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
                return self.send_command(Command('Cognition:representation:get', [name]))
            else:
                return self.send_command(Command('Cognition:representation:print', [name]))
        elif type == 'motion':
            if binary:
                return self.send_command(Command('Motion:representation:get', [name]))
            else:
                return self.send_command(Command('Motion:representation:print', [name]))
        else:
            logging.warning('Unknown representation type! Allowed: "cognition", "motion"')
        return 0

    def agent(self, name:str):
        """
        Selects an named agent for execution.

        :param name: the name of the agent (behavior), which should be executed
        :return:    Returns the id of the scheduled command
        """
        return self.send_command(Command('Cognition:behavior:set_agent', [('agent', name)]))

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
