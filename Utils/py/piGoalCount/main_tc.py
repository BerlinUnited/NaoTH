#!/usr/bin/env python3
# -*- coding: utf8 -*-

import random
import threading

import time

import teamcomm

if __name__ == '__main__':
    print("started TC @", time.time())

    loop_control = threading.Event()

    receiver = teamcomm.TeamCommReceiver(loop_control)
    receiver.start()

    sender = teamcomm.TeamCommSender(loop_control, '10.0.255.255')
    sender.start()

    try:
        while True:
            # some 'pseudo-updates'
            sender.msg.data.batteryCharge = random.random()
            sender.msg.data.temperature = random.random()
            sender.msg.data.bodyID = "receiving messes from " + ",".join(receiver.data.keys())

            time.sleep(1)
    except KeyboardInterrupt:
        # stop sender & receiver
        loop_control.set()
        receiver.join(10)
        sender.join(10)

    print("finished TC")