import sys
from PyQt5.QtWidgets import QApplication, QWidget

from LogPlayerForm import Ui_PlayerForm

from threading import Thread, Lock
from queue import Queue

import numpy as np

import time


class LogPlayerHelper:

    def __init__(self, logReader, worker_func):
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

        # create gui
        self.app = QApplication(sys.argv)
        self.form = PlayerWindow(self.seekQueue, self.playLock)

        # TODO: Don't access logReader.frames directly
        self.form.setSliderInterval(0, len(self.logReader.frames))

        self.uiRunning = True

        worker = Thread(target = worker_func)
        worker.start()

        self.form.show()
        # wait for ui to exit
        self.app.exec_()
        self.uiRunning = False
        worker.join()


    def read(self):
        i=0
        while self.uiRunning:
            # block if user pauses
            with self.playLock:
                pass

            if not self.seekQueue.empty():
                i = self.seekQueue.get()
            while i >= len(self.logReader.frames):
                i = self.seekQueue.get()

            frame = self.logReader[i]

            self.form.setSliderPos(i, frame["FrameInfo"].frameNumber)
            yield frame
            i+=1

class PlayerWindow(QWidget):
    def __init__(self, seekQueue, playLock):
        super(PlayerWindow, self).__init__()

        self.ui = Ui_PlayerForm()

        self.ui.setupUi(self)
        self.ui.playButton.clicked.connect(self.playButton_clicked)

        self.ui.seekSlider.sliderReleased.connect(self.seek)

        self.seekQueue = seekQueue
        self.playLock = playLock

        self.isPlaying = True

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
                time.sleep(0.1)

        # spawn log player
        LogPlayerHelper(log, worker)
