import socket

from . import Event

class StatusMonitor():
    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        Event.registerListener(self)

    def sendMessage(self, msg):
        self.sock.sendto(msg.encode(), ("localhost", 8000))

    def setNoGoProNetwork(self, evt:Event.NetworkNotAvailable):
        self.sendMessage('{{"blue":"blink", "delay":"0.1", "time":"{}"}}'.format(5))

    def setConnectingToGoPro(self, evt:Event.GoproConnecting):
        self.sendMessage('{{"blue":"blink", "time":"{}"}}'.format(11))

    def setConnectedToGoPro(self, evt:Event.GoproConnected):
        self.sendMessage('{{"blue":"on", "time":"{}"}}'.format(2))

    def setReceivedMessageFromGC(self, evt:Event.GameControllerMessage):
        if not all([t.teamNumber > 0 for t in evt.message.team]):
            self.sendMessage('{{"green":"blink", "time":"{}"}}'.format(5))
        else:
            self.sendMessage('{{"green":"on", "time":"{}"}}'.format(5))

    def setDidntReceivedMessageFromGC(self, evt:Event.GameControllerTimedout):
        self.sendMessage('{{"green":"off", "time":"{}"}}'.format(1))

    def setRecordingOn(self, delay=1):
        self.sendMessage('{{"red":"blink", "time":"{}"}}'.format(delay))

    def setRecordingOff(self, delay=1):
        self.sendMessage('{{"red":"off", "time":"{}"}}'.format(delay))
