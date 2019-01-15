#!/usr/bin/env python3
# -*- coding: utf8 -*-

'''
    Using the 'GoPro API for Python' to communicate with the GoPro:
    https://github.com/KonradIT/gopro-py-api
'''
import re
import sys
import os
import tempfile
import argparse
import threading
import time
import importlib
import importlib.util

from utils import Logger, Daemonize, Network, GoPro, GameController, GameLoggerSql, GameLoggerLog, LedStatusMonitor, \
    CheckGameController, rename, CheckBluetooth, blackboard


def parseArguments():
    use_config = any([ o in sys.argv for o in ['-c', '--config', '-gc', '--check-gc', '-n', '--rename'] ])
    parser = argparse.ArgumentParser(
        description='Starts a GoPro controller (on linux!)',
        epilog= "Example:\n"
                "\t{0} -n 10.0.4.255 -p 10004\n"
                "\t{0} -c -v\n"
                "\t{0} -gc\n"
                "\t{0} -n /path/to/video/files /path/to/log/files\n"
                "".format(os.path.basename(__file__)),
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument('-v', '--verbose', action='store_true', help='activate verbose mode')
    parser.add_argument('-c', '--config', action='store_true', help='use config file')
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
    parser.add_argument('-li', '--log-invisible', action='store_true', help='Whether the games with invisibles should be logged.')
    parser.add_argument('-bt', '--check-bt', action='store', nargs='?', type=check_mac_address, default=False, metavar='mac address', help='Tests the bluetooth functionality. If the mac address is not set, a default one is used.')
    use_rename = any([ o in sys.argv for o in ['-n', '--rename'] ])
    rename = parser.add_argument_group()
    rename.add_argument('-n', '--rename', action='store_true', help='Renames the video files based on the given entries in the log files.')
    rename.add_argument('videos', nargs=1 if use_rename else '?', help='Path to the video files, which should be renamed.')
    rename.add_argument('logs', nargs=1 if use_rename else '?', help='Path to the log files, on which the video files should be renamed too.')

    return parser.parse_args()

def check_mac_address(mac):
    if not re.match("[0-9a-f]{2}([-:]?)[0-9a-f]{2}(\\1[0-9a-f]{2}){4}$", mac.lower()):
        raise argparse.ArgumentTypeError('Not a valid mac address!')

    return mac

def main():
    """
    The main part of this program.
    All necessary threads are started and monitored.
    :return:
    """
    # remember the modification timestamp of the config; if the config gets changed, we can realod it!
    config_timestamp = 0 if not args.config else os.stat(importlib.util.find_spec("config").origin).st_mtime

    led = LedStatusMonitor()
    led.start()

    gopro = GoPro(args.background or args.quiet, args.ignore, args.max_time, args.log_invisible)
    if args.config:
        gopro.setUserSettings({
            'FRAME_RATE': config.fps if 'fps' in vars(config) else None,
            'FOV': config.fov if 'fov' in vars(config) else None,
            'RESOLUTION': config.resolution if 'resolution' in vars(config) else None,
        })
    gopro.start()

    teams = config.teams if args.config and  'teams' in vars(config) else None
    #gameLogger = GameLoggerSql(os.path.join(os.path.dirname(__file__), 'logs/game.db'), teams)
    gameLogger = GameLoggerLog(os.path.join(os.path.dirname(__file__), 'logs/'), teams, args.log_invisible)
    gameLogger.start()

    gameController = GameController()
    gameController.start()

    network = Network(args.device, args.ssid, args.passwd, args.retries, args.mac)
    network.start()

    # monitor threads and config
    threads = [ led, gopro, gameLogger, gameController, network ]
    try:
        while True:
            #print(blackboard)
            # if config was loaded from file and file was modified since last checked
            if args.config and config_timestamp != os.stat(importlib.util.find_spec("config").origin).st_mtime:
                config_timestamp = os.stat(importlib.util.find_spec("config").origin).st_mtime
                try:
                    # reload config from file
                    importlib.reload(config)
                    Logger.info("Reloaded modified config")
                    network.setConfig(None, config.ssid, config.passwd, config.retries, config.mac)
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
                for t in threads:
                    if not t.is_alive():
                        Logger.error("Thread %s is not running (anymore)!", str(t.__class__.__name__))
    except (KeyboardInterrupt, SystemExit):
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


if __name__ == '__main__':
    # this program is developed for the raspberry pi, so only linux is supported
    if not sys.platform.startswith('linux'):
        sys.stderr.write("Only linux based systems are currently supported!")
        exit(1)

    # args = { 'device': 'wifi0', 'ssid': 'NAOCAM', 'password':'a1b0a1b0a1' }
    args = parseArguments()

    log_directory = os.path.join(os.path.dirname(__file__), 'logs/')
    Logger.setupLogger(args.quiet, args.verbose, args.syslog, log_directory)

    # call different functions based on the arguments
    if args.check_gc:
        CheckGameController()
    elif args.check_bt != False:
        CheckBluetooth(args.check_bt if args.check_bt else 'D6:B9:D4:D7:B7:40')
    elif args.rename:
        rename(args.videos[0], args.logs[0])
    else:
        if args.config:
            # use config for network setup
            try:
                import config
                args.ssid = config.ssid
                args.passwd = config.passwd
                args.retries = config.retries
                args.mac = config.mac
            except:
                # no config available!
                Logger.error("No config available OR invalid config!")
                exit(2)

        # name and lock file for this program
        name = 'pyGoPro'
        lock_file = os.path.join(tempfile.gettempdir(), name + '.lock')
        # start the actual 'main' program
        daemon = Daemonize(app=name, pid=lock_file, action=main, logger=Logger.logger, foreground=not args.background, chdir='.', auto_close_fds=False)
        daemon.start()
