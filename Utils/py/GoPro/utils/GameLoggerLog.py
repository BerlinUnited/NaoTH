import os
import re
import queue
import threading
import socket
import time
import json

from utils import Logger, blackboard

# setup logger for network related logs
logger = Logger.getLogger("GameLoggerLog")

class GameLoggerLog(threading.Thread):

    def __init__(self, folder, teams=None, log_invisible=False):
        super().__init__()

        self.__cancel = threading.Event()

        self.folder = folder
        self.teams = teams if teams is not None else {}
        self.log_invisible = log_invisible
        self.messages = queue.Queue()

        self.state = {'t1': None, 't2': None, 'h': None, 'v': None}
        self.last_file = None
        self.added_video = False

        self.extension = '.log'
        self.separator = ', '
        self.raspi_name = socket.gethostname()
        self.gopro_info = 'Unknown'

    def run(self):
        # run until canceled
        while not self.__cancel.is_set():
            self.__handle_gopro()
            self.__handle_gamecontroller()
        self.__closeLog()
        logger.debug("GameLoggerLog thread finished.")

    def __handle_gopro(self):
        # check, whether the gopro stopped recording and got a new video file
        gopro = blackboard['gopro']
        if gopro is not None and gopro['state'] == 2 and self.state['v'] != gopro['lastVideo']:
            if self.last_file is not None:
                # got a videofile, add it to the game
                logger.debug("new video file: " + gopro['lastVideo'])
                # if there was already a video written to the log file, add seperator first
                if self.added_video: self.last_file.write(self.separator)
                # remember, that a video was already added
                else: self.added_video = True
                # write the video name to the log file
                self.last_file.write(json.dumps(gopro['lastVideo']))
                self.last_file.flush()
                self.state['v'] = gopro['lastVideo']
            else:
                logger.error("Got a video file, but without an running game!")

    def __handle_gamecontroller(self):
        gc_data = blackboard['gamecontroller']
        # check data of the gamecontroller
        if gc_data is not None:
            # check game file - did something changed in the game state?
            if self.__gamestate_changed(gc_data):
                # close previous/old game file
                self.__closeLog()
                # remember current state
                self.state['t1'] = gc_data.team[0].teamNumber
                self.state['t2'] = gc_data.team[1].teamNumber
                self.state['h']  = gc_data.firstHalf
                # if we shouldn't log games with invisibles and there's one, skip this game
                if self.log_invisible or (gc_data.team[0].teamNumber != 0 and gc_data.team[1].teamNumber != 0):
                    # replace whitespaces with underscore
                    t1 = self.__get_team(gc_data.team[0].teamNumber)
                    t2 = self.__get_team(gc_data.team[1].teamNumber)
                    t = t1 + '_vs_' + t2 if gc_data.firstHalf else t2 + '_vs_' + t1
                    h = '1stHalf' if gc_data.firstHalf else '2ndHalf'
                    # open new game file
                    file = self.folder + "_".join([time.strftime("%Y-%m-%d_%H-%M-%S", time.gmtime()), t, h, self.raspi_name]) + self.extension
                    # append, if already exists
                    self.last_file = open(file, 'a')
                    if self.last_file:
                        logger.debug("created new log file: "+file)
                        # make sure everybody can read/write the file
                        os.chmod(self.last_file.name, 0o666)
                        self.last_file.write('{ "video": [')
        else:
            # no gamecontroller / timed out? - close log
            self.__closeLog()

    def __gamestate_changed(self, gc_data):
        return self.last_file is None \
            or self.state['t1'] != gc_data.team[0].teamNumber \
            or self.state['t2'] != gc_data.team[1].teamNumber \
            or self.state['h'] != gc_data.firstHalf

    def __get_team(self, team):
        return re.sub("\s+", "_", str(self.teams[team] if team in self.teams else team))

    def __closeLog(self):
        if self.last_file is not None:
            self.last_file.write('], "info": '
                                 + json.dumps(blackboard['gopro']['info'])
                                 + ', "gopro_time": "' + blackboard['gopro']['datetime'] + '"'
                                 + ' }\n')
            self.last_file.close()
            logger.debug("closed log file: " + self.last_file.name)
            self.last_file = None
            self.added_video = False

    def cancel(self):
        self.__cancel.set()
