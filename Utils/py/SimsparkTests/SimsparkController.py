import logging
import math
import struct
import traceback
import threading
import asyncio
import sys
import functools
import time

import sexpr
from naoth.utils._debug import open_connection  # needed to work with python < 3.7


class SimsparkController(threading.Thread):
    """Represents the controlling class of an simspark instance. It serves two purposes, first its starts the simspark
    application and second it acts as simspark monitor to send (trainer) commands to the simspark instance.
    All available trainer commands can be found here at:
    http://simspark.sourceforge.net/wiki/index.php/Network_Protocol#Command_Messages_from_Coach.2FTrainer"""

    def __init__(self, app, monitor_port: int = 3200, agent_port: int = 3100, start_instance=True):
        """
        Constructor of the :SimsparkController:. Initializes public and private attributes.

        :param app:             the simspark executable, which should be used if :start_instance: is true
        :param monitor_port:    the port, which should be used for a simspark monitor (and this script)
        :param agent_port:      the port, where the simspark agents connect to
        :param start_instance:  True, if simspark should be started in a separate process, False otherwise
        """
        super().__init__(name="SimsparkController")

        self._app = app
        self._start_instance = start_instance
        self._p = None
        self._host = '127.0.0.1'
        # make sure we have valid ports (not 'None')
        self._port_monitor = monitor_port if monitor_port else 3200
        self._port_agent = agent_port if agent_port else 3100

        self._stream_reader = None
        self._stream_writer = None

        self._tasks = []  # list of started tasks (which should be cancelled in the end)
        self._loop = asyncio.new_event_loop()
        asyncio.get_child_watcher().attach_loop(self._loop)  # HACK: in order to work with < 3.7

        self._connected = threading.Event()  # the event for the other (main) thread to interact with
        self._connected_internal = asyncio.Event(loop=self._loop)  # internal event var

        self._cmd_q = asyncio.Queue(loop=self._loop)

        self.__environment = {}
        self.__scene = []
        self.__last_update = 0

    def run(self) -> None:
        # set the event loop to this thread
        asyncio.set_event_loop(self._loop)

        # start the simspark instance first
        self._loop.run_until_complete(self.__start_application())

        # schedule tasks
        self._tasks.append(self._loop.create_task(self._connection_listener()))  # name='Connection listener'
        self._tasks.append(self._loop.create_task(self._state_updater()))  # name='State updater'
        self._tasks.append(self._loop.create_task(self._command_sender()))  # name='Command sender'

        # run tasks cooperatively and wait 'till loop is stopped
        self._loop.run_forever()

        logging.info('Done')

    async def __start_application(self):
        """Starts a simspark instance as separate process and waits until it is completely started."""
        if self._start_instance:
            # prepare call
            app = [self._app]
            if self._port_monitor is not None:
                app.extend(['--server-port', str(self._port_monitor)])
            if self._port_agent is not None:
                app.extend(['--agent-port', str(self._port_agent)])
            # start simspark with additional arguments
            logging.info('Start Simspark: %s', ' '.join(app))
            # NOTE: the process must be started in its own session in order to gracefully stop the application.
            #       Otherwise an interrupt (CTRL+C) to the main process is propagated to the child process in the
            #       same process group (in posix os)
            self._p = await asyncio.create_subprocess_exec(*app,
                                                           start_new_session=True,  # see NOTE above
                                                           stdout=asyncio.subprocess.PIPE,
                                                           stderr=asyncio.subprocess.PIPE)
            # NOTE: currently i found no solution to determine exactly, when simspark has started.
            #       all attempts to read the output from simspark resulted in blocking or nothing read at all, despite
            #       simspark is outputting some infos ... :(
            #       It looks like that simspark doesn't flush its output buffers and therefore doesn't return something
            #       in piped mode.
            await asyncio.sleep(2)
            logging.info('Simspark started')
        else:
            logging.info('No instance of Simspark started!')

    async def __stop_application(self):
        """Stops a still active simspark instance."""

        async def process_stopped():
            """Helper function to wait for the process to be stopped, but not longer than necessary."""
            try:
                await asyncio.wait_for(self._p.wait(), 0.5)
            except asyncio.TimeoutError:
                pass  # ignore, since we kind of expected it

        if self._start_instance and self._p.returncode is None:
            try:
                logging.info("Quit simspark application")
                self._p.send_signal(2)
                # wait before killing
                await process_stopped()
                if self._p.returncode is None:
                    logging.info("Kill simspark application")
                    self._p.kill()
                    await process_stopped()
            except Exception as ex:
                logging.error("Error occurred while stopping simspark: %s", str(ex))

    def stop(self, timeout=None) -> None:
        """
        Stops the (running) thread and blocks until finished or until the optional timeout occurs.

        Since this method is called from another thread, the stop request is scheduled as task on the main loop and
        executed some ms later. Also `join` is called on the thread to wait until the thread is actually finished.

        The timeout can be used to make sure the main program continues, if an error prevents terminating this
        thread -- though that shouldn't happen.
        """
        logging.debug('Stop simspark')
        if self._loop.is_running():
            self._loop.call_soon_threadsafe(lambda: self._loop.create_task(self._stop_internal()))
            self.join(timeout)

    async def _stop_internal(self) -> None:
        """
        The (internal) scheduled stop request task called by `stop()`.

        It stops all scheduled tasks in reverse order and stops the main loop, which causes the thread to finish.
        """

        for task in reversed(self._tasks):
            task_name = task.get_name() if hasattr(task, 'get_name') else str(task)
            try:
                logging.debug('Cancel task: %s', task_name)
                task.cancel()
                await task
            except Exception as e:
                print('Stop simspark:', task_name, e, file=sys.stderr)

        logging.debug('Stop instance')
        await self.__stop_application()

        self._loop.stop()

    def wait_connected(self, timeout=None) -> None:
        """Blocks until the thread is connected to simspark or until the optional timeout occurs."""
        self._connected.wait(timeout)

    def is_connected(self):
        """
        Returns true, if the we're connected to the simspark instance.

        :return: True, if connected, False otherwise
        """
        return self._connected.is_set()

    def _set_connected(self, state: bool) -> None:
        """Internal helper method to handle the connection state."""
        if state:
            logging.info("Connected to simspark")
            self._connected_internal.set()
            self._connected.set()
        else:
            logging.info("Disconnected from simspark")
            self._connected.clear()
            self._connected_internal.clear()
            if self._stream_writer:
                self._stream_writer.close()

    async def _connection_listener(self):
        logging.debug("Connection task started")

        while True:
            try:
                # simspark instance was started, but stopped unexpectedly - stop everything
                if self._start_instance and self._p.returncode is not None:
                    logging.error("Simspark instance closed unexpected!")
                    self._loop.create_task(self._stop_internal())

                # (try to) establish connection or raise exception
                self._stream_reader, \
                self._stream_writer = await open_connection(host=self._host, port=self._port_monitor)

                # update internal & external connection state
                self._set_connected(True)

                # make sure we got all relevant infos
                self.cmd_reqfullstate()

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

        if self._stream_writer:
            self._stream_writer.close()
            # await self._stream_writer.wait_closed()  # NOTE: this doesn't complete?!
            # await self._stream_writer._protocol._get_close_waiter(self._stream_writer)  # HACK: in order to work with < 3.7

        logging.debug("Connection task stopped")

    async def _state_updater(self):
        """Task to receive the current state from simspark and updates the internal state representation."""
        logging.debug("State updater task started")

        def lost_connection(d):
            """Helper function to determine, if the connection was lost."""
            if d == b'':
                self._set_connected(False)
                return True
            return False

        while True:
            try:
                await self._connected_internal.wait()

                raw_size = await self._stream_reader.read(4)
                if lost_connection(raw_size): continue
                size = struct.unpack("!I", raw_size)[0]

                raw_data = await self._stream_reader.read(size)
                if size > 0 and lost_connection(raw_data): continue

                try:
                    sexp = sexpr.str2sexpr_strict(raw_data.decode())

                    if sexp and len(sexp) > 2:
                        self.__update_environment(sexp[0])
                        self.__update_scene(sexp[1:])
                        self.__last_update = time.monotonic()
                except sexpr.SExprIllegalClosingParenError:
                    # ignore specific parsing exception
                    pass
                except sexpr.SExprPrematureEOFError:
                    # ignore specific parsing exception
                    pass
                except Exception as e:
                    logging.error(str(e))
                    traceback.print_exc(limit=2)

            except asyncio.CancelledError:  # task cancelled
                break
            except OSError:  # connection lost
                self._set_connected(False)
            except Exception as e:  # unexpected exception
                print('State updater:', e, file=sys.stderr)

        logging.debug("State updater task stopped")

    async def _command_sender(self) -> None:
        """Task to send scheduled commands."""
        logging.debug("Command sender task started")

        while True:
            try:
                await self._connected_internal.wait()
                # get next command
                cmd = await self._cmd_q.get()
                # send command
                self._stream_writer.write(struct.pack("!I", len(cmd)) + str.encode(cmd))
                await self._stream_writer.drain()
                # mark as done
                self._cmd_q.task_done()
            except asyncio.CancelledError:  # task cancelled
                break
            except OSError:  # connection lost
                self._set_connected(False)
            except Exception as e:  # unexpected exception
                print('Send commands:', e, file=sys.stderr)

        logging.debug("Command sender task stopped")

    def send_command(self, cmd: str) -> None:
        """
        Schedules the given command in the command queue.

        :raises Exception: if not connected to simspark or if the given command is invalid
        """
        if not self.is_connected():
            raise Exception('Not connected to simspark!')

        if not cmd.startswith('(') or not cmd.endswith(')'):
            raise Exception('Invalid command!')

        # command queue is not thread safe - make sure we're add it in the correct thread
        # this can 'cause a delay of ~0.5ms
        self._loop.call_soon_threadsafe(functools.partial(self._cmd_q.put_nowait, cmd))

    def __update_environment(self, data):
        """
        Updates the internal state of the simspark environment.
        The environment contains information about the game time,
        the messages of the player, the game state and much more.

        :param data:    the updated environment infos as list of key/value pairs
        :return:        None
        """
        env = self.__environment
        for item in data:
            try:
                if item[0] == 'messages':
                    if len(item) > 1:
                        for m in item[1:]:
                            # check if the format is the expected
                            if len(m) == 3 and len(m[1]) == 2 and m[1][0] == 'side' and len(m[2]) == 2 and m[2][0] == 'ip':
                                if 'messages' not in env:
                                    env['messages'] = [0, 0]
                                if m[1][1] == '1':  # side 1 / left
                                    env['messages'][0] += 1
                                elif m[1][1] == '2':  # side 2 / right
                                    env['messages'][1] += 1
                        '''
                        # TODO: save messages of players
                        if item[0] not in self.__environment: self.__environment[item[0]] = {}
                        self.__environment[item[0]][] = {}
                        print(item)
                        '''
                elif item[0] in ['FieldLength', 'FieldWidth', 'FieldHeight', 'GoalWidth', 'GoalDepth', 'GoalHeight', 'FreeKickDistance', 'WaitBeforeKickOff', 'AgentRadius', 'BallRadius', 'BallMass', 'RuleGoalPauseTime', 'RuleKickInPauseTime', 'RuleHalfTime', 'time']:
                    env[item[0]] = float(item[1])
                elif item[0] in ['half', 'score_left', 'score_right', 'play_mode']:
                    env[item[0]] = int(item[1])
                else:
                    env[item[0]] = item[1:]
            except Exception as e:
                logging.warning("Exception while updating environment: %s\n%s", e, str(item))
        self.__environment = env

    def __update_scene(self, data):
        """
        Updates the internal scene state of the simspark simulation. This is necessary to retrieve the position of the
        simulated agents and of the ball. Simspark sends a 'full scene graph' each time a simulated object is added or
        removed, otherwise a 'sparse scene graph' is send with only the value of the simulated objects which changed.
        Only the position of the agents and the soccerball are extracted and saved, other objects are ignored.

        :param data:    the scene graph full/sparse as list of lists
        :return:        None
        """
        if len(data[0]) == 3:
            name, version, subversion = data[0]
            if name == 'RSG':  # Ruby Scene Graph: indicates that the scene graph is a full description of the env.
                # clear old scene graph
                del self.__scene[:]
                # apply new scene graph
                for nd in data[1]:
                    # update known objects
                    if len(nd) == 4 and isinstance(nd, list) and len(nd[3]) > 4 and 'soccerball' in nd[3][3][1]:
                        # found soccer ball
                        self.__scene.append({
                            'type': 'soccerball',
                            'x': float(nd[2][13]),
                            'y': float(nd[2][14]),
                            'z': float(nd[2][15])
                        })
                    elif len(nd) >= 4 and len(nd[3]) >= 4 and len(nd[3][3]) >= 4 and len(nd[3][3][3]) >= 4 and 'naobody' in nd[3][3][3][3][1]:
                        # found an agent
                        pose = self.__get_robot_pose(nd[3][2][1:])
                        # set the nao infos
                        self.__scene.append({
                            'type': 'nao',
                            'team': nd[3][3][3][5][2][3:],
                            'number': int(nd[3][3][3][5][1][6:]) if len(nd[3][3][3][5][1]) > 6 else 0,
                            'x': pose[0],
                            'y': pose[1],
                            'z': pose[2],
                            'r': pose[3]
                        })
                    else:
                        # 'unknown' object
                        self.__scene.append({'type': 'node'})
            elif name == 'RDS':  # Ruby Diff Scene, and indicates that the scene graph is a partial description of the environment
                # check scene graph
                if len(data) == 2 and len(self.__scene) == len(data[1]):
                    # iterate through scene objects and update known
                    for i, nd in enumerate(self.__scene):
                        # update ball position, only if changed
                        if nd['type'] == 'soccerball' and len(data[1][i][1]) >= 16:
                            ball = self.__scene[i]
                            ball['x'] = float(data[1][i][1][13])
                            ball['y'] = float(data[1][i][1][14])
                            ball['z'] = float(data[1][i][1][15])
                            # notify the managed list of the change!
                            self.__scene[i] = ball
                        elif nd['type'] == 'nao' and data[1][i][1][1][0] == 'SLT':
                            robot = self.__scene[i]
                            pose = self.__get_robot_pose(data[1][i][1][1][1:])
                            robot['x'] = pose[0]
                            robot['y'] = pose[1]
                            robot['z'] = pose[2]
                            robot['r'] = pose[3]
                            # notify the managed list of the change!
                            self.__scene[i] = robot
                else:
                    logging.warning('Scene graph mismatch!')
            else:
                logging.warning('invalid scene graph update!')
        else:
            logging.warning('too few/many scene graph data: %s', str(data[0]))

    def __get_robot_pose(self, data):
        """
        Retrieves the transformation matrix of the agent of the (sub-)scene graph and calculates the position of the
        agent. The matrix of the scene graph is a little bit different than what the robot would receive from the
        simulation.

        :param data:    the (sub-)scene graph which contains the transformation matrix of the agent
        :return:        a tuple (x,y,z,r) of the agents position
        """
        # make a rotation matrix
        _ = list(map(float, data))
        r = [[_[4], -_[0], _[8], _[12]],
             [_[5], -_[1], _[9], _[13]],
             [_[6], -_[2], _[10], _[14]],
             [_[7], -_[3], _[11], _[15]]]
        z = math.acos(r[0][0] / math.sqrt(r[0][0] * r[0][0] + r[1][0] * r[1][0])) * (-1 if r[1][0] < 0 else 1)
        #z /= (math.pi/180)

        return r[0][3], r[1][3], r[2][3], z

    def get_robots(self, team=None):
        """
        Returns all robots active in the simulation. If a team is set ('Left' or 'Right') only the robots of this team
        are returned.

        :param team:    limit the returned robots to the selected team (None, 'Left', 'Right')
        :return:        a list of the robots in the simulation
        """
        robots = []
        for nd in self.__scene:
            if nd['type'] == 'nao' and (team is None or team == nd['team']):
                robots.append(nd)
        return robots

    def get_robot(self, number, team='Left'):
        """
        Returns the robot with the :number: and :team: in the simulation. If the player number wasn't found None is
        returned.
        Eg: {'type': 'nao', 'team': 'Left', 'number': 3, 'x': -2.2787, 'y': 1.24506, 'z': 0.189894, 'r': 0.355352456433}

        :param number:  the player number of the robot, which should be returned
        :param team:    the team number of the player, default is 'Left'
        :return:        None or the found player infos
        """
        for i, nd in enumerate(self.__scene):
            if nd['type'] == 'nao' and nd['team'] == team and nd['number'] == number:
                return nd
        return None

    def get_ball(self):
        """
        Returns the ball informations in the simulation. If there's no ball in the simulation None is returned.
        Eg: {'type': 'soccerball', 'x': -1.5, 'y': 1.5, 'z': 0.0482982}

        :return:    None or the ball infos
        """
        for i, nd in enumerate(self.__scene):
            if nd['type'] == 'soccerball':
                return nd
        return None

    def __get_environment(self, key: str):
        return self.__environment[key] if key in self.__environment else None

    def get_time(self):
        """
        Returns the current simulation time.

        :return:    the current simulation time or None (if there's no time)
        """
        return self.__get_environment('time')

    def get_half(self):
        """
        Returns the current half.

        :return:    the current half or None (if there's no half info)
        """
        return self.__get_environment('half')

    def get_score(self, side: str = 'Left'):
        """
        Returns the current score of the left or right team.

        :param side: 'Left', if the left score should be returned, otherwise ('Right') the right score is returned
        :return:     the current score of the team (left or right) or None (if there's no half info)
        """
        return self.__get_environment('score_left') if side == 'Left' else self.__get_environment('score_right')

    def get_team(self, side: str = 'Left'):
        """
        Returns the name of the left or right team.

        :param side: 'Left', if the name of the left team should be returned, otherwise ('Right') the right team name is returned
        :return:     the name of the team (left or right) or None (if there's no name info)
        """
        return self.__get_environment('team_left') if side == 'Left' else self.__get_environment('team_right')

    def get_playModes(self):
        """
        Returns the play modes of the simulation.

        :return:    the available play modes
        """
        return self.__get_environment('play_modes')

    def get_playMode(self):
        """
        Returns the play mode of the simulation.

        :return:    the current play mode
        """
        modes = self.get_playModes()
        mode = self.__get_environment('play_mode')
        return modes[mode] if modes and len(modes) > mode else mode

    def get_messageCount(self):
        """
        Returns the number of messages of each team.

        :return:    number of messages per team
        """
        return self.__get_environment('messages')

    def cmd_dropball(self):
        """Schedules the '(dropBall)' trainer command for the simspark instance."""
        self.send_command('(dropBall)')

    def cmd_ballPos(self, x=0.0, y=0.0, z=0.05):
        """Schedules the '(ball (pos x y z))' trainer command for the simspark instance."""
        self.send_command('(ball (pos {} {} {}))'.format(x, y, z))

    def cmd_ballPosVel(self, x=0.0, y=0.0, z=0.05, vx=0.0, vy=0.0, vz=0.0):
        """Schedules the '(ball (pos x y z)(vel vx vy vz))' trainer command for the simspark instance."""
        self.send_command('(ball (pos {} {} {})(vel {} {} {}))'.format(x, y, z, vx, vy, vz))

    def cmd_killsim(self):
        """Schedules the '(killsim)' trainer command for the simspark instance."""
        self.send_command('(killsim)')

    def cmd_agentPos(self, n, x, y, z=0.2, t='Left'):
        """Schedules the '(agent (unum n) (team t) (pos x y z))' trainer command for the simspark instance."""
        self.send_command('(agent (unum {}) (team {}) (pos {} {} {}))'.format(n, t, x, y, z))

    def cmd_agentMove(self, n, x, y, z=0.2, r=0.0, t='Left'):
        """Schedules the '(agent (unum n) (team t) (move x y z r))' trainer command for the simspark instance."""
        self.send_command('(agent (unum {}) (team {}) (move {} {} {} {}))'.format(n, t, x, y, z, r))

    def cmd_agentBat(self, n, bat, t='Left'):
        """Schedules the '(agent (unum n) (team t) (battery bat))' trainer command for the simspark instance."""
        self.send_command('(agent (unum {}) (team {}) (battery {}))'.format(n, t, bat))

    def cmd_agentTemp(self, n, temp, t='Left'):
        """Schedules the '(agent (unum n) (team t) (temperature temp))' trainer command for the simspark instance."""
        self.send_command('(agent (unum {}) (team {}) (temperature {}))'.format(n, t, temp))

    def cmd_agentPosMoveBatTemp(self, n, x, y, z=0.2, mx=0.0, my=0.0, mz=0.0, mr=0.0, bat=100.0, temp=30.0, t='Left'):
        """Schedules the '(agent (unum n) (team t) (pos x y z)(move x y z r)(battery bat)(temperature temp))' trainer command for the simspark instance."""
        self.send_command('(agent (unum {}) (team {}) (pos {} {} {})(move {} {} {} {})(battery {})(temperature {}))'.format(n, t, x, y, z, mx, my, mz, mr, bat, temp))

    def cmd_kickoff(self, t='None'):
        """Schedules the '(kickOff t)' trainer command for the simspark instance. Accepted values are: "None", "Left", "Right"""
        self.send_command('(kickOff {})'.format(t))

    def cmd_select(self, n, t='Left'):
        """Schedules the '(select (unum n) (team n))' trainer command for the simspark instance."""
        self.send_command('(select (unum {}) (team {}))'.format(n, t))

    def cmd_reposSelected(self):
        """Schedules the '(repos)' trainer command for the simspark instance."""
        self.send_command('(repos)')

    def cmd_repos(self, n, t='Left'):
        """Schedules the '(repos (unum n) (team t))' trainer command for the simspark instance."""
        self.send_command('(repos (unum {}) (team {}))'.format(n, t))

    def cmd_killSelected(self):
        """Schedules the '(repos)' trainer command for the simspark instance."""
        self.send_command('(kill)')

    def cmd_kill(self, n, t):
        """Schedules the '(kill (unum n) (team t))' trainer command for the simspark instance."""
        self.send_command('(kill (unum {}) (team {}))'.format(n, t))

    def cmd_reqfullstate(self):
        """Schedules the '(reqfullstate)' trainer command for the simspark instance. This command is mainly relevant for
        the simspark monitors."""
        self.send_command('(reqfullstate)')

    def cmd_time(self, time):
        """Schedules the '(time t)' trainer command for the simspark instance."""
        self.send_command('(time {})'.format(time))

    def cmd_score(self, score_left, score_right):
        """Schedules the '(score (left sl) (right sr))' trainer command for the simspark instance."""
        self.send_command('(score (left {}) (right {}))'.format(score_left, score_right))

    def cmd_playMode(self, mode):
        """Schedules the '(playMode mode)' trainer command for the simspark instance.
        Available play modes can be found at: http://simspark.sourceforge.net/wiki/index.php/Play_Modes"""
        self.send_command('(playMode {})'.format(mode))

    @property
    def last_update(self):
        """Returns the last update of the scene/environment."""
        return self.__last_update