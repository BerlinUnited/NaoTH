import threading, time, json, traceback

from goprocam import GoProCamera, constants
from .GameControlData import GameControlData

from utils import Event, Logger

# setup logger for network related logs
logger = Logger.getLogger("GoPro")


class CamStatus(threading.Thread):
    """Thread retrieves the current state of the camera and tries to keep the cam alive."""

    def __init__(self, cam: GoProCamera.GoPro, interval: int = 5):
        threading.Thread.__init__(self)
        self.cam = cam
        self.interval = interval
        self.status = {'recording': False, 'mode': None}

        self.running = threading.Event()
        self.sleeper = threading.Event()

        self.timestamp = time.time()
        # HACK take a picture right aways to make sure that wifi is awake long enough
        self.keepAlivePhoto()

    def run(self):
        while not self.running.is_set():
            try:
                js = json.loads(self.cam.getStatusRaw())
                self.status['mode'] = self.cam.parse_value("mode",
                                                           js[constants.Status.Status][constants.Status.STATUS.Mode])
                self.status['recording'] = (js[constants.Status.Status][constants.Status.STATUS.IsRecording] == 1)
                #self.cam.power_on(self.cam._mac_address)
                self.sleeper.wait(self.interval)
                #Event.fire(Event.ConnectedGoproEvent())
                #statusMonitor.setConnectedToGoPro(self.interval + 1.0)

                # take a photo every 10 minutes if not recording to keep the camera alive
                if self.status['recording']:
                    self.timestamp = time.time()
                elif time.time() > self.timestamp + 60.0 * 5:
                    self.keepAlivePhoto()

            except:
                self.running.set()
                self.sleeper.set()

    def keepAlivePhoto(self):
        print("\ntake a picture to keep the cam alive")
        self.timestamp = time.time()
        self.cam.take_photo()
        #setCamVideoMode(self.cam)

    def stop(self):
        self.running.set()
        self.sleeper.set()
        self.join()

    def __getitem__(self, item):
        # if self.running.is_set():
        #  raise Exception("thread is stopped")

        return self.status[item]


class GoPro(threading.Thread):
    def __init__(self, quiet:bool, ignore:bool, max_time:int):
        super().__init__()
        Event.registerListener(self)

        self.quiet = quiet
        self.ignore = ignore
        self.max_time = max_time

        self.cam = None
        self.cam_status = None
        self.gc_data = GameControlData()

        self.is_connected = threading.Event()
        self.cancel = threading.Event()

    def connect(self):
        # get GoPro
        logger.info("Connecting to GoPro ...")

        # statusMonitor.setConnectingToGoPro(20)
        Event.fire(Event.GoproConnecting())
        self.cam = GoProCamera.GoPro()

        if self.cam.getStatusRaw():
            Event.fire(Event.GoproConnected())
            #statusMonitor.setConnectingToGoPro(10)

            # set GoPro to video mode
            self.setCamVideoMode()

            # status monitor and cam keep alive thread
            #self.cam_status = CamStatus(self.cam, 1)
            #self.cam_status.start()


    def disconnect(self):
        logger.info("Disconnecting from GoPro ...")
        # reset cam
        self.cam = None
        # stop status listener
        #self.cam_status.stop()
        Event.fire(Event.GoproDisconnected())

    def run(self):
        previous_state = GameControlData.STATE_INITIAL
        previous_output = ""
        output = ""

        while not self.cancel.is_set():
            # wait 'till connected to network
            self.is_connected.wait()

            try:

                if self.cam_status['recording']:
                    #statusMonitor.setRecordingOn(5.0)
                    pass
                else:
                    #statusMonitor.setRecordingOff(5.0)
                    pass

                # check if one team is 'invisible'
                both_teams_valid = all([t.teamNumber > 0 for t in self.gc_data.team])

                # handle output
                if not self.quiet:
                    output = "%s | %s | game state: %s |" % (
                        self.cam_status['mode'], "RECORDING!" if self.cam_status['recording'] else "Not recording", self.gc_data.getGameState())
                    print("\r" + output + " " * (len(previous_output) - len(output)), end="", flush=True)

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

                # update loop vars
                previous_output = output
                previous_state = self.gc_data.gameState
            except:
                traceback.print_exc()
                return

        self.disconnect()

    def __check_state(self):
        both_teams_valid = all([t.teamNumber > 0 for t in self.gc_data.team])

        if self.is_connected.is_set() and both_teams_valid:

    def connectedNetwork(self, evt:Event.NetworkConnected):
        self.connect()
        self.is_connected.set()

    def disconnectedNetwork(self, evt:Event.NetworkDisconnected):
        self.is_connected.clear()
        self.disconnect()

    def receivedGC(self, evt:Event.GameControllerMessage):
        self.gc_data = evt.message

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
