#!/usr/bin/python
import argparse
import math
import matplotlib.pyplot as plt
import matplotlib.animation as animation

from naoth.log import Reader as LogReader
from naoth.log import Parser
import naoth.math as nao_math

import line_detector

parser = argparse.ArgumentParser(description='script to display edgels from log files')
parser.add_argument("logfile", help='log file to draw edgels from')
parser.add_argument("--direction", help="show direction", action="store_true")
args = parser.parse_args()


class LinePlot:

    def __init__(self, ax, x_range, y_range):
        self.X = x_range
        self.Y = y_range

        self.ax = ax
        self.plotLines = [ax.plot([],[])[0]]
        self.lineCount = 0

    @staticmethod
    def color_from_err(error):
        if error < 0.2:
            return 'g'
        elif error < 0.5:
            return 'b'
        else:
            return 'r'

    def add_line(self, line):
        self.lineCount += 1
        if self.lineCount > len(self.plotLines):
            self.plotLines.append(ax.plot([],[])[0])

        plot = self.plotLines[self.lineCount-1]

        # draw lines
        if len(line.params) == 2:
            m = line.params[0]
            b = line.params[1]

            x_1 = (m * line.y_range[0] + line.x_range[0] - m * b) / (m * m + 1)
            y_1 = (m * m * line.y_range[0] + m * line.x_range[0] + b) / (m * m + 1)
            x_2 = (m * line.y_range[1] + line.x_range[1] - m * b) / (m * m + 1)
            y_2 = (m * m * line.y_range[1] + m * line.x_range[1] + b) / (m * m + 1)

            plot.set_data([x_1, x_2], [y_1, y_2])
        else:
            # vertical line
            plot.set_data([param1, param1], [line.y_range[0], line.y_range[1]])

        plot.set_color( LinePlot.color_from_err(line.error) )

    def clean(self):
        if self.lineCount < len(self.plotLines):
            for i in range(self.lineCount, len(self.plotLines)):
                self.plotLines[i].set_data([], [])
        self.lineCount = 0


def parseVector3(msg):
    return nao_math.Vector3(msg.x,msg.y,msg.z)


def parseCameraMatrix(matrixFrame):
    p = nao_math.Pose3D()
    p.translation = parseVector3(matrixFrame.pose.translation)
    p.rotation.c1 = parseVector3(matrixFrame.pose.rotation[0])
    p.rotation.c2 = parseVector3(matrixFrame.pose.rotation[1])
    p.rotation.c3 = parseVector3(matrixFrame.pose.rotation[2])
    return p


def getEdgels(frame):
    edgelFrameTop = frame["ScanLineEdgelPerceptTop"]

    edgelsTopA = []
    edgelsTopB = []
    for pair in edgelFrameTop.pairs:
        edgelsTopA.append(edgelFrameTop.edgels[pair.begin])
        edgelsTopB.append(edgelFrameTop.edgels[pair.end])

    edgelFrame = frame["ScanLineEdgelPercept"]

    edgelsA = []
    edgelsB = []
    for pair in edgelFrame.pairs:
        edgelsA.append(edgelFrame.edgels[pair.begin])
        edgelsB.append(edgelFrame.edgels[pair.end])

    # project edgels to robot coordinate system
    cameraMatrixTop = parseCameraMatrix(frame["CameraMatrixTop"])

    projectedEdgelsTopA = []
    for edgel in edgelsTopA:
        projectedPos = projectEdgel(edgel.point.x, edgel.point.y, cameraMatrixTop)
        projectedEdgelsTopA.append( (projectedPos[0], projectedPos[1],  edgel.direction.x, edgel.direction.y) )
    projectedEdgelsTopB = []
    for edgel in edgelsTopB:
        projectedPos = projectEdgel(edgel.point.x, edgel.point.y, cameraMatrixTop)
        projectedEdgelsTopB.append( (projectedPos[0], projectedPos[1],  edgel.direction.x, edgel.direction.y) )

    cameraMatrix = parseCameraMatrix(frame["CameraMatrix"])
    projectedEdgelsA = [ (projectEdgel(edgel.point.x, edgel.point.y, cameraMatrix), edgel.direction.x, edgel.direction.y)  for edgel in edgelsA]
    projectedEdgelsB = [ (projectEdgel(edgel.point.x, edgel.point.y, cameraMatrix), edgel.direction.x, edgel.direction.y)  for edgel in edgelsB]

    return (frame.number, (edgelsTopA, edgelsTopB), (edgelsA, edgelsB), (projectedEdgelsTopA, projectedEdgelsTopB), (projectedEdgelsA, projectedEdgelsB))


def getFocalLength():
    resolutionWidth = 640
    resolutionHeight = 480
    openingAngleDiagonal = 72.6/180*math.pi

    d2 = resolutionWidth * resolutionWidth + resolutionHeight * resolutionHeight
    halfDiagLength = 0.5 * math.sqrt(d2)
    return halfDiagLength / math.tan(0.5 * openingAngleDiagonal)


def projectEdgel(x,y,cMatrix):
    v = nao_math.Vector3()
    v.x = getFocalLength()
    v.y = 320 - x
    v.z = 240 - y

    v = cMatrix.rotation*v
    result = nao_math.Vector2()
    result.x = v.x
    result.y = v.y
    result = result*(cMatrix.translation.z/(-v.z))
    result.x = result.x + cMatrix.translation.x
    result.y = result.y + cMatrix.translation.y
    return (result.x, result.y)


def animate(i, log, edgelsPlotTop, linePlot, edgelsPlot, projectedEdgelsPlot):
    msg = log.next()

    edgelFrameTopA = [(edgel.point.x, -edgel.point.y, edgel.direction.x, edgel.direction.y) for edgel in msg[1][0]]
    edgelFrameTopB = [(edgel.point.x, -edgel.point.y, edgel.direction.x, edgel.direction.y) for edgel in msg[1][1]]
    #edgelRotationsTop = [(edgel.direction.x, edgel.direction.y) for edgel in msg[1].edgels]
    edgelsPlotTop[0].set_offsets(edgelFrameTopA)
    edgelsPlotTop[1].set_offsets(edgelFrameTopB)
    if args.direction:
        edgelsPlotTop[0].set_UVC([edgel.direction.x for edgel in msg[1][0]], [edgel.direction.y for edgel in msg[1][0]])
        edgelsPlotTop[1].set_UVC([edgel.direction.x for edgel in msg[1][0]], [edgel.direction.y for edgel in msg[1][1]])

    edgelFrameA = [(edgel.point.x, -edgel.point.y, edgel.direction.x, edgel.direction.y) for edgel in msg[2][0]]
    edgelFrameB = [(edgel.point.x, -edgel.point.y, edgel.direction.x, edgel.direction.y) for edgel in msg[2][1]]
    edgelsPlot[0].set_offsets(edgelFrameA)
    edgelsPlot[1].set_offsets(edgelFrameB)
    if args.direction:
        edgelsPlot[0].set_UVC([edgel.direction.x for edgel in msg[2][1]], [edgel.direction.y for edgel in msg[2][0]])
        edgelsPlot[1].set_UVC([edgel.direction.x for edgel in msg[2][1]], [edgel.direction.y for edgel in msg[2][1]])

    #projectedEdgelsPlot.set_offsets(msg[3] + msg[4])
    projectedEdgelsPlot[0].set_offsets(msg[3][0])
    projectedEdgelsPlot[1].set_offsets(msg[3][1])

    #####################################
    # It's time to get things done

    # A
    data = msg[3][0]
    lines = line_detector.detectLines(data)

    for line in lines:
        linePlot.add_line(line)

    # B
    data = msg[3][1]
    lines = line_detector.detectLines(data)

    for line in lines:
        linePlot.add_line(line)

    linePlot.clean()

# init plot
plt.close('all')
fig = plt.figure()

point_size = 5

ax = fig.add_subplot(2,2,1, aspect='equal')
ax.set_xlim([0, 640])
ax.set_ylim([-480, 0])
if args.direction:
    edgelsPlotTop = [ax.quiver([1,1], [1,1], [1,1], [0,0], pivot='mid', units='dots', scale=0.1),
        ax.quiver([1,1], [1,1], [1,1], [0,0], pivot='mid', color='r', units='dots', scale=0.1)]
else:
    edgelsPlotTop = [plt.scatter([], [], point_size), plt.scatter([], [], point_size)]

ax = fig.add_subplot(2,2,3, aspect='equal')
ax.set_xlim([0, 640])
ax.set_ylim([-480, 0])
if args.direction:
    edgelsPlot = [ax.quiver([1,1], [1,1], [1,1], [0,0], pivot='mid', units='dots', scale=0.1),
        ax.quiver([1,1], [1,1], [1,1], [0,0], pivot='mid', color='r', units='dots', scale=0.1)]
else:
    edgelsPlot = [plt.scatter([], [], point_size), plt.scatter([], [], point_size)]

ax = fig.add_subplot(1,2,2, aspect='equal')
ax.set_xlim([-10000, 10000])
ax.set_ylim([-10000, 10000])
projectedEdgelsPlot = [plt.scatter([], [], point_size, color='black'), plt.scatter([], [], point_size, color='orange')]

linePlot = LinePlot(ax, (-10000, 10000), (-10000, 10000))


# init parser
logParser = Parser()
logParser.register("ScanLineEdgelPerceptTop", "ScanLineEdgelPercept")
logParser.register("CameraMatrixTop", "CameraMatrix")

log = iter(LogReader(args.logfile, logParser, getEdgels))


# start animation
ani = animation.FuncAnimation(fig, animate, frames=100, fargs=(log, edgelsPlotTop, linePlot, edgelsPlot, projectedEdgelsPlot), interval=66)
plt.show()
