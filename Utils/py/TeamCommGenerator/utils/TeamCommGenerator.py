import json
import threading
import socket
import time

from utils.TeamCommReceiver import TeamCommReceiver
from utils.TeamCommSender import TeamCommSender


class TeamCommGenerator(threading.Thread):
    def __init__(self, host='localhost', port=10004, delay=0.4):
        threading.Thread.__init__(self)

        # indicator whether thread is/should running
        self.cancel = threading.Event()
        # whether the auto timestamp update should be activated or not (default 'yes')
        self.update_timestamp = threading.Event()
        self.update_timestamp.set()
        # whether the NTP should be used or not (default 'yes')
        self.use_ntp = threading.Event()
        self.use_ntp.set()

        # init some vars
        self.host = host
        self.port = port
        self.delay = delay

        self.receiver = TeamCommReceiver(self.cancel, self.host, self.port)
        self.sender = TeamCommSender(self.cancel, self.host, self.port, self.delay)


    def __repr__(self):
        return "Values of the generated messages (for {}:{}):\n{}".format(self.host, self.port, str(self.sender.msg))

    def run(self):
        self.receiver.start()
        self.sender.start()

        while not self.cancel.is_set():
            # update timestamp of messages
            if self.update_timestamp.is_set():
                self.sender.msg.data.timestamp = int(time.time() * 1000)

            # clears the ntp request "buffer"
            del self.sender.msg.data.ntpRequest[:]

            # only if activated fill the ntp request buffer
            if self.use_ntp.is_set():
                for p in self.receiver.data:
                    if self.sender.msg.playerNumber != self.receiver.data[p][1].playerNumber:
                        ntp = self.sender.msg.data.ntpRequest.add()
                        ntp.playerNum = self.receiver.data[p][1].playerNumber
                        ntp.sent = self.receiver.data[p][1].data.timestamp
                        ntp.received = self.receiver.data[p][0]
            time.sleep(self.delay)

        # close receiving socket
        try:
            self.receiver.socket.shutdown(socket.SHUT_RDWR)
        except Exception as e:
            # ignore exception
            pass
        finally:
            self.receiver.socket.close()
        # join threads
        self.receiver.join()
        self.sender.join()

    def getMessageFields(self):
        return list(self.sender.msg.__dict__.keys()) + list(self.sender.msg.data.DESCRIPTOR.fields_by_name.keys())

    def hasMessageField(self, field):
        return field in self.sender.msg.__dict__ or field in self.sender.msg.data.DESCRIPTOR.fields_by_name

    def setMessageField(self, field, value):
        parent, field = self.__getParent(field)
        # we got the "right" parent container
        if parent is not None:
            try:
                value = self.convertToType(type(getattr(parent, field)),value)
                if value is not None:
                    setattr(parent, field, value)
                else:
                    print("*** Couldn't set value, not a primitive value type!")
            except Exception as e:
                print("*** An error occured: ", e)
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
        if field in self.sender.msg.__dict__:
            return self.sender.msg
        elif field in self.sender.msg.data.DESCRIPTOR.fields_by_name:
            return self.sender.msg.data
        return None

    def convertToType(self, type, value):
        """Convert the given value to the specified type.
        Returns 'None', if we haven't a converting rule."""
        if type == int:
            return int(value)
        elif type == float:
            return float(value)
        elif type == bool:
            return bool(json.loads(value.lower()))
        elif type == str:
            return value
        return None

    def updateTimestamp(self, b:bool):
        if b:
            self.update_timestamp.set()
        else:
            self.update_timestamp.clear()

    def useNTP(self, b:bool):
        if b:
            self.use_ntp.set()
        else:
            self.use_ntp.clear()
