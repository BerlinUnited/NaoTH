from naoth.LogReader import LogReader
from naoth.LogReader import Parser
import matplotlib.pyplot as plt
import numpy as np
from os import listdir
from os.path import isfile, join
from scipy.spatial import ConvexHull

"""
this script takes a single or more log-files (Depending on how much are in the folder)
and calculates the reference hull and exports it as txt,
so that it can be stored in the "Config" folder
"""


def unpack_frame(frame):
    sjd = frame["SensorJointData"].jointData.position
    mjd = frame["MotorJointData"].position
    '''
          00: HeadPitch             01: HeadYaw             02: RShoulderRoll
          03: LShoulderRoll         04: RShoulderPitch      05: LShoulderPitch
          06: RElbowRoll            07: LElbowRoll          08: RElbowYaw
          09: LElbowYaw             10: RHipYawPitch        11: LHipYawPitch
          12: RHipPitch             13: LHipPitch           14: RHipRoll
          15: LHipRoll              16: RKneePitch          17: LKneePitch
          18: RAnklePitch           19: LAnklePitch         20: RAnkleRoll
          21: LAnkleRoll            22: LWristYaw           23: RWristYaw
          24: LHand                 25: RHand               26: numOfJoint
        '''
    return [sjd[4], sjd[5], mjd[4], mjd[5]]


def vertice_points(data, hull):
    return np.array([data[hull.vertices, 0], data[hull.vertices, 1]]).T


if __name__ == "__main__":
    # path to logfile folder
    path = "C:/Documents_repo/NaoTH/ArmCollisionDetector2018/data/nocollision/new/"

    # main part of the script
    # setting up some variables and our dataset
    dataset = [f for f in listdir(path) if isfile(join(path, f))]
    Lsjd, Lmjd, Rsjd, Rmjd = [], [], [], []
    Laccumulator = np.array(np.empty([1, 2]))
    Raccumulator = np.array(np.empty([1, 2]))

    # going through each log
    for i in range(0, len(dataset)):
        # Opening File
        myParser = Parser()
        myParser.register("MotorJointData", "JointData")
        file = '{}{}'.format(path, dataset[i])
        print('Opening file "{}"'.format(file))
        log = LogReader(file, myParser)
        data = np.array([unpack_frame(frame) for frame in log])
        print('Data successfully acquired')
        # Saving File data into numpy arrays and Synchronizing offset
        offset = 4
        Lsjd = data[offset:data.shape[0], 1]
        Lmjd = data[0:data.shape[0] - offset, 3]
        Rsjd = data[offset:data.shape[0], 0]
        Rmjd = data[0:data.shape[0] - offset, 2]
        # Taking the Error between Sensor measurement and planned angle
        absErrorL = Lsjd - Lmjd
        absErrorR = Rsjd - Rmjd
        pointsL = np.zeros([len(absErrorL), 2])
        pointsR = np.zeros([len(absErrorR), 2])
        pointsL[:, 0] = absErrorL
        pointsL[:, 1] = Lmjd
        pointsR[:, 0] = absErrorR
        pointsR[:, 1] = Rmjd
        print(pointsL.shape)
        # The acquired jointdata in the LOG file is defined by the Aldebaran documentation
        # We shift our angles in this code, so that the neutral arm position (arms down) corresponds
        # to a 0 degrees angle. Since the Zero position is "arm straight in front", and our zero
        # position is arm straight down, we take off 90 degrees of the measured value.
        # So now the new range is: -209.5 (arm in uppermost position) to 29.5 (lowermost position)
        pointsL[:, 0] -= np.radians(90)
        pointsR[:, 0] -= np.radians(90)
        # we calculate the convex hull over all motorjoint data entries and the calculated
        # error in that position
        hullL = ConvexHull(pointsL)
        hullR = ConvexHull(pointsR)
        # for every logfile we opened, we accumulate the vertice points of the hulls
        Laccumulator = np.concatenate((Laccumulator, vertice_points(pointsL, hullL)))
        Raccumulator = np.concatenate((Raccumulator, vertice_points(pointsR, hullR)))

    # now we calculate the convex hull over all vertice points we accumulated
    # something like a "master hull", a hull of hulls
    print(Raccumulator.shape)
    Lreference = ConvexHull(Laccumulator)
    Rreference = ConvexHull(Raccumulator)
    # now again we store only the vertice points in a separate array for storing
    Lwrite = np.array(vertice_points(Laccumulator, Lreference))
    Rwrite = np.array(vertice_points(Raccumulator, Rreference))

    # We now add a error margin. like pumping up the convex hull like a balloon
    for i in range(0, max(Lwrite.shape)):
        Lwrite[i] = np.dot(Lwrite[i], 1.5)
    for i in range(0, max(Rwrite.shape)):
        Rwrite[i] = np.dot(Rwrite[i], 1.5)
    # now we reset our zero shift to restore the original value range
    Lwrite[:, 0] += np.radians(90)
    Rwrite[:, 0] += np.radians(90)
    hull_path_L = './Lreference.txt'
    hull_path_R = './Rreference.txt'
    np.savetxt(hull_path_L, Lwrite)
    np.savetxt(hull_path_R, Rwrite)

    f, axarr = plt.subplots(1, 2, sharex='all', sharey='row')
    axarr[0].plot(Laccumulator[:, 0], Laccumulator[:, 1], 'o')
    axarr[0].plot(Lwrite[:, 0], Lwrite[:, 1], 'r*')
    for simplex in Lreference.simplices:
        axarr[0].plot(Laccumulator[simplex, 0], Laccumulator[simplex, 1], 'k--')

    for simplex in hullL.simplices:
        axarr[1].plot(pointsL[simplex, 0], pointsL[simplex, 1], 'b*')
    plt.show()
