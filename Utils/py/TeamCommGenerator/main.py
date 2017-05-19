#!/usr/bin/env python3

import socket
import sys
import threading
from struct import *
import time
import argparse
import signal

# protobuf
sys.path.append('../../pyLogEvaluator')
from Representations_pb2 import *


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
        print(self.name, " is running")
        while (self.running):
            try:
                # Send message
                self.socket.sendto(self.msg.pack(), (self.host, self.port))
                # wait to send new message
                time.sleep(self.delay)
            except socket.error as msg:
                print('Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
                self.running = False

    def hasMessageField(self, field):
        return field in self.msg.__dict__ or field in self.msg.data.DESCRIPTOR.fields_by_name

    def setMessageField(self, field, value):
        parent = None
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
                        parent = None
                        break
        else:
            parent = self.__getField(field)
        # we got the "right" parent container
        if parent is not None:
            setattr(parent, field, self.convertToType(type(getattr(parent, field)),value))
        else:
            raise Exception("Unknown message field")

    def __getField(self, field):
        if field in self.msg.__dict__:
            return self.msg
        elif field in self.msg.data.DESCRIPTOR.fields_by_name:
            return self.msg.data
        return None

    def convertToType(self, type, value):
        """Convert the given value to the specified type.
        Returns the value 'as is', if we have not converting rule."""
        if type == int:
            return int(value)
        elif type == float:
            return float(value)
        elif type == bool:
            return bool(value)
        return value

class SPLMessage(Struct):
    """Representation of the standard SPLMessage."""

    SPL_STANDARD_MESSAGE_STRUCT_HEADER = b'SPL '
    SPL_STANDARD_MESSAGE_STRUCT_VERSION = 6
    SPL_STANDARD_MESSAGE_DATA_SIZE = 780
    SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS = 5

    def __init__(self, teamnumber=0, playernumber=0):
        super(SPLMessage, self).__init__('4s3b?12f6B2h2bh')
        self.playerNumber = playernumber
        self.teamNumber = teamnumber
        self.fallen = False
        self.pose = Pose2D(0.0, 0.0, 0.0)  # x, y, r | +/-4500, +/-3000
        self.walkingTo = Vector2(0.0, 0.0)
        self.shootingTo = Vector2(0.0, 0.0)
        self.ballAge = -1
        self.ballPosition = Vector2(0.0, 0.0)
        self.ballVelocity = Vector2(0.0, 0.0)
        self.suggestion = [0 for x in range(self.SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS)]
        self.intention = 0
        self.averageWalkSpeed = 200  # see TeamCommSender
        self.maxKickDistance = 3000  # see TeamCommSender
        self.currentPositionConfidence = 100
        self.currentSideConfidence = 100

        self.data = BUUserTeamMessage()
        self.data.key = "naoth"
        self.data.timestamp = 0
        self.data.bodyID = 'unknown'
        self.data.wasStriker = False
        self.data.timeToBall = 0  # sys.maxsize
        self.data.isPenalized = False
        self.data.batteryCharge = 0.0
        self.data.temperature = 0.0

        self.numOfDataBytes = self.data.ByteSize()

    def pack(self):
        return Struct.pack(self,
                           self.SPL_STANDARD_MESSAGE_STRUCT_HEADER,
                           self.SPL_STANDARD_MESSAGE_STRUCT_VERSION,
                           self.playerNumber,
                           self.teamNumber,
                           self.fallen,
                           *self.pose.__dict__.values(),
                           *self.walkingTo.__dict__.values(),
                           *self.shootingTo.__dict__.values(),
                           self.ballAge,
                           *self.ballPosition.__dict__.values(),
                           *self.ballVelocity.__dict__.values(),
                           *self.suggestion,
                           self.intention,
                           self.averageWalkSpeed,
                           self.maxKickDistance,
                           self.currentPositionConfidence,
                           self.currentSideConfidence,
                           self.data.ByteSize(),
                           ) + self.data.SerializeToString()

    def __repr__(self):
        """Returns all 'active' message fields as string."""
        result = ""
        for attr in self.__dict__:
            if attr == "data":
                fields = self.__dict__[attr].DESCRIPTOR.fields_by_name
                for custom_attr in fields:
                    # do not show deprecated custom fields!
                    if fields[custom_attr].GetOptions().deprecated:
                        continue
                    result += "\t" + custom_attr + " = " + str(getattr(self.__dict__[attr], custom_attr)) + "\n"
            else:
                result += "\t" + attr + " = " + str(self.__dict__[attr]) + "\n"
        return result

class Vector2(object):
    def __init__(self, x, y):
        self.__dict__ = {'x':x, 'y':y}
    def __repr__(self):
        return str(self.__dict__)

class Vector3(object):
    def __init__(self, x, y, z):
        self.__dict__ = {'x':x, 'y':y, 'z':z}
    def __repr__(self):
        return str(self.__dict__)

class Pose2D(object):
    def __init__(self, x, y, r):
        self.__dict__ = {'x':x, 'y':y, 'r':r}
    def __repr__(self):
        return str(self.__dict__)

class CommandParser:
    def __init__(self, thread):
        self.thread = thread
        # create the top-level parser
        self.parser = argparse.ArgumentParser(add_help=False)
        subparsers = self.parser.add_subparsers()
        # help command
        parser_help = subparsers.add_parser('help', aliases=['h'])
        parser_help.set_defaults(func=self.help)
        # quit command
        parser_quit = subparsers.add_parser('quit', aliases=['q', 'exit'])
        parser_quit.set_defaults(func=self.quit)
        # info command
        parser_info = subparsers.add_parser('info', aliases=['i'])
        parser_info.set_defaults(func=self.info)
        # set command
        parser_set = subparsers.add_parser('set', aliases=['s'])
        parser_set.add_argument('field', default=None)
        parser_set.add_argument('value', default=None)
        parser_set.set_defaults(func=self.set)

    def parse(self, instr):
        # parse the args and call whatever function was selected
        try:
            cmd = self.parser.parse_args(instr.split())
            cmd.func(cmd)
        except Exception as e:
            print(e)
        except:
            print("Unknown command")

    def quit(self, args):
        print("quitting!")
        # stopping & join threads
        self.thread.running = False
        self.thread.join()

    def help(self, args):
        self.parser.print_help()
        #self.parser.print_usage()

    def info(self, args):
        print(self.thread)

    def set(self, args):
        self.thread.setMessageField(args.field, args.value)


def parseArguments():
    parser = argparse.ArgumentParser(description='Starts a TeamCommMessage generator', epilog="Example: {} -n 10.0.4.255 -p 10004".format(sys.argv[0]))
    parser.add_argument('-n', '--net', action='store', default='localhost', help='the host / net, where the messages are send to; e.g. 10.0.4.255; default: localhost')
    parser.add_argument('-p', '--port', action='store', default=10000, type=int, help='the port number where the messages are send to; default: 10004')
    parser.add_argument('-d', '--delay', action='store', default=0.4, type=float, help='sets the time between two messages (in seconds)), default: 0.4')

    return parser.parse_args()

def signalHandler(sig, frm):
    global tc
    if signal.SIGINT == sig:
        tc.running = False

#
if '__main__' == __name__:
    print("Starting ...")

    args = parseArguments()

    signal.signal(signal.SIGINT, signalHandler)

    tc = TeamCommGenerator(args.net, args.port, args.delay)  # "10.0.4.255"

    tc.start()

    cmdParser = CommandParser(tc)
    while tc.is_alive() and tc.running:
        instr = input(">>> ").strip()
        if len(instr) > 0:
            cmdParser.parse(instr)

    tc.join()

    print("bye, bye!")
