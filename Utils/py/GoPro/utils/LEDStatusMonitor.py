import socket
import threading
import time

from . import Event

class LedStatusMonitor(threading.Thread):

    def __init__(self):
        super().__init__()
        Event.registerListener(self)

        self.__cancel = threading.Event()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        self.network = 0
        self.gopro = 0
        self.gc = 0

    def run(self):
        while not self.__cancel.is_set():
            # handle the blue led
            if self.network == 0:
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
                if self.gopro <= 1:
                    # ... but not to the gopro itself
                    self.sendMessage('{{"blue":"blink", "time":"{}"}}'.format(1))
                elif self.gopro >= 2:
                    # ... and connected to the gopro
                    if self.gopro == 3:
                        # ..., but gopro has no sdcard
                        self.sendMessage('{{"blue":"blink", "time":"{}"}}'.format(1))
                    else:
                        # ... and is ready to record
                        self.sendMessage('{{"blue":"on", "time":"{}"}}'.format(1))

            # handle the green led
            if self.gc == 0:
                # nothing received from GameController
                self.sendMessage('{{"green":"off", "time":"{}"}}'.format(1))
            elif self.gc == 1:
                # only one team in the game (the other is "INVISIBLE")
                self.sendMessage('{{"green":"blink", "time":"{}"}}'.format(1))
            elif self.gc == 2:
                # GameController is ready
                self.sendMessage('{{"green":"on", "time":"{}"}}'.format(1))

            # handle the red led
            if self.gopro == 4:
                # gopro is recording
                self.sendMessage('{{"red":"blink", "time":"{}"}}'.format(1))
            elif self.gopro == 3:
                # gopro has no sdcard!
                self.sendMessage('{{"red":"blink", "time":"{}"}}'.format(1))
            else:
                # gopro is NOT recording
                self.sendMessage('{{"red":"off", "time":"{}"}}'.format(1))

            # update every 0.5 seconds
            time.sleep(0.5)

    def cancel(self):
        self.__cancel.set()
        Event.unregisterListener(self)

    def network_connecting(self, evt:Event.NetworkConnecting):
        self.network = 2

    def network_connected(self, evt:Event.NetworkConnected):
        self.network = 3

    def network_disconnected(self, evt:Event.NetworkDisconnected):
        self.network = 1

    def network_unavailable(self, evt:Event.NetworkNotAvailable):
        self.network = 0

    def gopro_connecting(self, evt:Event.GoproConnecting):
        self.gopro = 1

    def gopro_connected(self, evt:Event.GoproConnected):
        self.gopro = 2

    def gopro_disconnected(self, evt: Event.GoproDisconnected):
        self.gopro = 0

    def gopro_recording(self, evt:Event.GoproStartRecording):
        self.gopro = 4

    def gopro_stopped(self, evt:Event.GoproStopRecording):
        self.gopro = 2

    def gopro_nocard(self, evt:Event.GoproNoSdcard):
        self.gopro = 3

    def gopro_card_inserted(self, evt:Event.GoproSdcardInserted):
        self.gopro = 2

    def gc_message(self, evt:Event.GameControllerMessage):
        if not all([t.teamNumber > 0 for t in evt.message.team]):
            self.gc = 1
        else:
            self.gc = 2

    def gc_timeout(self, evt:Event.GameControllerTimedout):
        self.gc = 0

    def sendMessage(self, msg):
        self.sock.sendto(msg.encode(), ("localhost", 8000))
