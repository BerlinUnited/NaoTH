import threading, time, json, traceback

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
        self.cam_status = { 'recording': False, 'mode': None, 'lastVideo': None }
        self.gc_data = GameControlData()

        self.is_connected = threading.Event()
        self.cancel = threading.Event()

    def run(self):
        # init game state
        previous_state = GameControlData.STATE_INITIAL
        # run until canceled
        while not self.cancel.is_set():
            # wait 'till (re-)connected to network and gopro
            self.is_connected.wait()
            # valid cam
            if self.cam is not None:
                # update internal cam status
                self.updateStatus()
                # handling recording state
                previous_state = self.handleRecording(previous_state)
                # take "keep alive" photo;
                if self.take_photo_when_idle > 0 and self.take_photo_timestamp + self.take_photo_when_idle < time.time():
                    self.takePhoto()
        # if canceled, at least fire the disconnect event
        self.disconnect()

    def connect(self):
        # get GoPro
        logger.info("Connecting to GoPro ...")

        # statusMonitor.setConnectingToGoPro(20)
        Event.fire(Event.GoproConnecting())
        self.cam = GoProCamera.GoPro()

        if self.cam.getStatusRaw():
            Event.fire(Event.GoproConnected())
            # set GoPro to video mode
            self.setCamVideoMode()
            self.is_connected.set()

    def disconnect(self):
        logger.info("Disconnecting from GoPro ...")
        self.cam = None
        Event.fire(Event.GoproDisconnected())
        self.is_connected.clear()

    def updateStatus(self):
        js = json.loads(self.cam.getStatusRaw())
        self.cam_status['mode'] = self.cam.parse_value("mode", js[constants.Status.Status][constants.Status.STATUS.Mode])
        self.cam_status['recording'] = (js[constants.Status.Status][constants.Status.STATUS.IsRecording] == 1)
        self.cam_status['lastVideo'] = self.cam.getMedia()

    def takePhoto(self):
        """ Takes a photo. """
        if self.cam_status['recording']:
            self.timestamp = time.time()
        else:
            logger.debug("Take a picture")
            self.timestamp = time.time()
            self.cam.take_photo()
            # reset to video mode
            self.setCamVideoMode()

    def handleRecording(self, previous_state):
        # check if one team is 'invisible'
        both_teams_valid = all([t.teamNumber > 0 for t in self.gc_data.team])

        # handle output
        if not self.quiet:
            output = "%s | %s | game state: %s |" % (
                self.cam_status['mode'], "RECORDING!" if self.cam_status['recording'] else "Not recording",
                self.gc_data.getGameState())
            print(output, flush=True)

        # handle game state changes
        if not self.ignore and self.gc_data.secsRemaining < -self.max_time:
            # only stop, if we're still recording
            if self.cam_status['recording']:
                self.stopRecording()
        elif not self.cam_status['recording'] and both_teams_valid and (
                self.gc_data.gameState in [GameControlData.STATE_READY, GameControlData.STATE_SET,
                                           GameControlData.STATE_PLAYING]):
            self.startRecording()
        elif self.cam_status['recording'] and not (
                self.gc_data.gameState in [GameControlData.STATE_READY, GameControlData.STATE_SET,
                                           GameControlData.STATE_PLAYING]):
            self.stopRecording()

        # just for debugging/logging
        if previous_state != self.gc_data.gameState:
            logger.debug("Changed game state to: %s", self.gc_data.getGameState())

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
        logger.debug("Start recording")
        Event.fire(Event.GoproStartRecording())
        self.cam.shutter(constants.start)
        time.sleep(1)  # wait for the command to be executed


    def stopRecording(self):
        logger.debug("Stop recording")
        Event.fire(Event.GoproStopRecording())
        self.cam.shutter(constants.stop)
        time.sleep(1)  # wait for the command to be executed
