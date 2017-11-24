import sys
from PyQt4 import QtGui, uic

import LogPlayerForm

from multiprocessing import Process, Queue
from threading import Thread

import numpy as np

import time


class LogPlayerHelper:

    def __init__(self, logReader):
        # decorate LogReader read and exit fuctions
        self.logRead = logReader.read
        logReader.read = self.read

        self.logClose = logReader.close
        logReader.close = self.close

        self.isClosed = False

        self.frameQueue = Queue(maxsize=1)

        self.loadLog()

        self.guiProcess = GuiCreator(logReader, self.frameQueue)
        self.guiProcess.start()

        #self.loaderThread = Thread(target=self.loadLog)
        #self.loaderThread.start()

    def loadLog(self):
        for frame in self.logRead():
            pass

    def close(self):
        self.guiProcess.join()
        #self.loaderThread.join()
        print('joined!')

        self.logClose()


    def read(self):
        while 1:
            yield self.frameQueue.get()

class GuiCreator(Process):

    def __init__(self, logReader, frameQueue):
        super(GuiCreator, self).__init__()

        self.app = QtGui.QApplication(sys.argv)
        self.form = PlayerWindow()

        self.form.setSliderInterval(0, len(logReader.frames))

        self.callbackManager = CallbackManager(self.form, logReader, frameQueue)
        self.callbackManager.start()

    def run(self):
        self.form.show()
        self.app.exec_()

class CallbackManager(Thread):
    def __init__(self, form, logReader, frameQueue):
        Thread.__init__(self)

        self.form = form
        self.logReader = logReader
        self.frameQueue = frameQueue

    def run(self):
        print("CallbackManager GO")
        while 1:
            if self.form.isPlaying:
                print('PUSH')
                sliderPos = self.logReader.getSliderPos()
                self.frameQueue.put(self.logReader[sliderPos])
            time.sleep(1)

class PlayerWindow(QtGui.QMainWindow, LogPlayerForm.Ui_PlayerForm):
    def __init__(self, parent=None):
        super(PlayerWindow, self).__init__(parent)
        self.setupUi(self)
        self.playButton.clicked.connect(self.playButton_clicked)

        self.horizontalSlider.sliderReleased.connect(self.sliderChanged)

        self.isPlaying = False

    def setSliderInterval(self, a, b):
        self.horizontalSlider.setMinimum(a)
        self.horizontalSlider.setMaximum(b)

    def getSliderPos(self):
        return self.horizontalSlider.tickPosition()

    def playButton_clicked(self):
        if self.isPlaying:
            print("PAUSE")
            self.isPlaying = False
        else:
            print("PLAY")
            self.isPlaying = True

    def sliderChanged(self):
        print("NEW SLIDER VALUE BY USER")
        print(self.horizontalSlider.value())


if __name__ == '__main__':
    from LogReader import LogReader

    with LogReader("./cognition.log") as log:

        LogPlayerHelper(log)

        for frame in log.read():
            print(frame["FrameInfo"])
