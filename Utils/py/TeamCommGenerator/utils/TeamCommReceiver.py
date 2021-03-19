#!/usr/bin/env python3
# -*- coding: utf8 -*-

import sys, os, socket
import threading
from threading import Event
import time

from naoth.spl import SPLMessage


class TeamCommReceiver(threading.Thread):

    def __init__(self, loop_control:Event, host='localhost', port=10004):
        threading.Thread.__init__(self)

        # create dgram udp socket
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.socket.bind(('', port))
        except socket.error:
            raise Exception('Failed to create socket')

        # init some vars
        self.host = host
        self.port = port
        self.loop_control = loop_control
        
        # creating message only once and update the data ...
        self.data = {}

    def run(self):
        while not self.loop_control.is_set():
            try:
                # Send message
                data, addr = self.socket.recvfrom(1024)  # buffer size is 1024 bytes
                # make sure we got data
                if len(data) > 0:
                    self.data[addr[0]] = (int(time.time() * 1000), SPLMessage(data=data))
            except socket.error as msg:
                print('Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
                self.loop_control.set()

if __name__ == '__main__':
    print('starting ...')
    loop_control = Event()
    r = TeamCommReceiver(loop_control, "192.168.13.255", 10004)
    r.start()

    for i in range(10):
        print(r.data)
        time.sleep(1)
    loop_control.set()
    r.join()
    print('finished!')
