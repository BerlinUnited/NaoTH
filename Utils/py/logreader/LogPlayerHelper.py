import sys
from PyQt4 import QtGui, uic
from PyQt4.phonon import Phonon

import LogPlayerForm

from multiprocessing import Process, Queue, Pipe
from threading import Thread

import numpy as np

import time

from enum import Enum


class LogPlayerHelper:

    def __init__(self, logReader):
        # decorate LogReader read and exit fuctions
        self.logRead = logReader.read
        logReader.read = self.read

        self.logClose = logReader.close
        logReader.close = self.close

        self.logReader = logReader

        self.isClosed = False

        # scan frames in log
        for frame in self.logRead():
            pass

        # create ui connections
        self.log_conn, self.ui_conn = Pipe()

        # create gui
        self.guiProcess = GuiCreator((0, len(logReader.frames)), self.log_conn, self.ui_conn)
        self.guiProcess.start()


    def close(self):
        self.guiProcess.join()
        #self.loaderThread.join()
        print('joined!')

        self.logClose()


    def read(self):
        #TODO: cleanup

        i=0
        while i < len(self.logReader.frames):
            frame = self.logReader[i]

            newSliderPos = -1

            while self.ui_conn.poll():
                event =  self.ui_conn.recv()

                if isinstance(event, bool):
                    if event:
                        continue
                    while self.ui_conn.poll(None):
                        event =  self.ui_conn.recv()

                        if isinstance(event, bool):
                            if event:
                                break
                        elif isinstance(event, int):
                            newSliderPos = event

                elif isinstance(event, int):
                    newSliderPos = event

            if newSliderPos != -1:
                i = newSliderPos



            self.ui_conn.send((i, frame["FrameInfo"].frameNumber))
            yield frame
            i+=1

class GuiCreator(Process):

    def __init__(self, slider_range, log_conn, ui_conn):
        super(GuiCreator, self).__init__()

        self.app = QtGui.QApplication(sys.argv)
        self.form = PlayerWindow(log_conn)

        self.form.setSliderInterval(*slider_range)

        self.callbackManager = CallbackManager(self.form, log_conn, self.form.setSliderPos)

    def run(self):
        self.form.show()
        self.callbackManager.start()

        self.app.exec_()


class CallbackManager(Thread):
    def __init__(self, form, log_conn, setSliderPos):
        Thread.__init__(self)

        self.setSliderPos = setSliderPos

        self.form = form
        self.log_conn = log_conn

    def run(self):
        while 1:
            # wait for slider update
            self.setSliderPos(*self.log_conn.recv())

class PlayerWindow(QtGui.QMainWindow, LogPlayerForm.Ui_PlayerForm):
    def __init__(self, log_conn, parent=None):
        super(PlayerWindow, self).__init__(parent)
        self.setupUi(self)
        self.playButton.clicked.connect(self.playButton_clicked)

        self.seekSlider.sliderReleased.connect(self.seek)

        self.log_conn = log_conn

        self.isPlaying = True

    def setSliderPos(self, i, frameNumber):

        if not self.seekSlider.isSliderDown():
            self.seekSlider.setValue(i)

        # update lcd display
        self.frameCounter.display(frameNumber)

    def seek(self):
        self.log_conn.send(self.seekSlider.value())

    def setSliderInterval(self, a, b):
        self.seekSlider.setMinimum(a)
        self.seekSlider.setMaximum(b)

    def playButton_clicked(self):
        if self.isPlaying:
            print("PAUSE")
            self.log_conn.send(False)
            self.isPlaying = False
        else:
            print("PLAY")
            self.log_conn.send(True)
            self.isPlaying = True

if __name__ == '__main__':
    from LogReader import LogReader

    with LogReader("./cognition.log") as log:

        LogPlayerHelper(log)

        for frame in log.read():
            print(frame["FrameInfo"])
            # doing some serious work
            time.sleep(0.5)
