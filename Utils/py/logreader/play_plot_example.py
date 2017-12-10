
from LogPlayerHelper import LogPlayerHelper
from LogReader import LogReader, LogParser

from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QTimer
import pyqtgraph as pg

from queue import Queue

import time


if __name__ == '__main__':

    logParser = LogParser()
    #logParser.register("ScanLineEdgelPerceptTop", "ScanLineEdgelPercept")
    #logParser.register("CameraMatrixTop", "CameraMatrix")

    with LogReader("./cognition.log", parser=logParser) as log:

        # create qt app
        app = QApplication([])

        # Enable antialiasing for prettier plots
        pg.setConfigOptions(antialias=True)

        # plot window
        plotUI = pg.PlotWidget()
        plotUI.show()

        edgelPlot = plotUI.getPlotItem()
        edgelPlot.setTitle("Edgels")

        # limit the size of the queue to avoid spamming the gui
        worker_results = Queue(maxsize=1)

        def worker(logPlayer):
            for frame in logPlayer.work():
                # doing some serious work
                time.sleep(1/30)
                frameNumber = frame["FrameInfo"].frameNumber

                # pass a plot of your result to the gui
                workerPlot = pg.TextItem("Worker working on Frame " + str(frameNumber))

                logPlayer.sendToUi((frameNumber, workerPlot))

        # Log Player window
        logPlayer = LogPlayerHelper(log, loop=True, realtime=True, worker_func=worker)
        logPlayer.show()

        frames = logPlayer.read()

        workerPlot = None
        def update():
            # global because we do not update it on every update call
            global workerPlot

            frame = next(frames)

            edgelFrame = frame["ScanLineEdgelPercept"]

            edgelsX = [edgel.point.x for edgel in edgelFrame.edgels]
            edgelsY = [edgel.point.y for edgel in edgelFrame.edgels]

            edgelScatter = pg.ScatterPlotItem(x=edgelsX, y=edgelsY,
                pen='w', brush='b', size=10)

            # update worker plots if necessary
            worker_result = logPlayer.recvWorkerResult()
            if not worker_result is None:
                frameNumber, workerPlot = worker_result
                if not frameNumber == frame["FrameInfo"].frameNumber:
                    print(frameNumber, frame["FrameInfo"].frameNumber)

            # plot
            edgelPlot.clear()
            edgelPlot.addItem(edgelScatter)
            if workerPlot is not None:
                edgelPlot.addItem(workerPlot)

            #app.processEvents()

        timer = QTimer()
        timer.timeout.connect(update)
        timer.start(1/60)


        app.exec_()
        logPlayer.close()
