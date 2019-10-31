#!/usr/bin/python
import argparse
from pywget import wget
from pathlib import Path

import math
import matplotlib.animation as animation
import matplotlib.pyplot as plt

import naoth.math2d as m2
import naoth.math3d as m3
from naoth.LogReader import LogReader
from naoth.LogReader import Parser


def get_demo_logfiles():
    base_url = "https://www2.informatik.hu-berlin.de/~naoth/ressources/log/demo_edgels/"
    logfile_list = ["2019-07-05_11-45-00_Berlin United_vs_NomadZ_half2-1_93_Nao0212.log"]

    target_dir = Path("logs")
    Path.mkdir(target_dir, exist_ok=True)

    for logfile in logfile_list:
        if not Path(target_dir / logfile).is_file():
            wget.download(base_url+logfile, target_dir)


def parse_vector3(message):
    return m3.Vector3(message.x, message.y, message.z)


def parse_camera_matrix(matrix_frame):
    p = m3.Pose3D()
    p.translation = parse_vector3(matrix_frame.pose.translation)
    p.rotation.c1 = parse_vector3(matrix_frame.pose.rotation[0])
    p.rotation.c2 = parse_vector3(matrix_frame.pose.rotation[1])
    p.rotation.c3 = parse_vector3(matrix_frame.pose.rotation[2])
    return p


def get_edgels(frame):
    edgel_frame_top = frame["ScanLineEdgelPerceptTop"]

    edgel_frame = frame["ScanLineEdgelPercept"]

    # project edgels to robot coordinate system
    camera_matrix_top = parse_camera_matrix(frame["CameraMatrixTop"])
    projected_edgels_top = [project_edgel(edgel.point.x, edgel.point.y, camera_matrix_top)
                            for edgel in edgel_frame_top.edgels]

    camera_matrix = parse_camera_matrix(frame["CameraMatrix"])
    projected_edgels = [project_edgel(edgel.point.x, edgel.point.y, camera_matrix)
                        for edgel in edgel_frame.edgels]

    return frame.number, edgel_frame_top, edgel_frame, projected_edgels_top, projected_edgels


def get_focal_length():
    resolution_width = 640
    resolution_height = 480
    opening_diagonal_angle = 72.6/180*math.pi

    d2 = resolution_width ** 2 + resolution_height ** 2
    half_diag_length = 0.5 * math.sqrt(d2)
    return half_diag_length / math.tan(0.5 * opening_diagonal_angle)


def project_edgel(x, y, cam_matrix):
    v = m3.Vector3()
    v.x = get_focal_length()
    v.y = 320 - x
    v.z = 240 - y

    v = cam_matrix.rotation * v
    result = m2.Vector2()
    result.x = v.x
    result.y = v.y
    result = result*(cam_matrix.translation.z / (-v.z))
    result.x = result.x + cam_matrix.translation.x
    result.y = result.y + cam_matrix.translation.y
    return result.x, result.y


def animate(i, log_iterator, edgels_plot_top, edgels_plot, projected_edgels_plot):
    print(i)
    # TODO use for and yield here
    msg = log_iterator.__next__()

    edgel_frame = [(edgel.point.x, -edgel.point.y) for edgel in msg[1].edgels]
    edgels_plot_top.set_offsets(edgel_frame)

    edgel_frame = [(edgel.point.x, -edgel.point.y) for edgel in msg[2].edgels]
    edgels_plot.set_offsets(edgel_frame)

    projected_edgels_plot.set_offsets(msg[3] + msg[4])


if __name__ == "__main__":
    get_demo_logfiles()

    parser = argparse.ArgumentParser(description='script to display or export edgels from log files')
    parser.add_argument("--logfile", help='log file to draw edgels from',
                        default="logs/2019-07-05_11-45-00_Berlin United_vs_NomadZ_half2-1_93_Nao0212.log")

    args = parser.parse_args()

    logFilePath = args.logfile
    # init parser
    logParser = Parser()
    logParser.register("ScanLineEdgelPerceptTop", "ScanLineEdgelPercept")
    logParser.register("CameraMatrixTop", "CameraMatrix")

    log = iter(LogReader(logFilePath, logParser, get_edgels))

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
