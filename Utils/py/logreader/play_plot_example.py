
from LogPlayerHelper import LogPlayerHelper
from LogReader import LogReader, LogParser

from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QTimer
import pyqtgraph as pg

import time


if __name__ == '__main__':

    logParser = LogParser()
    logParser.register("ScanLineEdgelPerceptTop", "ScanLineEdgelPercept")
    #logParser.register("CameraMatrixTop", "CameraMatrix")

    with LogReader("./cognition.log", parser=logParser) as log:

        # create qt app
        app = QApplication([])

        # Enable antialiasing for prettier plots
        pg.setConfigOptions(antialias=True)

        # plot window
        plotUI_1 = pg.PlotWidget()
        plotUI_1.show()

        edgelPlot = plotUI_1.getPlotItem()
        edgelPlot.setTitle("Edgels")

        # plot window
        plotUI_2 = pg.PlotWidget()
        plotUI_2.show()

        edgelPlotTop = plotUI_2.getPlotItem()
        edgelPlotTop.setTitle("Edgels Top")

        # Log Player window
        playerUI = LogPlayerHelper(log, block=False, loop=True)
        playerUI.show()

        frames = log.read()
        def update():
            frame = next(frames)

            edgelPlot.clear()
            edgelPlotTop.clear()

            edgelFrame = frame["ScanLineEdgelPercept"]

            edgelsX = [edgel.point.x for edgel in edgelFrame.edgels]
            edgelsY = [edgel.point.y for edgel in edgelFrame.edgels]

            edgelScatter = pg.ScatterPlotItem(x=edgelsX, y=edgelsY,
                pen='w', brush='b', size=10)

            edgelFrame = frame["ScanLineEdgelPerceptTop"]

            edgelsX = [edgel.point.x for edgel in edgelFrame.edgels]
            edgelsY = [edgel.point.y for edgel in edgelFrame.edgels]

            edgelScatterTop = pg.ScatterPlotItem(x=edgelsX, y=edgelsY,
                pen='w', brush='r', size=10)

            edgelPlot.addItem(edgelScatter)
            edgelPlotTop.addItem(edgelScatterTop)
            #app.processEvents()

        timer = QTimer()
        timer.timeout.connect(update)
        timer.start(0)


        app.exec_()
        playerUI.close()
