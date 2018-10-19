import logging
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

    def run(self):
        self.__start_application()
        # print(self.__p)

        while not self.__start_instance or self.__p.poll() is None:
            if not self.connected.is_set():
                self.connect()
            else:
                length = struct.unpack("!I", self.socket.recv(4))[0]
                # print(length)
                msg = self.socket.recv(length).decode()
                # print(msg)
                sexp = sexpr.str2sexpr(msg)
                # TODO: save the retrieved infos somewhere!?
                print(sexp)
                if not self.__cmd_queue.empty():
                    cmd = self.__cmd_queue.get()
                    print(cmd)
                    self.socket.sendall(struct.pack("!I", len(cmd)) + str.encode(cmd))
                time.sleep(1)

        self.disconnect()

    def cmd_dropball(self):
        self.__cmd_queue.put('(dropBall)')

    def cmd_ballPos(self, x=0.0, y=0.0, z=0.0):
        self.__cmd_queue.put('(ball (pos {} {} {}))'.format(x, y, z))

    def cmd_ballPosVel(self, x=0.0, y=0.0, z=0.0, vx=0.0, vy=0.0, vz=0.0):
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
