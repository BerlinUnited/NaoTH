from naoth.LogReader import LogReader
from naoth.LogReader import Parser
import numpy as np
from matplotlib import pyplot as plt

path = "C:/Users/Etienne Couque/Documents/sidearmnc.log"


# path = "C:/Documents_repo/NaoTH/ArmCollisionDetector2018/data/nocollision/new/motion_nc1.log"


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


if __name__ == "__main__":
    myParser = Parser()
    # Is this a bug? When i try  myParser.register("JointData", "MotorJointData") the script fails
    myParser.register("MotorJointData", "JointData")
    print("Opening Logfile...")
    log = LogReader(path, myParser)
    data = [unpack_frame(frame) for frame in log]
    print("Data acquired")
    data = np.array(data)

    # define the tolerance percentage
    tolerance = .25

    # calculate the threshold
    maxR = np.max(np.abs(data[:, 0] - data[:, 2]))
    threshR = maxR + maxR * tolerance
    print("Right threshold: {}".format(threshR))
    maxL = np.max(np.abs(data[:, 1] - data[:, 3]))
    threshL = maxL + maxL * tolerance
    print("Left threshold: {}".format(threshL))
