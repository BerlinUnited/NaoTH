#Copyright © 2018 Naturalpoint
#
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.


# OptiTrack NatNet direct depacketization sample for Python 3.x
#
# Uses the Python NatNetClient.py library to establish a connection (by creating a NatNetClient),
# and receive data via a NatNet connection and decode it using the NatNetClient library.

from NatNetClient import NatNetClient
import argparse
import time

startTime = 0

parser = argparse.ArgumentParser(description='save 3d tracking data to file')
parser.add_argument("--trackinglog", default="track.log", help='3D tracking logfile')
parser.add_argument("--time", help='optional scan time in sec', type=float)
args = parser.parse_args()
tracking_file = open(args.trackinglog, 'w')

# This is a callback function that gets connected to the NatNet client and called once per mocap frame.
def receiveNewFrame( frameNumber, markerSetCount, unlabeledMarkersCount, rigidBodyCount, skeletonCount,
                    labeledMarkerCount, timecode, timecodeSub, timestamp, isRecording, trackedModelsChanged ):
    print( "Received frame", frameNumber )
    header = "FRAME: " + str(time.time() - startTime) + "\n"
    print(header, end='')
    tracking_file.write(header)

# This is a callback function that gets connected to the NatNet client. It is called once per rigid body per frame
def receiveRigidBodyFrame( id, position, rotation ):
    header = "BODY " + str(id) + "\n"
    print(header, end='')
    tracking_file.write(header)
    tracking_file.write(
        str(position[0]) + " " + str(position[1]) + " " + str(position[2]) + " "
        + str(rotation[0]) + " " + str(rotation[1]) + " " + str(rotation[2]) + " " + str( rotation[3]) + "\n"
    )
    print( "Received frame for rigid body", id )

# This is a callback function that gets connected to the NatNet client. It is called once per rigid body per frame
def receiveUnlabeledMarkers( markers ):
    header = "MARKER " + str(len(markers)) + "\n"
    print(header, end='')
    tracking_file.write(header)
    for m in markers:
        tracking_file.write(str(m[0]) + " " + str(m[1]) + " " + str(m[2]) + "\n")

# This will create a new NatNet client
streamingClient = NatNetClient()

# Configure the streaming client to call our rigid body handler on the emulator to send data out.
streamingClient.newFrameListener = receiveNewFrame
streamingClient.rigidBodyListener = receiveRigidBodyFrame
streamingClient.unlabeledMarkersListener = receiveUnlabeledMarkers

# Start up the streaming client now that the callbacks are set up.
# This will run perpetually, and operate on a separate thread.
streamingClient.run()

if args.time is None:
    input("Press Enter to stop...")
else:
    time.sleep(args.time)

streamingClient.stop()

