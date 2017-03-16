#!/usr/bin/python

from naoth.LogReader import LogReader
from naoth.LogReader import Parser

import matplotlib.pyplot as plt
import matplotlib.animation as animation

import numpy as np
import random


def getEdgels(frame):
    return (frame.number, frame["ScanLineEdgelPerceptTop"], frame["ScanLineEdgelPercept"])

def animate(i, edgels, log):
    msg = log.next()

    edgelFrame = [(edgel.point.x, -edgel.point.y) for edgel in msg[1].edgels]
    edgels.set_offsets(edgelFrame)

    edgelFrame = [(edgel.point.x, -edgel.point.y) for edgel in msg[2].edgels]
    edgelsBottom.set_offsets(edgelFrame)


# init plot
fig = plt.figure()

ax = fig.add_subplot(2,1,1)
ax.set_xlim([0, 640])
ax.set_ylim([-480, 0])
edgels = plt.scatter([], [])

ax = fig.add_subplot(2,1,2)
ax.set_xlim([0, 640])
ax.set_ylim([-480, 0])
edgelsBottom = plt.scatter([], [])

# init parser
logParser = Parser()
logParser.register("ScanLineEdgelPerceptTop", "ScanLineEdgelPercept")

log = iter(LogReader("/home/robert/logs/090917-1208-Nao0212/game.log", logParser, getEdgels))


# start animation
ani = animation.FuncAnimation(fig, animate, frames=100, fargs=(edgels, log), interval = 60)
plt.show()
