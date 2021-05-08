#!/usr/bin/env python3
# -*- coding: utf8 -*-

import socket
import sys
import threading
import time
from threading import Event

from naoth.spl import SPLMessage


class TeamCommSender(threading.Thread):
    def __init__(self, loop_control:Event, host='localhost', port=10004, delay=0.4):
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

        # init message
        self.msg = SPLMessage()
        self.msg.teamNumber = 4
        self.msg.data.key = "naoth"

        self.loop_control = loop_control

    def __repr__(self):
        return "Values of the generated messages (for {}:{}):\n{}".format(self.host, self.port, str(self.msg))

    def run(self):
        while not self.loop_control.is_set():
            try:
                # Send message
                self.socket.sendto(self.msg.pack(), (self.host, self.port))
                # wait to send new message
                time.sleep(self.delay)
            except socket.error as msg:
                print('Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
                self.loop_control.set()


if __name__ == '__main__':
    print('starting ...')
    loop_control = Event()
    s = TeamCommSender(loop_control, '10.0.255.255')
    s.start()
    time.sleep(10)
    loop_control.set()
    s.join()
    print('finished')