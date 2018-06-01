import os
import queue
import threading

import time

from utils import Event, Logger

# setup logger for network related logs
logger = Logger.getLogger("GoPro")

class GameLoggerLog(threading.Thread):

    def __init__(self, folder, teams=None):
        super().__init__()

        Event.registerListener(self)
        self.__cancel = threading.Event()

        self.folder = folder
        self.teams = teams if teams is not None else {}
        self.messages = queue.Queue()

        self.state = {}
        self.last_file = None

        self.extension = '.log'
        self.separator = '; '

    def run(self):

        while not self.__cancel.is_set():
            ts, msg = self.messages.get()
            if isinstance(msg, str) and self.last_file is not None:
                # got a videofile, add it to the game
                self.last_file.write(msg + self.separator)
                self.last_file.flush()
            elif msg is not None:
                t1 = self.teams[msg.team[0].teamNumber] if msg.team[0].teamNumber in self.teams else msg.team[0].teamNumber
                t2 = self.teams[msg.team[1].teamNumber] if msg.team[1].teamNumber in self.teams else msg.team[1].teamNumber
                h = '1stHalf' if msg.firstHalf else '2ndHalf'
                # check game file - did something changed in the game state?
                if self.last_file is None or self.state['t1'] != t1 or self.state['t2'] != t2 or self.state['h'] != h:
                    # close previous/old game file
                    if self.last_file is not None:
                        self.last_file.close()
                    # open new game file
                    file = self.folder + "_".join([time.strftime("%Y-%m-%d_%H-%M-%S", ts), t1, t2, h]) + self.extension
                    # append, if already exists
                    self.last_file = open(file, 'a')
                    if self.last_file:
                        # make sure everybody can read/write the file
                        os.chmod(self.last_file.name, 0o666)
                    # remember current state
                    self.state = { 'ts': ts, 't1': t1, 't2': t2, 'h': h }

            self.messages.task_done()

    def cancel(self):
        if self.last_file is not None:
            self.last_file.close()
        self.__cancel.set()
        # add dummy to queue in order to 'interrupt' waiting thread
        self.messages.put_nowait((None, None))
        self.messages.join()

    def receivedGC(self, evt:Event.GameControllerMessage):
        """ Is called, when a new GameController message was received. """
        self.messages.put_nowait((time.gmtime(), evt.message))

    def timeoutGC(self, evt:Event.GameControllerTimedout):
        """ Is called, when a new GameController times out. """
        if self.last_file is not None:
            self.last_file.close()
            self.last_file = None

    def stoppedGopro(self, evt:Event.GoproStopRecording):
        """ Is called, when the gopro stops recording. """
        if evt.file is not None and len(evt.file) > 0 and self.last_file is not None:
            # only if a 'valid' file is provided
            self.messages.put_nowait((time.gmtime(), evt.file))

