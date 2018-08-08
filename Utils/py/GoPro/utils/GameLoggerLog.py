import os
import re
import queue
import threading
import socket
import time
import json

from utils import Event, Logger

# setup logger for network related logs
logger = Logger.getLogger("GameLoggerLog")

class GameLoggerLog(threading.Thread):

    def __init__(self, folder, teams=None, log_invisible=False):
        super().__init__()

        Event.registerListener(self)
        self.__cancel = threading.Event()

        self.folder = folder
        self.teams = teams if teams is not None else {}
        self.log_invisible = log_invisible
        self.messages = queue.Queue()

        self.state = {}
        self.last_file = None

        self.extension = '.log'
        self.separator = '; '
        self.raspi_name = socket.gethostname()
        self.gopro_info = 'Unknown'

    def run(self):
        # run until canceled
        while not self.__cancel.is_set():
            # get the next message from the queue, added by the methods below
            ts, msg = self.messages.get()
            # is it the dummy/cancel message?
            if ts is None:
                self.messages.task_done()
                break
            elif isinstance(msg, str):
                if self.last_file is not None:
                    # got a videofile, add it to the game
                    self.last_file.write(json.dumps(msg) + self.separator)
                    self.last_file.flush()
                else:
                    logger.error("Got a video file, but without an running game!")
            elif msg is not None:
                # replace whitespaces with underscore
                t1 = re.sub("\s+", "_", str(self.teams[msg.team[0].teamNumber] if msg.team[0].teamNumber in self.teams else msg.team[0].teamNumber))
                t2 = re.sub("\s+", "_", str(self.teams[msg.team[1].teamNumber] if msg.team[1].teamNumber in self.teams else msg.team[1].teamNumber))
                h = '1stHalf' if msg.firstHalf else '2ndHalf'
                # check game file - did something changed in the game state?
                if self.last_file is None or self.state['t1'] != t1 or self.state['t2'] != t2 or self.state['h'] != h:
                    # close previous/old game file
                    self.__closeLog()
                    # remember current state
                    self.state = {'ts': ts, 't1': t1, 't2': t2, 'h': h}
                    # if we shouldn't log games with invisibles and there's one, skip this game
                    if self.log_invisible or (msg.team[0].teamNumber != 0 and msg.team[1].teamNumber != 0):
                        # open new game file
                        file = self.folder + "_".join([time.strftime("%Y-%m-%d_%H-%M-%S", ts), t1, t2, h, self.raspi_name]) + self.extension
                        # append, if already exists
                        self.last_file = open(file, 'a')
                        if self.last_file:
                            # make sure everybody can read/write the file
                            os.chmod(self.last_file.name, 0o666)
                            self.last_file.write('{ "video": [')

            self.messages.task_done()
        logger.debug("GameLoggerLog thread finished.")

    def __closeLog(self):
        if self.last_file is not None:
            self.last_file.write('], "info": ' + json.dumps(self.gopro_info) + ' }\n')
            self.last_file.close()
            self.last_file = None

    def cancel(self):
        self.__closeLog()
        self.__cancel.set()
        # add dummy to queue in order to 'interrupt' waiting thread
        self.messages.put_nowait((None, None))
        self.messages.join()

    def receivedGC(self, evt:Event.GameControllerMessage):
        """ Is called, when a new GameController message was received. """
        self.messages.put_nowait((time.gmtime(), evt.message))

    def timeoutGC(self, evt:Event.GameControllerTimedout):
        """ Is called, when a new GameController times out. """
        self.__closeLog()

    def stoppedGopro(self, evt:Event.GoproStopRecording):
        """ Is called, when the gopro stops recording. """
        if evt.file is not None and len(evt.file) > 0 and self.last_file is not None:
            # only if a 'valid' file is provided
            self.messages.put_nowait((time.gmtime(), evt.file))

    def goproInfo(self, evt:Event.GoproConnected):
        """ Is called, when the gopro is connected. """
        self.gopro_info = evt.info
