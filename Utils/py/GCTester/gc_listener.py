import threading, socket, time

from GameControlData import GameControlData
from GameControlData import GAMECONTROLLER_DATA_PORT

class GameController(threading.Thread):
    """
    The GameController class is used to receive the infos of a game.
    If new data was received, it gets parsed and published on the blackboard.
    """

    def __init__(self, source=None):
        """
        Constructor.
        Inits class variables and establish the udp socket connection to the GameController.
        """
        super().__init__()

        self.__cancel = threading.Event()

        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.socket.bind(('', GAMECONTROLLER_DATA_PORT))
        self.socket.settimeout(5)  # in sec

        self.__source = str(source) if source is not None else None

    def run(self):
        """
        Main method of this thread.
        It listens on the socket for incoming GameController messages, parses them and publishes it on the blackboard.

        :return: nothing
        """
        print("Listen to GameController {}".format(self.__source))
        while not self.__cancel.is_set():
            try:
                # receive GC data
                data, address = self.socket.recvfrom(8192)
                # only if we recieved somethind, parse & publish message
                if len(data) > 0:
                    if self.__source is None or address[0] == self.__source:
                        data = GameControlData(data)
                        print(data)
                    else:
                        print("Got data from a invalid source: {} != {}".format(address[0], self.__source))

            except socket.timeout:
                print("Not connected to GameController?")
                self.message = None
                continue
            except Exception as ex:
                # Unknown exception
                self.message = None
                print("Unknown exception: {}".format(ex))
                continue

        self.socket.close()
        print("GameController thread finished.")

    def cancel(self):
        """
        Cancels this GameController thread.

        :return: nothing
        """
        self.__cancel.set()
        self.socket.settimeout(0)
        # send dummy in order to 'interrupt' receiving socket
        self.socket.sendto(b'', ('', GAME_CONTROLLER_PORT))

    def setSource(self, source):
        """
        Sets the source ip address of the GameController, others are ignored.
        :param source: the new source ip address
        :return: nothing
        """
        self.__source = str(source) if source is not None else None
        
        
if __name__ == "__main__":
    gc = GameController()
    gc.start()
    
    try:
        while True:
            time.sleep(1)
    except (KeyboardInterrupt, SystemExit):
        print("Shutting down ...")
    
    # stop and wait for exit
    gc.cancel()
    gc.join()