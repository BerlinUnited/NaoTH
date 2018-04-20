import socket
import threading
from threading import Event
import time

from naoth.SPLMessage import SPLMessage


class TeamCommReceiver(threading.Thread):

    def __init__(self, loop_control:Event, host='localhost', port=10004):
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
        self.loop_control = loop_control

        # creating message only once and update the data ...
        self.msg = SPLMessage()

    def run(self):
        while not self.loop_control.is_set():
            try:
                # Send message
                data, addr = self.socket.recvfrom(1024)  # buffer size is 1024 bytes
                print('received from', addr)
                msg = SPLMessage(data=data)
                print(msg)

                # wait to send new message
                #time.sleep(self.delay)
            except socket.error as msg:
                print('Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
                self.loop_control.set()


if __name__ == '__main__':
    print('starting ...')
    loop_control = Event()
    r = TeamCommReceiver(loop_control)
    r.start()
    time.sleep(10)
    loop_control.set()
    r.join()
    print('finished!')
