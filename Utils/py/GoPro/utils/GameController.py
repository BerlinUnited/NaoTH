import threading, socket

from utils import Event, Logger
from utils.GameControlData import GameControlData

GAME_CONTROLLER_PORT = 3838

# setup logger for network related logs
logger = Logger.getLogger("GameController")

class GameController(threading.Thread):
    """."""

    def __init__(self):
        super().__init__()

        self.cancel = threading.Event()

        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.socket.bind(('', GAME_CONTROLLER_PORT))
        self.socket.settimeout(5)  # in sec


    def run(self):
        logger.info("Listen to GameController")
        while not self.cancel.is_set():
            try:
                # receive GC data
                data, address = self.socket.recvfrom(8192)
                message = GameControlData(data)

                # one of the teams is invisible
                Event.fire(Event.GameControllerMessage(message))

            except socket.timeout:
                Event.fire(Event.GameControllerTimedout())
                #statusMonitor.setDidntReceivedMessageFromGC(1)
                logger.warning("Not connected to GameController?")
                self.message = None
                continue
            except Exception as ex:
                self.message = None
                print(ex)
                continue
