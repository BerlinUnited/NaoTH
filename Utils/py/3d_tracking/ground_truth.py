from __future__ import print_function
import argparse

parser = argparse.ArgumentParser(description='save 3d tracking data to file')
parser.add_argument("trackinglog", help='3D tracking logfile')
parser.add_argument("time", help='scan time in sec', type=float)
args = parser.parse_args()

import time
import atexit
from NatNetClient import NatNetClient

# TODO: replace from_bytes with own method for python2 and python3 compatibility

startTime = 0

# server address is specified here
trackingClient = NatNetClient()

tracking_file = open(args.trackinglog, 'w')


# This is a callback function that gets connected to the NatNet client and called once per mocap frame.
def receiveNewFrame(frameNumber, markerSetCount, unlabeledMarkers, rigidBodyCount, skeletonCount,
                    labeledMarkers, latency, timecode, timecodeSub, timestamp, isRecording,
                    trackedModelsChanged):
    header = "FRAME: " + str(time.time() - startTime) + "\n"
    print(header, end='')
    tracking_file.write(header)

    unlabeledOnly = []

    # extract unlabeled markers
    for pos in unlabeledMarkers:
        if pos not in labeledMarkers:
            unlabeledOnly.append(pos)

    # write to file
    header = "MARKER " + str(len(unlabeledOnly)) + "\n"
    print(header, end='')
    tracking_file.write(header)
    for pos in unlabeledOnly:
        tracking_file.write(str(pos[0]) + " " + str(pos[1]) + " " + str(pos[2]) + "\n")


# This is a callback function that gets connected to the NatNet client. It is called once per rigid body per frame
def receiveRigidBodyFrame(id, position, rotation, markers):
    header = "BODY " + str(id) + " " + str(len(markers)) + "\n"
    print(header, end='')
    tracking_file.write(header)
    tracking_file.write(
        str(position[0]) + " " + str(position[1]) + " " + str(position[2]) + " " + str(
            rotation[0]) + " " + str(rotation[1]) + " " + str(rotation[2]) + " " + str(
            rotation[3]) + "\n")
    for pos in markers:
        tracking_file.write(str(pos[0]) + " " + str(pos[1]) + " " + str(pos[2]) + "\n")


def exitfunc():
    trackingClient.stop()
    tracking_file.close()
    print("STOPPED RECORDING")


atexit.register(exitfunc)

# Configure the streaming client to call our rigid body handler on the emulator to send data out.
trackingClient.newFrameListener = receiveNewFrame
trackingClient.rigidBodyListener = receiveRigidBodyFrame

# Start up the streaming client now that the callbacks are set up.
# This will run perpetually, and operate on a separate thread.
trackingClient.run()

time.sleep(args.time)
exitfunc()
