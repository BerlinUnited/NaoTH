import logging
import math
import multiprocessing
import socket
import struct
import subprocess
import time

import sexpr


class SimsparkController(multiprocessing.Process):
    def __init__(self, app, start_instance=True):
        super().__init__()

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

    def connect(self):
        # print(self.__p, self.__p.poll() is None)
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
        if self.socket:
            self.socket.close()
            self.socket = None
            self.connected.clear()

    def cancel(self):
        self.__cancel.set()

    def is_connected(self):
        return self.connected.is_set()

    def __start_application(self):
        if self.__start_instance:
            logging.info('Start Simspark')
            self.__p = subprocess.Popen([self.app], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            # wait until nothing is printed by simspark anymore
            l = [0, 1]
            while l[0] != l[1]:
                l = [l[1], len(self.__p.stdout.peek())]
                time.sleep(0.25)
            logging.info('Simspark started')
        else:
            logging.info('No instance of Simspark started!')

    def __stop_application(self):
        if self.__p and self.__p.poll() is None:
            logging.info("Quit simspark application")
            self.__p.kill()

    def run(self):
        self.__start_application()
        # print(self.__p)

        while (not self.__start_instance or self.__p.poll() is None) and not self.__cancel.is_set():
            if not self.connected.is_set():
                self.connect()
            else:
                length = struct.unpack("!I", self.socket.recv(4))[0]
                # print(length)
                msg = self.socket.recv(length).decode()
                # print(msg)
                sexp = sexpr.str2sexpr(msg)

                self.__update_environment(sexp[0])
                self.__update_scene(sexp[1:])

                if not self.__cmd_queue.empty():
                    cmd = self.__cmd_queue.get()
                    logging.debug(cmd)
                    self.socket.sendall(struct.pack("!I", len(cmd)) + str.encode(cmd))
        # send kill command, before disconnecting
        self.cmd_killsim()
        # disconnect
        self.disconnect()
        # if process still running, kill it
        self.__stop_application()

    def __update_environment(self, data):
        for item in data:
            if item[0] == 'messages':
                if len(item) > 1:
                    # TODO: save messages of players
                    #if item[0] not in self.__environment: self.__environment[item[0]] = {}
                    #self.__environment[item[0]][] = {}
                    #print(item)
                    pass
            else:
                self.__environment[item[0]] = item[1]

    def __update_scene(self, data):
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
                    #print(self.__scene)
                else:
                    # 'unknown' object
                    self.__scene.append({ 'type': 'node'})
                    #print(nd)
        elif name == 'RDS':  # Ruby Diff Scene, and indicates that the scene graph is a partial description of the environment
            # check scene graph
            if len(self.__scene) == len(data[1]):
                # iterate through scene objects and update known
                for i, nd in enumerate(self.__scene):
                    # update ball position, only if changed
                    if nd['type'] == 'soccerball' and len(data[1][i][1]) > 2:
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
        robots = []
        for nd in self.__scene:
            if nd['type'] == 'nao' and (team is None or team == nd['team']):
                robots.append(nd)
        return robots

    def get_robot(self, number, team='Left'):
        for i, nd in enumerate(self.__scene):
            if nd['type'] == 'nao' and nd['team'] == team and nd['number'] == number:
                return nd
        return None

    def get_ball(self):
        for i, nd in enumerate(self.__scene):
            if nd['type'] == 'soccerball':
                return nd
        return None

    def cmd_dropball(self):
        self.__cmd_queue.put('(dropBall)')

    def cmd_ballPos(self, x=0.0, y=0.0, z=0.05):
        self.__cmd_queue.put('(ball (pos {} {} {}))'.format(x, y, z))

    def cmd_ballPosVel(self, x=0.0, y=0.0, z=0.05, vx=0.0, vy=0.0, vz=0.0):
        self.__cmd_queue.put('(ball (pos {} {} {})(vel {} {} {}))'.format(x, y, z, vx, vy, vz))

    def cmd_killsim(self):
        self.__cmd_queue.put('(killsim)')

    def cmd_agentPos(self, n, x, y, z=0.2, t='Left'):
        self.__cmd_queue.put('(agent (unum {}) (team {}) (pos {} {} {}))'.format(n, t, x, y, z))

    def cmd_agentMove(self, n, x, y, z=0.2, r=0.0, t='Left'):
        self.__cmd_queue.put('(agent (unum {}) (team {}) (move {} {} {} {}))'.format(n, t, x, y, z, r))

    def cmd_agentBat(self, n, bat, t='Left'):
        self.__cmd_queue.put('(agent (unum {}) (team {}) (battery {}))'.format(n, t, bat))

    def cmd_agentTemp(self, n, temp, t='Left'):
        self.__cmd_queue.put('(agent (unum {}) (team {}) (temperature {}))'.format(n, t, temp))

    def cmd_kickoff(self, t='None'):
        ''' Sends the "kickoff" command to the simspark instance.
            Accepted values are: "None", "Left", "Right"
        '''
        self.__cmd_queue.put('(kickOff {})'.format(t))

    def cmd_reqfullstate(self):
        self.__cmd_queue.put('(reqfullstate)')

    '''
    "(playMode )",  
    "(agent (unum <num>) (team <team>) (pos <x> <y> <z>)(move <x> <y> <z> <rot>)(battery <batterylevel>)(temperature <temperature>))", 
    "(select (unum <num>) (team <team>))", 
    "(kill)", 
    "(kill (unum <num>) (team <team>))", 
    "(repos)", 
    "(repos (unum <num>) (team <team>))", 
    "(time <time>)", 
    "(score (left <score>) (right <score>))",
    "(reqfullstate)"

    '''
