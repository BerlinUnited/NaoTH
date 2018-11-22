import json

from . import Logger
from .Blackboard import blackboard
from .Network import Network
from .GameController import GameController
from .GameControlData import GameControlData
from .GoPro import GoPro
from .daemonize import Daemonize
from .GameLoggerSql import GameLoggerSql
from .GameLoggerLog import GameLoggerLog
from .LEDStatusMonitor import LedStatusMonitor
from .CheckGameController import CheckGameController
from .Bluetooth import Bluetooth
from .CheckBluetooth import CheckBluetooth


def rename(videos_dir, logs_dir):
    """ Renames all files from the 'videos_dir' based on the log entry in the 'logs_dir'. """
    import os, re

    if not os.path.isdir(videos_dir):
        Logger.error("Video folder doesn't exists or isn't a directory! ({})".format(videos_dir))
    elif not os.path.isdir(logs_dir):
        Logger.error("Log folder doesn't exists or isn't a directory! ({})".format(logs_dir))
    else:
        Logger.info("Renaming video files")

        vids = {}
        logs = {}

        # read video file names
        for filename in os.listdir(videos_dir):
            name = os.path.splitext(filename)[0]
            if name not in vids:
                vids[name] = []
            vids[name].append(os.path.join(videos_dir, filename))

        # read logs and their content (video files)
        for filename in os.listdir(logs_dir):
            if filename.endswith(".log"):
                try:
                    data = json.load(open(os.path.join(logs_dir, filename)))
                    if data and 'video' in data:
                        for v in data['video']:
                            m = re.match('.+/(.+).MP4', v)
                            if m:
                                name = m.group(1)
                                if name not in logs:
                                    logs[name] = []
                                logs[name].append(os.path.splitext(filename)[0])
                except Exception as e:
                    print('ERROR[',filename, '] Invalid JSON, skipping file;', e)

        cnt = 0
        # rename video file based on the log entry
        for l in logs:
            if len(logs[l]) == 1 and l in vids:
                for v in vids[l]:
                    n = re.sub('(.+/)(.+)(\..{3})', '\g<1>'+logs[l][0]+'_'+l+'\g<3>', v)
                    Logger.debug("Renaming '{}' to '{}'".format(v,n))
                    os.replace(v, n)
                    cnt+=1
            elif len(logs[l]) > 1:
                print("ERROR: multiple games for the same video file!?")
        Logger.info("Renamed {} files".format(cnt))
