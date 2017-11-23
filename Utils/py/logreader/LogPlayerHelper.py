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

        self.guiProcess = GuiCreator()
        self.guiProcess.start()


        #self.loaderThread = Thread(target=self.loadLog)
        #self.loaderThread.start()

    def loadLog(self):
        print('readLog')
        firstFrame = None
        for frame in self.logRead():
            frameInfo = frame["FrameInfo"]
            """
            if not firstFrame:
                firstFrame = frameInfo.frameNumber
                self.form.setSliderInterval( (firstFrame, firstFrame) )
            else:
                self.form.setSliderInterval( (firstFrame, frameInfo.frameNumber) )
            """
            if self.isClosed:
                break

    def close(self):
        self.guiProcess.join()
        #self.loaderThread.join()
        print('joined!')

        self.logClose()


    def read(self):
        return self.logRead

class GuiCreator(Process):

    def __init__(self):
        super(GuiCreator, self).__init__()

        self.app = QtGui.QApplication(sys.argv)
        self.form = PlayerWindow()

        self.logLoadUpdater = LogLoadUpdater(self.form)
        self.logLoadUpdater.start()

    def run(self):
        self.form.show()
        self.app.exec_()

class CallbackManager(Thread):
    def __init__(self):
        Thread.__init__(self)
        pass

    def run(self):
        pass

class LogLoadUpdater(Thread):
    def __init__(self, form):
        Thread.__init__(self)
        self.form = form

    def run(self):
        i = 1
        while 1:
            a = 0
            b = i
            self.form.setSliderInterval(a, b)
            print("running")
            time.sleep(1)
            i+=1

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

        log.read()
        #for frame in log.read():
        #    print(frame["FrameInfo"])
