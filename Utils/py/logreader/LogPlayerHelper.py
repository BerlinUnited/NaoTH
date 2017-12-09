from PyQt5.QtWidgets import QApplication, QWidget
from PyQt5.QtCore import QTimer

from LogPlayerForm import Ui_PlayerForm

from threading import Thread, ThreadError, Lock
from queue import Queue

import time

class LogPlayerHelper:

    def __init__(self, logReader, loop=False, realtime=False):
        self.logReader = logReader

        # scan frames in log
        for frame in self.logReader.read():
            pass

        # queue to post seek slider events
        self.seekQueue = Queue()
        self.playLock = Lock()

        # create gui
        self.form = PlayerWindow(self.seekQueue, self.playLock)

        # TODO: Don't access logReader.frames directly
        self.form.setSliderInterval(0, len(self.logReader.frames)-1)

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

        timeMillis = lambda: int(round(time.time() * 1000))
        systemTime = timeMillis()

        i=0
        # TODO: change to widget is running
        while self.uiRunning:
            userSeeked = False
            playNextFrame = False

            # check if user changed sliderPos
            while not self.seekQueue.empty():
                i = self.seekQueue.get()
                userSeeked = True

            if not userSeeked and not self.playLock.locked():

                # check if current frame is not the last
                if i+1 >= len(self.logReader.frames):
                    if self.form.ui.loopCheckBox.isChecked():
                        i = 0
                    else:
                        # pause player
                        self.form.playButton_clicked()
                        i = len(self.logReader.frames)-1
                # check if it's time for a new frame
                elif self.form.ui.realTimeCheckBox.isChecked():
                    timePassed = timeMillis() - systemTime
                    nextFrame = self.logReader.frames[i+1]

                    frameTimePassed = nextFrame["FrameInfo"].time - frame["FrameInfo"].time
                    frameTimePassed /= self.form.ui.multiplierBox.value()

                    if timePassed > frameTimePassed:
                        i+=1
                        systemTime = timeMillis()
                else:
                    i+=1
                    systemTime = timeMillis()


            frame = self.logReader.frames[i]

            if not userSeeked:
                self.form.setSliderPos(i)
            self.form.setFrameNumber(frame["FrameInfo"].frameNumber)

            yield frame

class PlayerWindow(QWidget):
    def __init__(self, seekQueue, playLock):
        super(PlayerWindow, self).__init__()

        self.ui = Ui_PlayerForm()

        self.ui.setupUi(self)
        self.ui.playButton.clicked.connect(self.playButton_clicked)

        #self.ui.multiplierBox.setEnabled(False)
        self.ui.realTimeCheckBox.stateChanged.connect(self.setRealTime)

        self.ui.seekSlider.sliderMoved.connect(self.seek)
        self.ui.seekSlider.sliderReleased.connect(self.seek)

        self.seekQueue = seekQueue
        self.playLock = playLock

    def setRealTime(self):
        isChecked = self.ui.realTimeCheckBox.isChecked()
        self.ui.multiplierBox.setEnabled(isChecked)

    def setFrameNumber(self, frameNumber):
        # update lcd display
        self.ui.frameCounter.display(frameNumber)


    def setSliderPos(self, i):
        if not self.ui.seekSlider.isSliderDown():
            self.ui.seekSlider.setValue(i)

    def seek(self):
        self.seekQueue.put(self.ui.seekSlider.value())

    def setSliderInterval(self, a, b):
        self.ui.seekSlider.setMinimum(a)
        self.ui.seekSlider.setMaximum(b)

    def playButton_clicked(self):
        if self.playLock.locked():
            self.playLock.release()
        else:
            self.playLock.acquire()

if __name__ == '__main__':
    from LogReader import LogReader

    app = QApplication([])

    with LogReader("./cognition.log") as logReader:

        logPlayer = LogPlayerHelper(logReader, realtime=True, loop=True)
        logPlayer.show()

        frames = logPlayer.read()
        def update():
            frame = next(frames)
            print(frame["FrameInfo"])
            # doing some serious work
            #time.sleep(0.1)

        timer = QTimer()
        timer.timeout.connect(update)
        # update gui with 60 fps
        timer.start(1/60)

        # start application
        app.exec_()
        logPlayer.close()
