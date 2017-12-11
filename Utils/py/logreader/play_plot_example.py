
from LogPlayerHelper import LogPlayerHelper
from LogReader import LogReader, LogParser

from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QTimer

import pyqtgraph as pg

import time


def worker(logPlayer):
    for frame in logPlayer.work():
        # doing some serious work
        time.sleep(1/30)
        frameNumber = frame["FrameInfo"].frameNumber

        # pass a plot of your result to the gui
        workerPlot = pg.TextItem("Worker working on Frame " + str(frameNumber))

        logPlayer.sendToUi(workerPlot)

if __name__ == '__main__':
    # create qt app
    app = QApplication([])

    # Enable antialiasing for prettier plots
    pg.setConfigOptions(antialias=True)

    # init pyqtgraph plot
    plotUI = pg.PlotWidget()
    plotUI.show()
    edgelPlot = plotUI.getPlotItem()
    edgelPlot.setTitle("Edgels")

    with LogReader("./cognition.log") as log:
        # Log Player window
        logPlayer = LogPlayerHelper(log, loop=True, realtime=True, worker_func=worker)
        logPlayer.show()

        # logPlayer generates frames corresponding to the slider pos
        frame_generator = logPlayer.read()

        workerPlot = None
        def update():
            # global because we do not update it on every update call
            global workerPlot

            # get next frame
            frame = next(frame_generator)

            edgelFrame = frame["ScanLineEdgelPercept"]

            edgelsX = [edgel.point.x for edgel in edgelFrame.edgels]
            edgelsY = [edgel.point.y for edgel in edgelFrame.edgels]

            edgelScatter = pg.ScatterPlotItem(x=edgelsX, y=edgelsY,
                pen='w', brush='b', size=10)

            worker_result = logPlayer.recvWorkerResult()
            # update worker plot if necessary
            if not worker_result is None:
                workerPlot = worker_result

            # plot
            edgelPlot.clear()
            edgelPlot.addItem(edgelScatter)
            if workerPlot is not None:
                edgelPlot.addItem(workerPlot)

        timer = QTimer()
        timer.timeout.connect(update)
        # update gui with 60 fps
        timer.start(1/60)

        # execute qt app
        app.exec_()

        # close worker thread
        logPlayer.close()
