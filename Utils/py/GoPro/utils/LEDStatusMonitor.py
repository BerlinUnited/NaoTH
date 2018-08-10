import socket
import threading
import time

from utils import Logger, blackboard

# setup logger for network related logs
logger = Logger.getLogger("LED")

class LedStatusMonitor(threading.Thread):

    def __init__(self):
        super().__init__()

        self.__cancel = threading.Event()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        self.network = None
        self.gopro = None
        self.gc = None

    def run(self):
        while not self.__cancel.is_set():
            self.network = blackboard['network']
            self.gopro = blackboard['gopro']
            self.gc = blackboard['gamecontroller']

            # handle the blue led
            if self.network is None or self.network == 0:
                # (gopro) network not available/visible
                self.sendMessage('{{"blue":"blink", "delay":"0.1", "time":"{}"}}'.format(1))
            elif self.network == 1:
                # not connected to (gopro) network
                self.sendMessage('{{"blue":"blink", "delay":"0.4", "time":"{}"}}'.format(1))
            elif self.network == 2:
                # connecting to (gopro) network
                self.sendMessage('{{"blue":"blink", "delay":"0.8", "time":"{}"}}'.format(1))
            elif self.network == 3:
                # connected to (gopro) network ...
                if self.gopro is None or self.gopro['state'] <= 1:
                    # ... but not to the gopro itself
                    self.sendMessage('{{"blue":"blink", "time":"{}"}}'.format(1))
                elif self.gopro['state'] >= 2:
                    # ... and connected to the gopro
                    if self.gopro['state'] == 3:
                        # ..., but gopro has no sdcard
                        self.sendMessage('{{"blue":"blink", "time":"{}"}}'.format(1))
                    else:
                        # ... and is ready to record
                        self.sendMessage('{{"blue":"on", "time":"{}"}}'.format(1))

            # handle the green led
            if self.gc is None:
                # nothing received from GameController
                self.sendMessage('{{"green":"off", "time":"{}"}}'.format(1))
            elif not all([t.teamNumber > 0 for t in self.gc.team]):
                # only one team in the game (the other is "INVISIBLE")
                self.sendMessage('{{"green":"blink", "time":"{}"}}'.format(1))
            else:
                # GameController is ready
                self.sendMessage('{{"green":"on", "time":"{}"}}'.format(1))

            # handle the red led
            if self.gopro is not None and self.gopro['state'] == 4:
                # gopro is recording
                self.sendMessage('{{"red":"blink", "time":"{}"}}'.format(1))
            elif self.gopro is not None and self.gopro['state'] == 3:
                # gopro has no sdcard!
                self.sendMessage('{{"red":"blink", "time":"{}"}}'.format(1))
            else:
                # gopro is NOT recording
                self.sendMessage('{{"red":"off", "time":"{}"}}'.format(1))

            # update every 0.5 seconds
            time.sleep(0.5)
        logger.debug("LED thread finished.")

    def cancel(self):
        self.__cancel.set()

    def sendMessage(self, msg):
        self.sock.sendto(msg.encode(), ("localhost", 8000))
