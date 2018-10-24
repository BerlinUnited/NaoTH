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


class Command:
    def __init__(self, name, args=None):
        self.name = name
        self.args = args

    def serialize(self, cmd_id):
        cmd_args = []
        if self.args:
            for a in self.args:
                if isinstance(a, str):
                    cmd_args.append(Messages_pb2.CMDArg(name=a))
                else:
                    cmd_args.append(Messages_pb2.CMDArg(name=a[0], bytes=a[1]))

        proto = Messages_pb2.CMD(name=self.name, args=cmd_args)
        return struct.pack("<I", cmd_id) + struct.pack("<I", proto.ByteSize()) + proto.SerializeToString()

    def get_name(self):
        return self.name

    def add_arg(self, arg):
        self.args.append(arg)


class AgentController(multiprocessing.Process):

    def __init__(self, app, cwd, number=None, sync=True, start_instance=True):
        super().__init__()

        self.app = os.path.abspath(app) if os.path.isfile(app) else app
        self.cwd = cwd
        self.number = number
        self.sync = sync
        self.port = None

        self.__p = None
        self.__start_instance = start_instance
        self.__socket = None
        self.__cmd_q = multiprocessing.Queue()
        self.__cmd_id = 1

        self.__cancel = multiprocessing.Event()
        self.__connected = multiprocessing.Event()
        self.started = multiprocessing.Event()

    def __start_agent(self):
        if self.__start_instance:
            logging.info('Starting agent')

            args = [self.app]
            if self.sync: args.append('--sync')
            if self.number: args.extend(['-n', str(self.number)])

            self.__p = subprocess.Popen(args, cwd=self.cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
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
        self.__cancel.set()

    def connect(self):
        self.__socket = socket.create_connection(('localhost', self.port))
        self.__connected.set()

    def __send_heart_beat(self):
        ''' Send heart beat to agent. '''
        if self.__socket:
            self.__socket.setblocking(True)
            self.__socket.sendall(struct.pack('!i', -1))

    def __poll_answers(self):
        if self.__socket:
            try:
                self.__socket.setblocking(False)
                id = struct.unpack("<I", self.__socket.recv(4))[0]
                if id > 0:
                    self.__socket.setblocking(True)
                    length = struct.unpack("<I", self.__socket.recv(4))[0]
                    if length > 0:
                        data = self.__socket.recv(length)
                        # TODO: the caller should know, how to handle the response!?
                        try:
                            dbr = Messages_pb2.DebugRequest()
                            dbr.ParseFromString(data)
                            print(dbr)
                        except Exception as e:
                            print(e)
                            print(data)
                    else:
                        logging.warning('Got invalid command data length!')
                else:
                    logging.warning('Got invalid command id!')

            except BlockingIOError:
                # no data available - ignore
                pass

    def __send_commands(self):
        self.__socket.setblocking(True)

        while not self.__cmd_q.empty():
            cmd = self.__cmd_q.get()
            #c = Messages_pb2.CMD(name='Cognition:representation:get', args=[Messages_pb2.CMDArg(name='DebugRequest')])
            logging.info('Send command "%s"', cmd.get_name())
            self.__socket.sendall(cmd.serialize(self.__cmd_id))
            self.__cmd_id += 1

    def send_command(self, cmd:Command):
        self.__cmd_q.put_nowait(cmd)

    def run(self):
        self.__start_agent()

        if self.port is None:
            logging.error("Couldn't retrieve agents debug port!")
            return

        while not self.__cancel.is_set() and (not self.__start_instance or self.__p.poll() is None):
            if not self.__connected.is_set():
                self.connect()
                #self.send_command(Command('Cognition:representation:get', ['DebugRequest']))

            self.__send_heart_beat()
            self.__poll_answers()
            self.__send_commands()

            time.sleep(0.3)

        self.__stop_agent()

    def debugrequest(self, request:str, enable:bool, type:str='cognition'):
        if type == 'cognition':
            self.send_command(Command('Cognition:debugrequest:set', [(request, ('on' if enable else 'off').encode())]))
        elif type == 'motion':
            self.send_command(Command('Motion:debugrequest:set', [(request, ('on' if enable else 'off').encode())]))
