#!/usr/bin/env python3
# -*- coding: utf8 -*-

'''
    Using the 'GoPro API for Python' to communicate with the GoPro:
    https://github.com/KonradIT/gopro-py-api
'''

import sys
import os
import tempfile
import argparse
import threading
import time
import importlib
import importlib.util

from utils import Logger, Daemonize, Network, GoPro, GameController, GameLoggerSql, GameLoggerLog, Event, LedStatusMonitor


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
    config_timestamp = 0 if not args.config else os.stat(importlib.util.find_spec("config").origin).st_mtime

    led = LedStatusMonitor()
    led.start()

    gopro = GoPro(args.background or args.quiet, args.ignore, args.max_time)
    if args.config:
        gopro.setUserSettings({
            'FRAME_RATE': config.fps if 'fps' in vars(config) else None,
            'FOV': config.fov if 'fov' in vars(config) else None,
            'RESOLUTION': config.resolution if 'resolution' in vars(config) else None,
        })
    gopro.start()

    teams = config.teams if args.config and  'teams' in vars(config) else None
    #gameLogger = GameLoggerSql(os.path.join(os.path.dirname(__file__), 'logs/game.db'), teams)
    gameLogger = GameLoggerLog(os.path.join(os.path.dirname(__file__), 'logs/'), teams)
    gameLogger.start()

    gameController = GameController()
    gameController.start()

    network = Network(args.device, args.ssid, args.passwd, args.retries)
    network.start()

    try:
        while True:
            # if config was loaded from file and file was modified since last checked
            if args.config and config_timestamp != os.stat(importlib.util.find_spec("config").origin).st_mtime:
                config_timestamp = os.stat(importlib.util.find_spec("config").origin).st_mtime
                try:
                    # reload config from file
                    importlib.reload(config)
                    Logger.info("Reloaded modified config")
                    network.setConfig(None, config.ssid, config.passwd, config.retries)
                    gopro.setUserSettings({
                        'FRAME_RATE': config.fps if 'fps' in vars(config) else None,
                        'FOV': config.fov if 'fov' in vars(config) else None,
                        'RESOLUTION': config.resolution if 'resolution' in vars(config) else None,
                    })
                except Exception as e:
                    Logger.error("Invalid config! " + str(e))
            else:
                # do nothing
                time.sleep(1)
    except (KeyboardInterrupt, SystemExit) as e:
        print("Shutting down ...")
    # cancel threads
    led.cancel()
    gopro.cancel()
    gameLogger.cancel()
    gameController.cancel()
    network.cancel()
    # wait for finished threads
    led.join()
    gopro.join()
    gameLogger.join()
    gameController.join()
    network.join()

    print("Bye")


class CheckGameController:
    def __init__(self, loopControl:threading.Event):
        Event.registerListener(self)

        gameController = GameController()
        gameController.start()

        try:
            loopControl.wait()
        except (KeyboardInterrupt, SystemExit):
            loopControl.set()

        gameController.cancel()
        gameController.join()


    def receivedGC(self, evt:Event.GameControllerMessage):
        """ Is called, when a new GameController message was received. """
        gc_data = evt.message
        print(gc_data)
        # check if one team is 'invisible'
        if any([t.teamNumber == 0 for t in gc_data.team]):
            print("-- INVISIBLES are playing! --\n")


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
        CheckGameController(loopControl)
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

        daemon = Daemonize(app=name, pid=lock_file, action=main, logger=Logger.logger, foreground=not args.background, chdir='.')
        daemon.start()
