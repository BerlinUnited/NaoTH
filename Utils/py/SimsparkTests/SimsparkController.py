import logging
import math
import multiprocessing
import socket
import struct
import subprocess
import time
import traceback

import sexpr


class SimsparkController(multiprocessing.Process):
    """Represents the controlling class of an simspark instance. It serves two purposes, first its starts the simspark
    application and second it acts as simspark monitor to send (trainer) commands to the simspark instance.
    All available trainer commands can be found here at:
    http://simspark.sourceforge.net/wiki/index.php/Network_Protocol#Command_Messages_from_Coach.2FTrainer"""

    def __init__(self, app, start_instance=True):
        """Constructor of the :SimsparkController:. Initializes public and private attributes."""
        super().__init__(name="SimsparkController")

        self.app = app
        self.__start_instance = start_instance
        self.__p = None
        self.port = 3200
        self.host = '127.0.0.1'
        self.socket = None

        self.__m = multiprocessing.Manager()
        self.__environment = self.__m.dict()
        self.__scene = self.__m.list()

        self.__cmd_queue = multiprocessing.Queue()
        self.__cancel = multiprocessing.Event()
        self.connected = multiprocessing.Event()

    def terminate(self):
        """If this process instance gets killed, this is the last chance to kill the simspark instance too."""
        self.__stop_application()
        super(SimsparkController, self).terminate()

    def connect(self):
        """Tries to connect to a running simspark instance. The instance could be started by this class or was started
        externally."""
        if not self.__start_instance or (self.__p and self.__p.poll() is None):
            logging.info("Connecting to simspark")
            try:
                self.socket = socket.create_connection((self.host, self.port))
                self.connected.set()
                logging.info("Connected")
            except Exception as e:
                print('ERROR! Couldnt connect to simspark', e)
                if self.__p:
                    print(self.__p.stdout.peek().decode())
        else:
            logging.error("Simspark isn't running!?")

    def disconnect(self):
        """Disconnects this simspark monitor from the simspark instance."""
        # empty command queue before closing socket
        while not (self.__cmd_queue.empty() and self.__cmd_queue.qsize() == 0):
            cmd = self.__cmd_queue.get()
            logging.debug(cmd)
            self.socket.sendall(struct.pack("!I", len(cmd)) + str.encode(cmd))
        # no commands left, close socket
        if self.socket:
            self.socket.close()
            self.socket = None
            self.connected.clear()

    def cancel(self):
        """Sets the canceled flag of this process."""
        self.__cancel.set()

    def is_connected(self):
        """
        Returns if th monitor is still connected to the simspark instance

        :return: True, if connected, False otherwise
        """
        return self.connected.is_set()

    def __start_application(self):
        """Starts a simspark instance as separate process and waits until it is completely started."""
        if self.__start_instance:
            logging.info('Start Simspark')
            self.__p = subprocess.Popen([self.app], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            # wait until nothing is printed by simspark anymore
            time.sleep(1)
            logging.info('Simspark started')
        else:
            logging.info('No instance of Simspark started!')

    def __stop_application(self):
        """Stops a still active simspark instance."""
        if self.__p and self.__p.poll() is None:
            logging.info("Quit simspark application")
            self.__p.kill()

    def run(self):
        """The main method of this process. It starts the simspark application, connects to it and receives updates of
        the simulation state and also sends scheduled commands to the simulation."""
        self.__start_application()

        while (not self.__start_instance or self.__p.poll() is None) and not self.__cancel.is_set():
            # connect to the simspark instance
            if not self.connected.is_set():
                self.connect()
            else:
                data = self.socket.recv(4)
                # check if we got some data, empty data is disconnected!
                if data:
                    try:
                        length = struct.unpack("!I", data)[0]
                        msg = self.socket.recv(length).decode()
                        sexp = sexpr.str2sexpr(msg)

                        if sexp and len(sexp) > 2:
                            self.__update_environment(sexp[0])
                            self.__update_scene(sexp[1:])

                        # send scheduled (trainer) commands
                        if not self.__cmd_queue.empty():
                            cmd = self.__cmd_queue.get()
                            logging.debug(cmd)
                            self.socket.sendall(struct.pack("!I", len(cmd)) + str.encode(cmd))
                    except Exception as e:
                        print('ERROR:', e, e.__class__)
                        traceback.print_exc(limit=2)
                else:
                    # no data - disconnected
                    self.connected.clear()
        # send kill command, before disconnecting
        self.cmd_killsim()
        # disconnect
        self.disconnect()
        # if process still running, kill it
        self.__stop_application()

    def __update_environment(self, data):
        """
        Updates the internal state of the simspark environment. The environment contains information about the game time,
        the messages of the player, the game state and much more.

        :param data:    the updated environment infos as list of key/value pairs
        :return:        None
        """
        env = self.__environment
        for item in data:
            if item[0] == 'messages':
                if len(item) > 1:
                    # TODO: save messages of players
                    #if item[0] not in self.__environment: self.__environment[item[0]] = {}
                    #self.__environment[item[0]][] = {}
                    #print(item)
                    pass
            else:
                if item[0] in ['FieldLength','FieldWidth','FieldHeight','GoalWidth','GoalDepth','GoalHeight','FreeKickDistance','WaitBeforeKickOff','AgentRadius','BallRadius','BallMass','RuleGoalPauseTime','RuleKickInPauseTime','RuleHalfTime','time']:
                    env[item[0]] = float(item[1])
                elif item[0] in ['half','score_left','score_right','play_mode']:
                    env[item[0]] = int(item[1])
                else:
                    env[item[0]] = item[1]
        self.__environment = env

    def __update_scene(self, data):
        """
        Updates the internal scene state of the simspark simulation. This is necessary to retrieve the position of the
        simulated agents and of the ball. Simspark sends a 'full scene graph' each time a simulated object is add/removed,
        otherwise a 'sparse scene graph' is send with only the value of the simulated objects which changed. Only the
        position of the agents and the soccerball are extracted and saved, other objects are ignored.

        :param data:    the scene graph full/sparse as list of lists
        :return:        None
        """
        name, version, subversion = data[0]

        if name == 'RSG':  # Ruby Scene Graph, and indicates that the scene graph is a full description of the environment.
            # clear old scene graph
            del self.__scene[:]
            # apply new scene graph
            for nd in data[1]:
                # update known objects
                if len(nd) == 4 and isinstance(nd, list) and len(nd[3]) > 4 and 'soccerball' in nd[3][3][1]:
                    # found soccer ball
                    self.__scene.append({ 'type': 'soccerball', 'x':float(nd[2][13]), 'y':float(nd[2][14]), 'z':float(nd[2][15]) })
                elif len(nd) >= 4 and len(nd[3]) >= 4 and len(nd[3][3]) >= 4 and len(nd[3][3][3]) >= 4 and 'naobody' in nd[3][3][3][3][1]:
                    # found an agent
                    pose = self.__get_robot_pose(nd[3][2][1:])
                    # set the nao infos
                    self.__scene.append({
                        'type': 'nao',
                        'team': nd[3][3][3][5][2][3:],
                        'number': int(nd[3][3][3][5][1][6:]) if len(nd[3][3][3][5][1])>6 else 0,
                        'x': pose[0],
                        'y': pose[1],
                        'z': pose[2],
                        'r': pose[3]
                    })
                else:
                    # 'unknown' object
                    self.__scene.append({ 'type': 'node'})
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
                    elif nd['type'] == 'nao' and len(data[1][i][1]) > 4:
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
        Returns all robots acitve in the simulation. If a team is set ('Left' or 'Right') only the robots of this team
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
        Eg: {'type': 'nao', 'team': 'Left', 'number': 3, 'x': -2.2787, 'y': 1.24506, 'z': 0.189894, 'r': 0.35535245643343066}

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

    def __get_environment(self, key:str):
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

    def get_score(self, side:str='Left'):
        """
        Returns the current score of the left or right team.

        :param left: 'Left', if the left score should be returned, otherwise ('Right') the right score is returned
        :return:     the current score of the team (left or right) or None (if there's no half info)
        """
        return self.__get_environment('score_left') if side == 'Left' else self.__get_environment('score_right')

    def get_team(self, side:str='Left'):
        """
        Returns the name of the left or right team.

        :param left: 'Left', if the name of the left team should be returned, otherwise ('Right') the right team name is returned
        :return:     the name of the team (left or right) or None (if there's no name info)
        """
        return self.__get_environment('team_left') if side == 'Left' else self.__get_environment('team_right')

    def get_playMode(self):
        """
        Returns the play mode of the simulation.

        :return:    the current play mode
        """
        return self.__get_environment('play_mode')

    def cmd_dropball(self):
        """Schedules the '(dropBall)' trainer command for the simspark instance."""
        self.__cmd_queue.put('(dropBall)')

    def cmd_ballPos(self, x=0.0, y=0.0, z=0.05):
        """Schedules the '(ball (pos x y z))' trainer command for the simspark instance."""
        self.__cmd_queue.put('(ball (pos {} {} {}))'.format(x, y, z))

    def cmd_ballPosVel(self, x=0.0, y=0.0, z=0.05, vx=0.0, vy=0.0, vz=0.0):
        """Schedules the '(ball (pos x y z)(vel vx vy vz))' trainer command for the simspark instance."""
        self.__cmd_queue.put('(ball (pos {} {} {})(vel {} {} {}))'.format(x, y, z, vx, vy, vz))

    def cmd_killsim(self):
        """Schedules the '(killsim)' trainer command for the simspark instance."""
        self.__cmd_queue.put('(killsim)')

    def cmd_agentPos(self, n, x, y, z=0.2, t='Left'):
        """Schedules the '(agent (unum n) (team t) (pos x y z))' trainer command for the simspark instance."""
        self.__cmd_queue.put('(agent (unum {}) (team {}) (pos {} {} {}))'.format(n, t, x, y, z))

    def cmd_agentMove(self, n, x, y, z=0.2, r=0.0, t='Left'):
        """Schedules the '(agent (unum n) (team t) (move x y z r))' trainer command for the simspark instance."""
        self.__cmd_queue.put('(agent (unum {}) (team {}) (move {} {} {} {}))'.format(n, t, x, y, z, r))

    def cmd_agentBat(self, n, bat, t='Left'):
        """Schedules the '(agent (unum n) (team t) (battery bat))' trainer command for the simspark instance."""
        self.__cmd_queue.put('(agent (unum {}) (team {}) (battery {}))'.format(n, t, bat))

    def cmd_agentTemp(self, n, temp, t='Left'):
        """Schedules the '(agent (unum n) (team t) (temperature temp))' trainer command for the simspark instance."""
        self.__cmd_queue.put('(agent (unum {}) (team {}) (temperature {}))'.format(n, t, temp))

    def cmd_agentPosMoveBatTemp(self, n, x, y, z=0.2, mx=0.0, my=0.0, mz=0.0, mr=0.0, bat=100.0, temp=30.0, t='Left'):
        """Schedules the '(agent (unum n) (team t) (pos x y z)(move x y z r)(battery bat)(temperature temp))' trainer command for the simspark instance."""
        self.__cmd_queue.put('(agent (unum {}) (team {}) (pos {} {} {})(move {} {} {} {})(battery {})(temperature {}))'.format(n, t, x, y, z, mx, my, mz, mr, bat, temp))

    def cmd_kickoff(self, t='None'):
        """Schedules the '(kickOff t)' trainer command for the simspark instance. Accepted values are: "None", "Left", "Right"""
        self.__cmd_queue.put('(kickOff {})'.format(t))

    def cmd_select(self, n, t='Left'):
        """Schedules the '(select (unum n) (team n))' trainer command for the simspark instance."""
        self.__cmd_queue.put('(select (unum {}) (team {}))'.format(n, t))

    def cmd_reposSelected(self):
        """Schedules the '(repos)' trainer command for the simspark instance."""
        self.__cmd_queue.put('(repos)')

    def cmd_repos(self, n, t='Left'):
        """Schedules the '(repos (unum n) (team t))' trainer command for the simspark instance."""
        self.__cmd_queue.put('(repos (unum {}) (team {}))'.format(n, t))

    def cmd_killSelected(self):
        """Schedules the '(repos)' trainer command for the simspark instance."""
        self.__cmd_queue.put('(kill)')

    def cmd_kill(self, n, t):
        """Schedules the '(kill (unum n) (team t))' trainer command for the simspark instance."""
        self.__cmd_queue.put('(kill (unum {}) (team {}))'.format(n, t))

    def cmd_reqfullstate(self):
        """Schedules the '(reqfullstate)' trainer command for the simspark instance. This command is mainly relevant for
        the simspark monitors."""
        self.__cmd_queue.put('(reqfullstate)')

    def cmd_time(self, time):
        """Schedules the '(time t)' trainer command for the simspark instance."""
        self.__cmd_queue.put('(time {})'.format(time))

    def cmd_score(self, score_left, score_right):
        """Schedules the '(score (left sl) (right sr))' trainer command for the simspark instance."""
        self.__cmd_queue.put('(score (left {}) (right {}))'.format(score_left, score_right))

    def cmd_playMode(self, mode):
        """Schedules the '(playMode mode)' trainer command for the simspark instance.
        Available play modes can be found at: http://simspark.sourceforge.net/wiki/index.php/Play_Modes"""
        self.__cmd_queue.put('(playMode {})'.format(mode))
