import inspect


class Event:
    pass

######################################################
## BEGIN Events

######################################################
## Network Events
######################################################

class NetworkConnected(Event):
    pass

class NetworkDisconnected(Event):
    pass

class NetworkNotAvailable(Event):
    pass

######################################################
## GameController Events
######################################################

class GameControllerTimedout(Event):
    pass

class GameControllerMessage(Event):
    def __init__(self, msg):
        self.message = msg

######################################################
## GoPro Events
######################################################

class GoproConnecting(Event):
    pass

class GoproConnected(Event):
    pass

class GoproDisconnected(Event):
    pass

class GoproStartRecording(Event):
    pass

class GoproStopRecording(Event):
    def __init__(self, file):
        self.file = file

class GoproNoSdcard(Event):
    pass

class GoproSdcardInserted(Event):
    pass

######################################################
## Other Events
######################################################



## END Events
######################################################

class Manager:

    def __init__(self):

        self.listeners = {}

    def registerListener(self, l: object):
        callbacks = inspect.getmembers(l, self.__checkMethods)
        for c in callbacks:
            t = self.__getEventType(c[1])
            if t not in self.listeners:
                self.listeners[t] = []
            self.listeners[t].append(c[1])

    def __getEventType(self, m):
        if m and inspect.ismethod(m):
            s = inspect.signature(m)
            if len(s.parameters) == 1:
                p = list(s.parameters.values())
                if Event in p[0].annotation.__bases__:
                    return p[0].annotation
        return None

    def __checkMethods(self, m):
        return self.__getEventType(m) is not None

    def unregisterListener(self, l: object):
        callbacks = inspect.getmembers(l, self.__checkMethods)
        for c in callbacks:
            t = self.__getEventType(c[1])
            if t in self.listeners:
                self.listeners[t].remove(c[1])

    def fire(self, event: Event):
        if event.__class__ in self.listeners:
            for l in self.listeners[event.__class__]:
                l(event)


# make public module methods
manager = Manager()
registerListener = manager.registerListener
unregisterListener = manager.unregisterListener
fire = manager.fire