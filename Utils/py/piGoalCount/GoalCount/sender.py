#!/usr/bin/env python3
# -*- coding: utf8 -*-

import sys
import threading
import socket
import time
from threading import Event
import os

naoth_path = os.path.abspath(os.path.dirname(__file__)+ '/../')
if naoth_path not in sys.path:
    sys.path.append(naoth_path)

from naoth import TeamMessage_pb2
from naoth.SPLMessage import SPLMessage


class TeamCommSender(threading.Thread):
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

        # init message
        self.msg = SPLMessage()
        self.msg.teamNumber = 4
        self.msg.playerNumber = 0
        self.msg.data.key = "naoth"
        self.msg.data.robotState = TeamMessage_pb2.penalized

        self.loop_control = Event()

    def __repr__(self):
        return "Values of the generated messages (for {}:{}):\n{}".format(self.host, self.port, str(self.msg))

    def run(self):
        while not self.loop_control.is_set():
            try:
                # Send message
                self.msg.data.timestamp = int(time.monotonic() * 1000)
                #print('send message:', self.host, self.port)
                self.socket.sendto(self.msg.pack(), (self.host, self.port))
                #print(self.msg.data.message)
                # wait to send new message
                time.sleep(self.delay)
            except socket.error as msg:
                print('Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
                self.running = False

    def set_score(self, goalsLeft, goalsRight, time):
        self.msg.data.bdrPlayerState.goalsLeft = goalsLeft
        self.msg.data.bdrPlayerState.goalsRight = goalsRight
        self.msg.data.bdrPlayerState.time_playing = time     
                
    def stop(self):
      self.loop_control.set()
      self.join()
               
               
if __name__ == '__main__':
    print('starting ...')
    loop_control = Event()
    s = TeamCommSender(loop_control, '10.0.4.255')
    s.start()
    time.sleep(10)
    loop_control.set()
    s.join()
    print('finished')