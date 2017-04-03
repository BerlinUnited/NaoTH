#!/usr/bin/python

from naoth.LogReader import LogReader
from naoth.LogReader import Parser

import matplotlib.pyplot as plt
import matplotlib.animation as animation

import numpy as np
import random

import math

import math3d as m3
import math2d as m2

# TODO: parse as argument
LOGFILE = "/home/robert/logs/090917-1208-Nao0212/game.log"


def parseVector3(msg):
    return m3.Vector3(msg.x,msg.y,msg.z)

def parseCameraMatrix(matrixFrame):
    p = m3.Pose3D()
    p.translation = parseVector3(matrixFrame.pose.translation)
    p.rotation.c1 = parseVector3(matrixFrame.pose.rotation[0])
    p.rotation.c2 = parseVector3(matrixFrame.pose.rotation[1])
    p.rotation.c3 = parseVector3(matrixFrame.pose.rotation[2])
    return p

def getEdgels(frame):
    edgelFrameTop = frame["ScanLineEdgelPerceptTop"]

    edgelFrame = frame["ScanLineEdgelPercept"]

    # project edgels to robot coordinate system
    cameraMatrixTop = parseCameraMatrix(frame["CameraMatrixTop"])
    projectedEdgelsTop = [ projectEdgel(edgel.point.x, edgel.point.y, cameraMatrixTop) for edgel in edgelFrameTop.edgels]

    cameraMatrix = parseCameraMatrix(frame["CameraMatrix"])
    projectedEdgels = [ projectEdgel(edgel.point.x, edgel.point.y, cameraMatrix) for edgel in edgelFrame.edgels]

    return (frame.number, edgelFrameTop, edgelFrame, projectedEdgelsTop, projectedEdgels)

def getFocalLength():
    resolutionWidth = 640
    resolutionHeight = 480
    openingAngleDiagonal = 72.6/180*math.pi

    d2 = resolutionWidth * resolutionWidth + resolutionHeight * resolutionHeight
    halfDiagLength = 0.5 * math.sqrt(d2)
    return halfDiagLength / math.tan(0.5 * openingAngleDiagonal)

def projectEdgel(x,y,cMatrix):
    v = m3.Vector3()
    v.x = getFocalLength()
    v.y = 320 - x
    v.z = 240 - y

    v = cMatrix.rotation*v
    result = m2.Vector2()
    result.x = v.x
    result.y = v.y
    result = result*(cMatrix.translation.z/(-v.z))
    result.x = result.x + cMatrix.translation.x
    result.y = result.y + cMatrix.translation.y
    return (result.x, result.y)

def animate(i, log, edgelsPlotTop, edgelsPlot, projectedEdgelsPlot):
    msg = log.next()

    edgelFrame = [(edgel.point.x, -edgel.point.y) for edgel in msg[1].edgels]
    edgelsPlotTop.set_offsets(edgelFrame)

    edgelFrame = [(edgel.point.x, -edgel.point.y) for edgel in msg[2].edgels]
    edgelsPlot.set_offsets(edgelFrame)

    projectedEdgelsPlot.set_offsets(msg[3] + msg[4])


# init plot
plt.close('all')
fig = plt.figure()

point_size = 5

ax = fig.add_subplot(2,2,1, aspect='equal')
ax.set_xlim([0, 640])
ax.set_ylim([-480, 0])
edgelsPlotTop = plt.scatter([], [], point_size)

ax = fig.add_subplot(2,2,3, aspect='equal')
ax.set_xlim([0, 640])
ax.set_ylim([-480, 0])
edgelsPlot = plt.scatter([], [], point_size)

ax = fig.add_subplot(1,2,2, aspect='equal')
ax.set_xlim([-10000, 10000])
ax.set_ylim([-10000, 10000])
projectedEdgelsPlot = plt.scatter([], [], point_size)


# init parser
logParser = Parser()
logParser.register("ScanLineEdgelPerceptTop", "ScanLineEdgelPercept")
logParser.register("CameraMatrixTop", "CameraMatrix")

log = iter(LogReader(LOGFILE, logParser, getEdgels))


# start animation
ani = animation.FuncAnimation(fig, animate, frames=100, fargs=(log, edgelsPlotTop, edgelsPlot, projectedEdgelsPlot), interval = 60)
plt.show()
