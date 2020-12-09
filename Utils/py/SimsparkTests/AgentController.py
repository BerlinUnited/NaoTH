import asyncio
import logging
import os
import re
import signal

from naoth.utils import AgentController as Controller


class AgentController(Controller):
    """Represents the controlling instance of a simspark agent. It serves two purposes, first its starts the agent
    application and second it sends debug and other requests to the instance."""

    def __init__(self, app, cwd, team=None, number=None, sync=True, start_instance=True, connect=True):
        """Constructor of the :AgentController:. Initializes public and private attributes."""
        super().__init__('localhost', start=False)
        self.name = "AgentController:{}:{}".format(str(team), str(number))

        self.app = os.path.abspath(app) if os.path.isfile(app) else app
        self.cwd = cwd
        self.team = team
        self.number = number
        self.sync = sync
        self.dbg_connect = connect
        self.ss_port = None  # simspark port

        self.__p = None
        self.__start_instance = start_instance

    def run(self):
        """The main method of this process. It starts the agent application, connects to it and also sends scheduled
        commands to the agent (debug requests, representation, ...)."""

        # start the agent instance first
        self._loop.run_until_complete(self.__start_agent())
        # if process started, schedule task to monitor the process!
        if self.__start_instance:
            self._tasks.append(self._loop.create_task(self.__monitor_agent()))

        if self.dbg_connect:
            # connect to the debug port of the agent instance
            super(AgentController, self).run()
        else:
            # do NOT connect to the debug port of the agent instance
            self.run_disconnected()

    def run_disconnected(self):
        # set the event loop to this thread
        asyncio.set_event_loop(self._loop)
        # run tasks cooperatively and wait 'till loop is stopped
        self._set_connected(True)
        self._loop.run_forever()
        self._set_connected(True)

    async def __start_agent(self):
        """Starts a agent instance as separate process and waits until it is completely started."""
        if self.__start_instance:
            logging.info('Starting agent')

            args = [self.app]
            if self.sync:    args.append('--sync')
            if self.number:  args.extend(['-n', str(self.number)])
            if self.team:    args.extend(['--team', str(self.team)])
            if self.ss_port: args.extend(['--port', str(self.ss_port)])

            # NOTE: the process must be started in its own session in order to gracefully stop the application.
            #       Otherwise an interrupt (CTRL+C) to the main process is propagated to the child process in the
            #       same process group (in posix os)
            self.__p = await asyncio.create_subprocess_exec(*args,
                                                            cwd=self.cwd,
                                                            start_new_session=True,  # see NOTE above
                                                            stdout=asyncio.subprocess.PIPE,
                                                            stderr=asyncio.subprocess.PIPE)
            logging.debug(' '.join(args))

            while True:
                # TODO: retrieve some informations about the agent (Playernumber, Port, ...
                o = (await self.__p.stdout.readline()).decode()
                #[DebugServer:port 5402] is ready for incoming connections.
                if o.startswith('SimSpark Controller runs') or len(o) == 0:
                    break
                elif o.startswith('[DebugServer:port'): # re.match('\[DebugServer:port (\d+)\]', o):
                    self._port = re.match('\[DebugServer:port (\d+)\]', o).group(1)
            logging.info('Agent started')
        else:
            logging.info('No instance of agent started!')
            self._port = 5400 + self.number

    async def __monitor_agent(self):
        try:
            await self.__p.wait()
            # if we get here, something unexpected happen!?
            raise Exception('Agent process stopped unexpected!')
        except asyncio.CancelledError:
            # stop the agent gracefully
            await self.__stop_agent()

    async def __stop_agent(self):
        """Stops a still active agent instance."""

        async def process_stopped():
            """Helper function to wait for the process to be stopped, but not longer than necessary."""
            try:
                await asyncio.wait_for(self.__p.wait(), 0.5)
            except TimeoutError:
                pass  # ignore, since we kind of expected it

        if self.__start_instance and self.__p.returncode is None:
            # stop the controller thread first
            logging.info("Quit agent application")
            self.__p.send_signal(signal.SIGHUP)
            await process_stopped()
            # still running?
            if self.__p.returncode is None:
                logging.info("Stop agent application")
                self.__p.terminate()
                await process_stopped()
                # still running?
                if self.__p.returncode is None:
                    logging.info("Kill agent application")
                    self.__p.kill()
                    await process_stopped()
