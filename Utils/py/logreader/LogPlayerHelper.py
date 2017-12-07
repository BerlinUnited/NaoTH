from PyQt5.QtWidgets import QApplication, QWidget

from LogPlayerForm import Ui_PlayerForm

from threading import Thread, ThreadError, Lock
from queue import Queue

import time

# call this function to create a QApplication and only start a LogPlayerHelper ui
def spawnLogPlayer(logReader, worker_func, *args, **kwargs):
    app = QApplication([])
    playerUI = LogPlayerHelper(logReader, *args, **kwargs)
    playerUI.show()

    worker = Thread(target = worker_func)
    worker.start()

    # start application
    app.exec_()

    playerUI.close()


class LogPlayerHelper:

    def __init__(self, logReader, block=True, loop=False, realtime=False):
        # decorate LogReader read function
        self.logRead = logReader.read
        logReader.read = self.read

        self.logReader = logReader

        # scan frames in log
        for frame in self.logRead():
            pass

        # queue to post seek slider events
        self.seekQueue = Queue()
        self.playLock = Lock()

        self.block = block

        # create gui
        self.form = PlayerWindow(self.seekQueue, self.playLock)

        # TODO: Don't access logReader.frames directly
        self.form.setSliderInterval(0, len(self.logReader.frames))

        self.form.ui.loopCheckBox.setChecked(loop)
        self.form.ui.realTimeCheckBox.setChecked(realtime)
        self.form.setRealTime()

        self.uiRunning = True

    def show(self):
        self.form.show()

    def close(self):
        self.uiRunning = False
        try:
            self.playLock.release()
        except ThreadError:
            pass

    def read(self):
        frame = self.logReader[0]

        currentFrameTime = frame["FrameInfo"].time

        timeMillis = lambda: int(round(time.time() * 1000))
        systemTime = timeMillis()
        updateTime = time.time()

        i=0
        while self.uiRunning:
            userSeeked = False

            # user pauses
            if self.block:
                with self.playLock:
                    pass
            else:
                if not self.form.isPlaying:
                    yield frame
                    continue

            if not self.seekQueue.empty():
                i = self.seekQueue.get()
                userSeeked = True

            while i >= len(self.logReader.frames):
                if self.form.ui.loopCheckBox.isChecked():
                    i = 0
                elif self.block:
                    # wait for slider change
                    i = self.seekQueue.get()
                    userSeeked = True
                else:
                    i = len(self.logReader.frames)-1
                    break

            if not userSeeked and self.form.ui.realTimeCheckBox.isChecked():
                timePassed = timeMillis() - systemTime
                frameTimePassed = self.logReader[i]["FrameInfo"].time - currentFrameTime
                frameTimePassed /= self.form.ui.multiplierBox.value()
                if frameTimePassed > timePassed:
                    if self.block:
                        time.sleep((frameTimePassed-timePassed)/1000)
                    else:
                        yield frame
                        continue

            frame = self.logReader[i]

            systemTime = timeMillis()
            currentFrameTime = frame["FrameInfo"].time

            # TODO: The Slider freezes randomly if update in Thread
            self.form.setSliderPos(i, frame["FrameInfo"].frameNumber)
            updateTime = time.time()

            yield frame
            i+=1

class PlayerWindow(QWidget):
    def __init__(self, seekQueue, playLock):
        super(PlayerWindow, self).__init__()

        self.ui = Ui_PlayerForm()

        self.ui.setupUi(self)
        self.ui.playButton.clicked.connect(self.playButton_clicked)

        #self.ui.multiplierBox.setEnabled(False)
        self.ui.realTimeCheckBox.stateChanged.connect(self.setRealTime)

        self.ui.seekSlider.sliderReleased.connect(self.seek)

        self.seekQueue = seekQueue
        self.playLock = playLock

        self.isPlaying = True

    def setRealTime(self):
        isChecked = self.ui.realTimeCheckBox.isChecked()
        self.ui.multiplierBox.setEnabled(isChecked)

    def setSliderPos(self, i, frameNumber):

        if not self.ui.seekSlider.isSliderDown():
            self.ui.seekSlider.setValue(i)

        # update lcd display
        self.ui.frameCounter.display(frameNumber)

    def seek(self):
        self.seekQueue.put(self.ui.seekSlider.value())

    def setSliderInterval(self, a, b):
        self.ui.seekSlider.setMinimum(a)
        self.ui.seekSlider.setMaximum(b)

    def playButton_clicked(self):
        if self.isPlaying:
            self.playLock.acquire()
            self.isPlaying = False
        else:
            self.playLock.release()
            self.isPlaying = True

if __name__ == '__main__':
    from LogReader import LogReader

    with LogReader("./cognition.log") as log:

        def worker():
            for frame in log.read():
                print(frame["FrameInfo"])
                # doing some serious work
                #time.sleep(0.1)

        spawnLogPlayer(log, worker, realtime=True)
