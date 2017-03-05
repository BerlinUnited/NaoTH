#!/usr/bin/python

from naoth.LogReader import LogReader
from naoth.LogReader import Parser

  
def get_camera_matrix(frame):
    cm_bottom = frame["CameraMatrix"]
    cm_top = frame["CameraMatrixTop"]
    return [frame.number, cm_bottom, cm_top]


def get_edgels(frame):
     edgel_percepts = frame["ScanLineEdgelPercept"]
     return [frame.number, edgel_percepts]

if __name__ == "__main__":
    myParser = Parser()
    myParser.register("CameraMatrixTop", "CameraMatrix")

    for msg in LogReader("./game.log", myParser, get_edgels):
        print(msg)
