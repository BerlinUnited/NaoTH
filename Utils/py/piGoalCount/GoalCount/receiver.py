#!/usr/bin/env python3
# -*- coding: utf8 -*-

import sys, os, socket
import threading
from threading import Event
import time


naoth_path = os.path.abspath(os.path.dirname(__file__)+ '/../')
if naoth_path not in sys.path:
    sys.path.append(naoth_path)

from naoth.SPLMessage import SPLMessage


class TeamCommReceiver(threading.Thread):

    def __init__(self, host='localhost', port=10004):
        threading.Thread.__init__(self)

        # create dgram udp socket
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.socket.bind(('', port))
        except socket.error:
            raise Exception('Failed to create socket')

        # init some vars
        self.host = host
        self.port = port
        self.loop_control = Event()
        
        # creating message only once and update the data ...
        self.data = {}
        
        self.time_playing = 0

    def run(self):
        while not self.loop_control.is_set():
            try:
                # Send message
                data, addr = self.socket.recvfrom(1024)  # buffer size is 1024 bytes
                msg = SPLMessage(data=data)
                self.data[addr[0]] = msg
                self.time_playing = msg.data.bdrPlayerState.time_playing
                
                print("received from: " + addr[0])
                # wait to send new message
                #time.sleep(self.delay)
            except socket.error as msg:
                print('Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
                self.loop_control.set()

    def stop(self):
      self.loop_control.set()
      self.join()

if __name__ == '__main__':
    print('starting ...')
    r = TeamCommReceiver()
    r.start()
    time.sleep(10)
    r.stop()
    #r.join()
    print('finished!')
