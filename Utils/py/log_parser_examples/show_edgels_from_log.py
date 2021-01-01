#!/usr/bin/python
import math
from argparse import ArgumentParser

import matplotlib.animation as animation
import matplotlib.pyplot as plt
import naoth.math as m
from naoth.log import Parser
from naoth.log import Reader as LogReader
from naoth.datasets import edgels


def parse_vector3(message):
    return m.Vector3(message.x, message.y, message.z)


def parse_camera_matrix(matrix_frame):
    p = m.Pose3D()
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
    opening_diagonal_angle = 72.6 / 180 * math.pi

    d2 = resolution_width ** 2 + resolution_height ** 2
    half_diag_length = 0.5 * math.sqrt(d2)
    return half_diag_length / math.tan(0.5 * opening_diagonal_angle)


def project_edgel(x, y, cam_matrix):
    v = m.Vector3()
    v.x = get_focal_length()
    v.y = 320 - x
    v.z = 240 - y

    v = cam_matrix.rotation * v
    result = m.Vector2()
    result.x = v.x
    result.y = v.y
    result = result * (cam_matrix.translation.z / (-v.z))
    result.x = result.x + cam_matrix.translation.x
    result.y = result.y + cam_matrix.translation.y
    return result.x, result.y


def animate(i, log_iterator, edgels_plot_top, edgels_plot, projected_edgels_plot):
    # TODO use for and yield here
    msg = next(log_iterator)

    edgel_frame = [(edgel.point.x, -edgel.point.y) for edgel in msg[1].edgels]
    if edgel_frame:
        edgels_plot_top.set_offsets(edgel_frame)
    # set plot visible if current edgel_frame contains edgels
    edgels_plot_top.set_visible(edgel_frame)

    edgel_frame = [(edgel.point.x, -edgel.point.y) for edgel in msg[2].edgels]
    if edgel_frame:
        edgels_plot.set_offsets(edgel_frame)
    # set plot visible if current edgel_frame contains edgels
    edgels_plot.set_visible(edgel_frame)

    projected_edgels = msg[3] + msg[4]
    if projected_edgels:
        projected_edgels_plot.set_offsets(projected_edgels)
    # set plot visible if projected_edgels contains edgels
    projected_edgels_plot.set_visible(projected_edgels)


if __name__ == "__main__":
    parser = ArgumentParser(description='script to display or export edgels from log files')
    parser.add_argument("--logfile", help='log file to draw edgels from')
    args = parser.parse_args()

    logFilePath = args.logfile if args.logfile else edgels.load_data()
    # init parser
    logParser = Parser()

    with LogReader(logFilePath, parser=logParser) as reader:
        log = map(get_edgels, reader.read())

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
