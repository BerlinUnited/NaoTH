import sys
import threading
import socket
import time
from naoth.SPLMessage import SPLMessage

class TeamCommGenerator(threading.Thread):
    def __init__(self, host='localhost', port=10004, delay=0.4):
        threading.Thread.__init__(self)

        # create dgram udp socket
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        except socket.error:
            print('Failed to create socket')
            sys.exit()

        # init some vars
        self.host = host
        self.port = port
        self.delay = delay

        # creating message only once and update the data ...
        self.msg = SPLMessage()

        # indicator whether thread is/should running
        self.running = False

    def __repr__(self):
        return "Values of the generated messages (for {}:{}):\n{}".format(self.host, self.port, str(self.msg))

    def run(self):
        self.running = True
        #print(self.name, " is running")
        while (self.running):
            try:
                # Send message
                self.socket.sendto(self.msg.pack(), (self.host, self.port))
                # wait to send new message
                time.sleep(self.delay)
            except socket.error as msg:
                print('Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
                self.running = False

    def getMessageFields(self):
        return list(self.msg.__dict__.keys()) + list(self.msg.data.DESCRIPTOR.fields_by_name.keys())

    def hasMessageField(self, field):
        return field in self.msg.__dict__ or field in self.msg.data.DESCRIPTOR.fields_by_name

    def setMessageField(self, field, value):
        parent, field = self.__getParent(field)
        # we got the "right" parent container
        if parent is not None:
            value = self.convertToType(type(getattr(parent, field)),value)
            if value is not None:
                setattr(parent, field, value)
            else:
                print("*** Couldn't set value, not a primitive value type!")
        else:
            print("*** Unknown message field")

    def getMessageField(self, field):
        parent, field = self.__getParent(field)
        return getattr(parent, field)

    def __getParent(self, field):
        # field contains '.' -> nested field access
        if '.' in field:
            fields = field.split('.')
            parent = self.__getField(fields[0])
            field = fields[-1]
            if parent is not None:
                for f in fields[:-1]:
                    if hasattr(parent, f):
                        parent = getattr(parent, f)
                    else:
                        return (None, None)
                return (parent, field)
        else:
            return (self.__getField(field), field)
        return (None, None)

    def __getField(self, field):
        if field in self.msg.__dict__:
            return self.msg
        elif field in self.msg.data.DESCRIPTOR.fields_by_name:
            return self.msg.data
        return None

    def convertToType(self, type, value):
        """Convert the given value to the specified type.
        Returns 'None', if we haven't a converting rule."""
        if type == int:
            return int(value)
        elif type == float:
            return float(value)
        elif type == bool:
            return bool(value)
        elif type == str:
            return value
        return None
