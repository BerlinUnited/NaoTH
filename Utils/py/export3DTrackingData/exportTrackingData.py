#!/usr/bin/python

from naoth.LogReader import LogReader
from naoth.LogReader import Parser


def get_gps(frame):
     gps_data = frame["GPSData"]
     return [frame.number, gps_data]

if __name__ == "__main__":
    myParser = Parser()

    for msg in LogReader("./GPS.log", myParser, get_gps):
        print(msg)
