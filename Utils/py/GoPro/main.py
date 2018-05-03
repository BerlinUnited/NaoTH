#!/usr/bin/env python3
# -*- coding: utf8 -*-

'''
    Using the 'GoPro API for Python' to communicate with the GoPro:
    https://github.com/KonradIT/gopro-py-api
'''

import sys
import os
import tempfile
import socket
import traceback
import argparse
import logging
import logging.handlers
import threading
import json
import time

import goprocam
from utils import Logger, Daemonize, Network, GoPro, GameController
# , GameController, GoPro,


def parseArguments():
    use_config = '-c' in sys.argv or '--config' in sys.argv
    parser = argparse.ArgumentParser(description='Starts a GoPro controller (on linux!)', epilog="Example: {} -n 10.0.4.255 -p 10004".format(sys.argv[0]))
    parser.add_argument('-v', '--verbose', action='store_true', help='activate verbose mode')
    parser.add_argument('-c', '--config', action='store_true', help='use config for network setup')
    parser.add_argument('-q', '--quiet', action='store_true', help="doesn\'t print out anything, except errors; '-v' is ignored!")
    parser.add_argument('-b', '--background', action='store_true', help='autom. forks to background, no "status" will be printed out')
    parser.add_argument('-d', '--device', action='store', help='the device used to connect to the GoPro wifi network (eg. "wifi0")')
    parser.add_argument('-s', '--ssid',   action='store', required=not use_config, help='the SSID name of the GoPro wifi network (eg. "NAOCAM")')
    parser.add_argument('-p', '--passwd', action='store', required=not use_config, help='the password of the GoPro wifi network')
    parser.add_argument('--syslog', action='store_true', help='writes log to system log (too)')
    parser.add_argument('-r', '--retries', action='store', type=int, default=-1, help='How many retries should be attempted to connect to wifi before giving up.')
    parser.add_argument('-m', '--max-time', action='store', type=int, default=600, help='How many seconds should be continued to record, after playing time expired and "finished" state wasn\'t set. (precaution to prevent endless recording!; default: 600s)')
    parser.add_argument('-i', '--ignore', action='store_true', help='Ignores the "max time" option - possible infinity recording, if "finished" state isn\'t set.')
    parser.add_argument('-gc', '--check-gc', action='store_true', help='Tries to listen to GameController and print its state (for debugging).')

    return parser.parse_args()

def main():

    loopControl = threading.Event()

    gopro = GoPro(args.background or args.quiet, args.ignore, args.max_time)
    gopro.start()

    gameController = GameController()
    gameController.start()

    network = Network(args.device, args.ssid, args.passwd, args.retries)
    network.start()

    print("...")

    try:
        loopControl.wait()
    except KeyboardInterrupt as e:
        print("Exit.")

    network.cancel.set()
    network.join()
    gameController.cancel.set()
    gameController.socket.settimeout(0)
    gameController.join()
    gopro.cancel.set()
    gopro.join()


def checkGameController(loopControl:threading.Event):
    # listen to gamecontroller
    logger.info("Listen to GameController")
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    s.bind(('', 3838))
    s.settimeout(5)  # in sec

    gc_data = GameControlData()

    while not loopControl.is_set():
        try:
            try:
                # receive GC data
                message, address = s.recvfrom(8192)
                gc_data.unpack(message)
            except socket.timeout:
                logger.warning("Not connected to GameController?")
                continue
            except Exception as ex:
              print(ex)
              continue

            print(gc_data)
            # check if one team is 'invisible'
            if any([t.teamNumber == 0 for t in gc_data.team]):
                print("-- INVISIBLES are playing! --\n")

            previous_state = gc_data.gameState
        except (KeyboardInterrupt, SystemExit):
            logger.debug("Interrupted or Exit")
            print("")  # intentionally print empty line
            loopControl.set()
        except:
            traceback.print_exc()
    # close socket
    s.close()


if __name__ == '__main__':
    if not sys.platform.startswith('linux'):
        sys.stderr.write("Only linux based systems are currently supported!")
        exit(1)

    # define vars
    tempdir = tempfile.gettempdir()
    name = 'pyGoPro'  # os.path.basename(sys.argv[0])
    # check for existing lock file and running process
    lock_file = os.path.join(tempdir, name + '.lock')

    # args = { 'device': 'wifi0', 'ssid': 'NAOCAM', 'password':'a1b0a1b0a1' }
    args = parseArguments()

    Logger.setupLogger(args.quiet, args.verbose, args.syslog)

    if args.check_gc:
        loopControl = threading.Event()
        checkGameController(loopControl)
    else:
        # use config for network setup
        if args.config:
            try:
                import config
                args.ssid = config.ssid
                args.passwd = config.passwd
                args.retries = config.retries
            except:
                # no config available!
                Logger.error("No config available OR invalid config!")
                exit(2)

        daemon = Daemonize(app=name, pid=lock_file, action=main, logger=Logger.logger, foreground=not args.background)
        daemon.start()
