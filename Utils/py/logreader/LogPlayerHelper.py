from PyQt5.QtWidgets import QApplication, QWidget
from PyQt5.QtCore import QTimer

from LogPlayerForm import Ui_PlayerForm

from threading import Thread, ThreadError, Lock
from queue import Queue
from peekQueue import PeekQueue

import time

class LogPlayerHelper:

    def __init__(self, logReader, worker_func=None, loop=False, realtime=False):
        self.logReader = logReader

        # scan frames in log
        for frame in self.logReader.read():
            pass

        # queue to post seek slider events
        self.seekQueue = Queue()
        self.playLock = Lock()

        # create gui
        self.form = PlayerWindow(self.seekQueue, self.playLock)

        # TODO: Don't access logReader members directly
        # TODO: cleanup quick fix len(self.logReader.frames)-2
        self.form.setSliderInterval(0, len(self.logReader.frames)-2)

        self.form.ui.loopCheckBox.setChecked(loop)
        self.form.ui.realTimeCheckBox.setChecked(realtime)
        self.form.setRealTime()

        self.uiRunning = True

        self.work_queue = PeekQueue(maxsize=1)
        self.worker = None
        if worker_func is not None:
            self.worker = Thread(target=worker_func, args=(self,))
            self.worker.start()
        self.newWokerResult = False
        self.workerResult = None
        self.workerResultPending = Queue(maxsize=1)

    def show(self):
        self.form.show()

    def close(self):
        self.uiRunning = False
        if not self.worker is None:
            self.work_queue.put(None)
            self.worker.join()

    def work(self):
        newWork = self.work_queue.peek()
        while not newWork is None:
            yield newWork
            self.work_queue.get()
            newWork = self.work_queue.peek()

    # TODO: raise exeption if no worker exists
    def sendToUi(self, data):
        self.workerResultPending.put(data)

    def recvWorkerResult(self):
        if self.newWokerResult:
            self.newWokerResult = False
            return self.workerResult

    def read(self):
        frame = self.logReader[0]

        timeMillis = lambda: int(round(time.time() * 1000))
        systemTime = timeMillis()

        playNextFrame = False

        i=0
        while self.uiRunning:
            userSeeked = False

            # check if user changed sliderPos
            while not self.seekQueue.empty():
                i = self.seekQueue.get()
                userSeeked = True

            if not userSeeked and not self.playLock.locked():

                # check if current frame is not the last
                if i+1 >= len(self.logReader.frames):
                    if self.form.ui.loopCheckBox.isChecked():
                        # TODO: Worker is out of sync for the first frame if looped back
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

                    if playNextFrame or timePassed > frameTimePassed:
                        playNextFrame = True
                        systemTime = timeMillis()
                else:
                    playNextFrame = True
                    systemTime = timeMillis()


            if self.worker is None:
                if playNextFrame:
                    i+=1
                    playNextFrame = False
                frame = self.logReader.frames[i]
            else:
                if self.work_queue.empty():
                    if playNextFrame:
                        i+=1
                        playNextFrame = False

                        self.work_queue.put(self.logReader.frames[i])
                        frame = self.logReader.frames[i-1]
                    else:
                        self.work_queue.put(self.logReader.frames[i])
                        frame = self.logReader.frames[i]

                    if not self.workerResultPending.empty():
                        self.workerResult = self.workerResultPending.get()
                        self.newWokerResult = True

            # update slider pos
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

        timer = QTimer()
        timer.timeout.connect(update)
        # update gui with 60 fps
        timer.start(1/60)

        # start application
        app.exec_()
