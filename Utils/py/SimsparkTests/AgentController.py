import ctypes
import logging
import multiprocessing
import os
import re
import subprocess
import time
import signal

from naoth.utils import DebugCommand, AgentController as Controller


class UnableToConnect(Exception):
    pass


class AgentController(multiprocessing.Process):
    """Represents the controlling instance of a simspark agent. It serves two purposes, first its starts the agent
    application and second it sends debug and other requests to the instance."""

    def __init__(self, app, cwd, team=None, number=None, sync=True, start_instance=True, connect=True):
        """Constructor of the :AgentController:. Initializes public and private attributes."""
        super().__init__(name="AgentController:{}:{}".format(str(team), str(number)))

        self.app = os.path.abspath(app) if os.path.isfile(app) else app
        self.cwd = cwd
        self.team = team
        self.number = number
        self.sync = sync
        self.dbg_port = None
        self.dbg_connect = connect
        self.ss_port = None  # simspark port

        self.__p = None
        self.__m = multiprocessing.Manager()
        self.__start_instance = start_instance
        self.__controller = Controller('localhost', self.dbg_port, start=False)
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
            if self.sync:    args.append('--sync')
            if self.number:  args.extend(['-n', str(self.number)])
            if self.team:    args.extend(['--team', self.team])
            if self.ss_port: args.extend(['--port', self.ss_port])

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
                    self.dbg_port = re.match('\[DebugServer:port (\d+)\]', o).group(1)
            self.started.set()
            logging.info('Agent started')
        else:
            logging.info('No instance of agent started!')
            self.dbg_port = 5400 + self.number
            self.started.set()

        self.__controller.start()

    def __stop_agent(self):
        """Stops a still active agent instance."""
        if self.__start_instance and self.__p.poll() is None:
            # stop the controller thread first
            if self.__controller.is_alive(): self.__controller.stop()
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

    def send_command(self, cmd:DebugCommand) -> DebugCommand:
        return self.__controller.send_command(cmd)

    def run(self):
        """The main method of this process. It starts the agent application, connects to it and also sends scheduled
        commands to the agent (debug requests, representation, ...)."""
        self.__start_agent()

        if self.dbg_port is None:
            logging.error("Couldn't retrieve agents debug port!")
            return

        while not self.__cancel.is_set() and (not self.__start_instance or self.__p.poll() is None):
            self.__controller.wait_connected()
            time.sleep(0.1)

        self.__stop_agent()

    def debugrequest(self, request:str, enable:bool, type:str='cognition') -> DebugCommand:
        return self.__controller.debugrequest(request, enable, type)

    def module(self, name:str, enable:bool, type:str='cognition') -> DebugCommand:
        return self.__controller.module(name, enable, type)

    def representation(self, name:str, type:str='cognition', binary:bool=False) -> DebugCommand:
        return self.__controller.representation(name, type, binary)

    def agent(self, name:str) -> DebugCommand:
        return self.__controller.agent(name)

    def behavior(self, complete=False) -> DebugCommand:
        return self.__controller.behavior(complete)
