import threading, time, json, inspect

from goprocam import GoProCamera, constants
from .GameControlData import GameControlData

from utils import Event, Logger

# setup logger for network related logs
logger = Logger.getLogger("GoPro")


class GoPro(threading.Thread):
    def __init__(self, quiet:bool, ignore:bool, max_time:int):
        super().__init__()
        Event.registerListener(self)

        self.quiet = quiet
        self.ignore = ignore
        self.max_time = max_time
        self.take_photo_when_idle = 0 # in seconds
        self.take_photo_timestamp = 0

        self.cam = None
        self.cam_status = { 'recording': False, 'mode': None, 'lastVideo': None, 'sd_card': False }
        self.cam_settings = {}
        self.user_settings = {}
        self.gc_data = GameControlData()

        self.is_connected = threading.Event()
        self.__cancel = threading.Event()

    def setUserSettings(self, settings):
        self.user_settings = settings
        if self.cam:
            self.__updateUserSettings()

    def run(self):
        # init game state
        previous_state = { 'game': GameControlData.STATE_INITIAL, 'time': time.time(), 'card': True }
        # run until canceled
        while not self.__cancel.is_set():
            # wait 'till (re-)connected to network and gopro
            self.is_connected.wait()
            # valid cam
            if self.cam is not None:
                # update internal cam status
                self.updateStatus()
                # handling recording state
                previous_state['game'] = self.handleRecording(previous_state)
                # take "keep alive" photo;
                if self.take_photo_when_idle > 0 and self.take_photo_timestamp + self.take_photo_when_idle < time.time():
                    self.takePhoto()
                # notify listeners on the sd card status
                if previous_state['card'] == True and self.cam_status['sd_card'] == False:
                    Event.fire(Event.GoproNoSdcard())
                elif previous_state['card'] == False and self.cam_status['sd_card'] == True:
                    Event.fire(Event.GoproSdcardInserted())
                previous_state['card'] = self.cam_status['sd_card']
        # if canceled, at least fire the disconnect event
        self.disconnect()

    def connect(self):
        # try to connect
        while not self.is_connected.is_set() and not self.__cancel.is_set():
            # get GoPro
            logger.info("Connecting to GoPro ...")

            # statusMonitor.setConnectingToGoPro(20)
            Event.fire(Event.GoproConnecting())
            self.cam = GoProCamera.GoPro()
            if self.updateStatus():
                Event.fire(Event.GoproConnected())
                # set GoPro to video mode
                self.setCamVideoMode()
                self.__updateUserSettings()
                self.is_connected.set()
                # if cam already recording, raise event
                if self.cam_status['recording']:
                    Event.fire(Event.GoproStartRecording())
            else:
                self.disconnect()
                time.sleep(1)

    def disconnect(self):
        logger.info("Disconnecting from GoPro ...")
        self.cam = None
        Event.fire(Event.GoproDisconnected())
        self.is_connected.clear()

    def updateStatus(self):
        if self.cam:
            raw = self.cam.getStatusRaw()
            if raw and self.cam:
                js = json.loads(raw)
                self.cam_status['mode'] = self.cam.parse_value("mode", js[constants.Status.Status][constants.Status.STATUS.Mode])
                self.cam_status['recording'] = (js[constants.Status.Status][constants.Status.STATUS.IsRecording] == 1)
                self.cam_status['sd_card'] = (js[constants.Status.Status][constants.Status.STATUS.SdCardInserted] == 0)
                self.cam_status['lastVideo'] = self.cam.getMedia()
                # update video settings
                for var, val in vars(constants.Video).items():
                    if not var.startswith("_") and not inspect.isclass(val) and val in js[constants.Status.Settings]:
                        self.cam_settings[var] = (val, js[constants.Status.Settings][val])
                return True
        return False

    def takePhoto(self):
        """ Takes a photo. """
        if self.cam_status['recording']:
            self.timestamp = time.time()
        else:
            logger.debug("Take a picture")
            if self.cam is not None:
                self.timestamp = time.time()
                self.cam.take_photo()
                # reset to video mode
                self.setCamVideoMode()
            else:
                logger.error("Not connected to cam!?")

    def handleRecording(self, previous_state):
        # check if one team is 'invisible'
        both_teams_valid = all([t.teamNumber > 0 for t in self.gc_data.team])

        # handle output
        if not self.quiet:
            output = "%s | %s | game state: %s | %s" % (
                self.cam_status['mode'], "RECORDING!" if self.cam_status['recording'] else "Not recording", self.gc_data.getGameState(), self.gc_data.secsRemaining)
            print(output, flush=True)

        # handle game state changes
        if not self.ignore and self.gc_data.secsRemaining < -self.max_time:
            # only stop, if we're still recording
            if self.cam_status['recording']:
                self.stopRecording()
        elif self.gc_data.gameState == GameControlData.STATE_SET and previous_state['time'] + self.max_time < time.time():
            # too long in the set state, stop recording!
            if self.cam_status['recording']:
                logger.debug("Stopped recording because we were too long in set")
                self.stopRecording()
        elif self.cam_status['sd_card'] and not self.cam_status['recording'] and both_teams_valid and (
                self.gc_data.gameState in [GameControlData.STATE_READY, GameControlData.STATE_SET,
                                           GameControlData.STATE_PLAYING]):
            self.startRecording()
        elif self.cam_status['recording'] and not (
                self.gc_data.gameState in [GameControlData.STATE_READY, GameControlData.STATE_SET,
                                           GameControlData.STATE_PLAYING]):
            self.stopRecording()

        # handle game changes
        if previous_state['game'] != self.gc_data.gameState:
            logger.debug("Changed game state to: %s @ %d", self.gc_data.getGameState(), time.time())
            previous_state['time'] = time.time()

        return self.gc_data.gameState

    def connectedNetwork(self, evt:Event.NetworkConnected):
        """ Is called, when the network manager has connected to the GoPro network and connects to the camera itself. """
        self.connect()

    def disconnectedNetwork(self, evt:Event.NetworkDisconnected):
        """ Is called, when the network manager is disconnected from the GoPro network. """
        self.disconnect()

    def receivedGC(self, evt:Event.GameControllerMessage):
        """ Is called, when a new GameController message was received. """
        self.gc_data = evt.message

    def timeoutGC(self, evt:Event.GameControllerTimedout):
        """ Is called, when a new GameController times out. """
        pass

    def setCamVideoMode(self):
        logger.debug("Set GoPro to 'video' mode")
        self.cam.mode(constants.Mode.VideoMode)
        # wait for the command to be executed
        time.sleep(0.5)


    def startRecording(self):
        if self.cam_status['mode'] != "Video":
            self.setCamVideoMode()
        self.__updateUserSettings()
        logger.debug("Start recording")
        Event.fire(Event.GoproStartRecording())
        self.cam.shutter(constants.start)
        time.sleep(1)  # wait for the command to be executed


    def stopRecording(self):
        logger.debug("Stop recording")
        self.cam.shutter(constants.stop)
        self.updateStatus()
        time.sleep(1)  # wait for the command to be executed
        Event.fire(Event.GoproStopRecording(self.cam_status['lastVideo']))

    def cancel(self):
        self.__cancel.set()
        # if cam is still waiting for the network, 'release' the lock
        if not self.is_connected.is_set():
            self.is_connected.set()

    def __updateUserSettings(self):
        logger.debug("Set user video settings")

        for s in self.user_settings:
            # is setting 'valid'
            if s in self.cam_settings:
                # is setting is set and is cam setting different from user settings?
                if self.user_settings[s] is not None and self.user_settings[s] != "" and self.cam_settings[s][1] != int(self.user_settings[s]):
                    # try to set the user setting on the cam
                    res = self.cam.gpControlSet(str(self.cam_settings[s][0]), str(self.user_settings[s]))
                    if len(res) == 0:
                        logger.warning("The following setting can not be set for this cam: '"+s+"'")
                        # remove from user settings
                        self.user_settings[s] = None
            else:
                logger.warning("Unknown video setting: '"+s+"'")

