#!/usr/bin/env python3
# -*- coding: utf8 -*-

'''
    Using the 'GoPro API for Python' to communicate with the GoPro:
    https://github.com/KonradIT/gopro-py-api
'''

import sys
import os
import tempfile
import atexit
import signal
import socket
import traceback
import argparse
import logging
import logging.handlers
import threading
import json
import time

from GameControlData import GameControlData
from goprocam import GoProCamera, constants
from daemonize import Daemonize
import Network

LOGGER_NAME = __name__
LOGGER_FMT  = '%(levelname)s: %(message)s'
LOGGER_FMT_CHILD = '%(levelname)s[%(name)s]: %(message)s'

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

def setupLogger(quiet:bool = False, verbose:bool = False, syslog:bool = False):
    lh = logging.StreamHandler()
    lh.setFormatter(LogFormatter())

    lvl = logging.ERROR if quiet else (logging.DEBUG if verbose else logging.INFO)
    logging.basicConfig(level=lvl, format=LOGGER_FMT)
    logger = logging.getLogger(LOGGER_NAME)
    logger.propagate = False
    logger.addHandler(lh)

    Network.logger.addHandler(lh)
    Network.logger.propagate = False

    if syslog:
        sh = logging.handlers.SysLogHandler(address='/dev/log')
        sh.setFormatter(logging.Formatter('GoPro:'+LOGGER_FMT))

        logger.addHandler(sh)
        Network.logger.addHandler(sh)

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

        return logging.Formatter.format(self, record)  # super(LogFormatter, self).format(format)


class CamStatus(threading.Thread):
    """Thread retrieves the current state of the camera and tries to keep the cam alive."""
    def __init__(self, cam: GoProCamera.GoPro, interval: int=5):
        threading.Thread.__init__(self)
        self.cam = cam
        self.interval = interval
        self.status = {'recording': False, 'mode': None}

        self.running = threading.Event()
        self.sleeper = threading.Event()

    def run(self):
        while not self.running.is_set():
            js = json.loads(self.cam.getStatusRaw())
            self.status['mode'] = self.cam.parse_value("mode", js[constants.Status.Status][constants.Status.STATUS.Mode])
            self.status['recording'] = js[constants.Status.Status][constants.Status.STATUS.IsRecording] == 1
            self.cam.power_on(self.cam._mac_address)
            self.sleeper.wait(self.interval)

    def stop(self):
        self.running.set()
        self.sleeper.set()
        self.join()

    def __getitem__(self, item):
        return self.status[item]


def main():
    loopControl = threading.Event()
    logger.info("Setting up network")
    # check wifi device
    device = Network.getWifiDevice(args.device)
    if device is not None:
        logger.info("Using device %s", device)
        try:
            attempts = 0
            while (args.retries == -1 or attempts < args.retries) and not loopControl.is_set():
                # check/connect to wifi network
                network = Network.connectToSSID(device, args.ssid, args.passwd)
                if network is not None:
                    logger.info("Connected to %s", network)
                    # connect to gopro and start main loop
                    main_gopro(loopControl)
                else:
                    # wait before retry to connect
                    time.sleep(10)
                attempts += 1
                logger.debug("Connecting network attempts: " + str(attempts))
        except KeyboardInterrupt as e:
            logger.debug("Interrupted ...")
            loopControl.set()


def main_gopro(loopControl: threading.Event):
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
        s.settimeout(5)  # in sec

        # status monitor and cam keep alive thread
        status = CamStatus(cam, 1)
        status.start()

        # start main loop
        main_loop(cam, status, s, loopControl)

        # stop status listener
        status.stop()
        # close socket
        s.close()


def main_loop(cam: GoProCamera.GoPro, cam_status: CamStatus, gc_socket: socket.socket, loopControl: threading.Event):
    previous_state = GameControlData.STATE_INITIAL
    previous_output = ""
    output = ""
    gc_data = GameControlData()
    while not loopControl.is_set():
        try:
            try:
                # receive GC data
                message, address = gc_socket.recvfrom(8192)
                gc_data.unpack(message)
            except socket.timeout:
                # didn't receive anything from GameController - stop recording (if necessary)
                if cam_status['recording']:
                    logger.warning("Not connected to GameController?")
                    stopRecording(cam)
                continue
            # handle output
            if not args.background and not args.quiet:
                output = "%s | %s | game state: %s |" % (
                    cam_status['mode'], "RECORDING!" if cam_status['recording'] else "Not recording", gc_data.getGameState())
                print("\r" + output + " " * (len(previous_output) - len(output)), end="", flush=True)

            # handle game state changes
            if not args.ignore and gc_data.secsRemaining < -args.max_time:
                # only stop, if we're still recording
                if cam_status['recording']:
                    stopRecording(cam)
            elif not cam_status['recording'] and (
                        gc_data.gameState in [GameControlData.STATE_READY, GameControlData.STATE_SET,
                                              GameControlData.STATE_PLAYING]):
                startRecording(cam, cam_status)
            elif cam_status['recording'] and not (
                        gc_data.gameState in [GameControlData.STATE_READY, GameControlData.STATE_SET,
                                              GameControlData.STATE_PLAYING]):
                stopRecording(cam)

            # just for debugging/logging
            if previous_state != gc_data.gameState:
                logger.debug("Changed game state to: %s", gc_data.getGameState())

            # update loop vars
            previous_output = output
            previous_state = gc_data.gameState
        except (KeyboardInterrupt, SystemExit):
            logger.debug("Interrupted or Exit")
            print("")  # intentionally print empty line
            loopControl.set()
        except:
            traceback.print_exc()


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

            print(gc_data)

            previous_state = gc_data.gameState
        except (KeyboardInterrupt, SystemExit):
            logger.debug("Interrupted or Exit")
            print("")  # intentionally print empty line
            loopControl.set()
        except:
            traceback.print_exc()
    # close socket
    s.close()


def startRecording(cam, cam_status):
    if cam_status['mode'] != "Video":
        setCamVideoMode(cam)
    logger.debug("Start recording")
    cam.shutter(constants.start)
    time.sleep(1)  # wait for the command to be executed


def stopRecording(cam):
    logger.debug("Stop recording")
    cam.shutter(constants.stop)
    time.sleep(1)  # wait for the command to be executed


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

    logger = setupLogger(args.quiet, args.verbose, args.syslog)

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
                logger.error("No config available OR invalid config!")
                exit(2)

        daemon = Daemonize(app=name, pid=lock_file, action=main, logger=logger, foreground=not args.background)
        daemon.start()
