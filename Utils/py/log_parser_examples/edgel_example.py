#!/usr/bin/python
import argparse
from pathlib import Path

import math
import matplotlib.animation as animation
import matplotlib.pyplot as plt

import naoth.math2d as m2
import naoth.math3d as m3
from naoth.LogReader import LogReader
from naoth.LogReader import Parser


def parse_vector3(message):
    return m3.Vector3(message.x, message.y, message.z)


def parse_camera_matrix(matrix_frame):
    p = m3.Pose3D()
    p.translation = parse_vector3(matrix_frame.pose.translation)
    p.rotation.c1 = parse_vector3(matrix_frame.pose.rotation[0])
    p.rotation.c2 = parse_vector3(matrix_frame.pose.rotation[1])
    p.rotation.c3 = parse_vector3(matrix_frame.pose.rotation[2])
    return p


def getEdgels(frame):
    edgelFrameTop = frame["ScanLineEdgelPerceptTop"]

    edgelFrame = frame["ScanLineEdgelPercept"]

    # project edgels to robot coordinate system
    cameraMatrixTop = parse_camera_matrix(frame["CameraMatrixTop"])
    projectedEdgelsTop = [ projectEdgel(edgel.point.x, edgel.point.y, cameraMatrixTop) for edgel in edgelFrameTop.edgels]

    cameraMatrix = parse_camera_matrix(frame["CameraMatrix"])
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
    msg = log.__next__()

    edgelFrame = [(edgel.point.x, -edgel.point.y) for edgel in msg[1].edgels]
    edgelsPlotTop.set_offsets(edgelFrame)

    edgelFrame = [(edgel.point.x, -edgel.point.y) for edgel in msg[2].edgels]
    edgelsPlot.set_offsets(edgelFrame)

    projectedEdgelsPlot.set_offsets(msg[3] + msg[4])

    # It's time to get things done


# TODO wird das noch gebraucht?
def get_camera_matrix(frame):
    cm_bottom = frame["CameraMatrix"]
    cm_top = frame["CameraMatrixTop"]
    return [frame.number, cm_bottom, cm_top]


# @deprecated
def get_edgels(frame):
     edgel_percepts = frame["ScanLineEdgelPercept"]
     return [frame.number, edgel_percepts]


if __name__ == "__main__":
    test_path = Path("C://RoboCup//PROJECTS//2019_edgel_matching_data//2019-07-05_11-45-00_Berlin United_vs_NomadZ_half2\game_logs//1_93_Nao0212_190705-1205\game.log")

    # TODO add argument for exporting
    parser = argparse.ArgumentParser(description='script to display or export edgels from log files')
    parser.add_argument("--logfile", help='log file to draw edgels from',
                        default=str(test_path))

    args = parser.parse_args()

    """
    logFilePath = parse_arguments(sys.argv[1:])

    myParser = Parser()
    myParser.register("CameraMatrixTop", "CameraMatrix")

    for msg in LogReader(logFilePath, myParser, get_edgels):
        print(msg)
    """
    logFilePath = args.logfile
    # init parser
    logParser = Parser()
    logParser.register("ScanLineEdgelPerceptTop", "ScanLineEdgelPercept")
    logParser.register("CameraMatrixTop", "CameraMatrix")

    log = iter(LogReader(logFilePath, logParser, getEdgels))

    # init plot
    plt.close('all')
    fig = plt.figure()

    point_size = 5

    ax = fig.add_subplot(2, 2, 1, aspect='equal')
    ax.set_xlim([0, 640])
    ax.set_ylim([-480, 0])
    edgelsPlotTop = plt.scatter([], [], point_size)

    ax = fig.add_subplot(2, 2, 3, aspect='equal')
    ax.set_xlim([0, 640])
    ax.set_ylim([-480, 0])
    edgelsPlot = plt.scatter([], [], point_size)

    ax = fig.add_subplot(1, 2, 2, aspect='equal')
    ax.set_xlim([-10000, 10000])
    ax.set_ylim([-10000, 10000])
    projectedEdgelsPlot = plt.scatter([], [], point_size)

    # start animation
    ani = animation.FuncAnimation(fig, animate, frames=100,
                                  fargs=(log, edgelsPlotTop, edgelsPlot, projectedEdgelsPlot),
                                  interval=60)
    plt.show()
