#!/usr/bin/env python3
# -*- coding: utf8 -*-

'''
    Using the 'GoPro API for Python' to communicate with the GoPro:
    https://github.com/KonradIT/gopro-py-api
'''

import sys
import socket
import traceback
import argparse
import logging
import threading
import json
import time

from GameControlData import GameControlData
from goprocam import GoProCamera, constants
import Network

LOGGER_NAME = __name__
LOGGER_FMT  = '%(levelname)s: %(message)s'
LOGGER_FMT_CHILD = '%(levelname)s[%(name)s]: %(message)s'

def parseArguments():
    parser = argparse.ArgumentParser(description='Starts a GoPro controller (on linux!)', epilog="Example: {} -n 10.0.4.255 -p 10004".format(sys.argv[0]))
    parser.add_argument('-v', '--verbose', action='store_true', help='activate verbose mode')
    parser.add_argument('-q', '--quiet', action='store_true', help="doesn\'t print out anything, except errors; '-v' is ignored!")
    parser.add_argument('-b', '--background', action='store_true', help='if running in background mode, no "status" will be printed out')
    parser.add_argument('-d', '--device', action='store', help='the device used to connect to the GoPro wifi network (eg. "wifi0")')
    parser.add_argument('-s', '--ssid',   action='store', required=True, help='the SSID name of the GoPro wifi network (eg. "NAOCAM")')
    parser.add_argument('-p', '--passwd', action='store', required=True, help='the password of the GoPro wifi network')

    return parser.parse_args()

def setupLogger(quiet:bool = False, verbose:bool = False):
    lh = logging.StreamHandler()
    lh.setFormatter(LogFormatter())

    lvl = logging.ERROR if quiet else (logging.DEBUG if verbose else logging.INFO)
    logging.basicConfig(level=lvl, format=LOGGER_FMT)
    logger = logging.getLogger(LOGGER_NAME)
    logger.propagate = False
    logger.addHandler(lh)

    Network.logger.addHandler(lh)
    Network.logger.propagate = False

    return logger

def setCamVideoMode(cam):
    logger.debug("Set GoPro to 'video' mode")
    cam.mode(constants.Mode.VideoMode)
    # wait for the command to be executed
    time.sleep(0.5)


class LogFormatter(logging.Formatter):
    def format(self, record):
        if record.name != LOGGER_NAME:
            self._style._fmt = LOGGER_FMT_CHILD
        else:
            self._style._fmt = LOGGER_FMT

        return logging.Formatter.format(self, record) #super(LogFormatter, self).format(format)

class CamStatus(threading.Thread):
    def __init__(self, cam:GoProCamera.GoPro, interval:int=5):
        threading.Thread.__init__(self)
        self.cam = cam
        self.interval = interval
        self.status = { 'recording': False, 'mode': None }

        self.running = threading.Event()
        self.sleeper = threading.Event()

    def run(self):
        while not self.running.is_set():
            js = json.loads(self.cam.getStatusRaw())
            self.status['mode'] = self.cam.parse_value("mode", js[constants.Status.Status][constants.Status.STATUS.Mode])
            self.status['recording'] = js[constants.Status.Status][constants.Status.STATUS.IsRecording] == 1
            self.sleeper.wait(self.interval)

    def stop(self):
        self.running.set()
        self.sleeper.set()
        self.join()

    def __getitem__(self, item):
        return self.status[item]


if __name__ == '__main__':
    if not sys.platform.startswith('linux'):
        sys.stderr.write("Only linux based systems are currently supported!")
        exit(1)

    #args = { 'device': 'wifi0', 'ssid': 'NAOCAM', 'password':'a1b0a1b0a1' }
    # GP26329941 / cycle9210
    # GP26297683 / epic0546
    args = parseArguments()

    logger = setupLogger(args.quiet, args.verbose)

    logger.info("Setting up network")

    # check wifi device
    device = Network.getWifiDevice(args.device)
    if device is not None:
        logger.info("Using device %s", device)
        # check/connect to wifi network
        network = Network.connectToSSID(device, args.ssid, args.passwd)
        if network is not None:
            logger.info("Connected to %s", network)
            # get GoPro
            logger.info("Connecting to GoPro ...")
            cam = GoProCamera.GoPro()
            if cam.getStatusRaw():
                # set GoPro to video mode
                setCamVideoMode(cam)

                # listen to gamecontroller
                logger.info("Listen to GameController")
                s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
                s.bind(('', 3838))

                status = CamStatus(cam, 1)
                status.start()

                previous_state = GameControlData.STATE_INITIAL
                previous_output= ""
                output = ""
                gc_data = GameControlData()
                while True:
                    try:
                        # receive GC data
                        message, address = s.recvfrom(8192)
                        gc_data.unpack(message)

                        # handle output
                        if not args.background and not args.quiet:
                            output = "%s | %s | game state: %s |"%(status['mode'], "RECORDING!" if status['recording'] else "Not recording", gc_data.getGameState())
                            print("\r"+output + " "*(len(previous_output)-len(output)), end="", flush=True)

                        # handle game state changes
                        if not status['recording'] and (gc_data.gameState in [GameControlData.STATE_READY, GameControlData.STATE_SET, GameControlData.STATE_PLAYING]):
                            if status['mode'] != "Video":
                                setCamVideoMode(cam)
                            logger.debug("Start recording")
                            cam.shutter(constants.start)
                            time.sleep(1) # wait for the command to be executed
                        elif status['recording'] and not (gc_data.gameState in [GameControlData.STATE_READY, GameControlData.STATE_SET, GameControlData.STATE_PLAYING]):
                            logger.debug("Stop recording")
                            cam.shutter(constants.stop)
                            time.sleep(1)  # wait for the command to be executed

                        # update loop vars
                        previous_output = output
                        previous_state = gc_data.gameState
                    except (KeyboardInterrupt, SystemExit):
                        print("") # intentionally print empty line
                        break
                    except:
                        traceback.print_exc()
                # stop status listener
                status.stop()
                # close socket
                s.close()