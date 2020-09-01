import numpy as np
from matplotlib import pyplot as plt
from naoth.log import Parser
from naoth.log import Reader as LogReader
from naoth.datasets import joints


def unpack_frame(frame):
    sensor_joint_data = frame["SensorJointData"].jointData.position
    motor_joint_data = frame["MotorJointData"].position
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
    return [sensor_joint_data[16], sensor_joint_data[17],
            motor_joint_data[16], motor_joint_data[17]]


if __name__ == "__main__":
    path = joints.load_data()

    myParser = Parser()

    with LogReader(path, myParser) as log:
        print(log[0]["SensorJointData"])

        data = [unpack_frame(frame) for frame in log.read()]

        data = np.array(data)

        offset = 4
        Lsjd = data[offset:data.shape[0], 1]
        Lmjd = data[0:data.shape[0] - offset, 3]
        Rsjd = data[offset:data.shape[0], 0]
        Rmjd = data[0:data.shape[0] - offset, 2]

        fig, axes = plt.subplots(3, 2, sharex='all', constrained_layout=True)
        axes[0, 0].title.set_text('Left MotorJointData')
        axes[0, 1].title.set_text('Right MotorJointData')
        axes[1, 0].title.set_text('Left SensorJointData')
        axes[1, 1].title.set_text('Right SensorJointData')
        axes[2, 0].title.set_text('Left absolute error')
        axes[2, 1].title.set_text('Right absolure error')

        axes[0, 0].plot(Lmjd)
        axes[0, 0].set_xlabel('Frame'), axes[0, 0].set_ylabel('Data')
        axes[0, 1].plot(Rmjd)
        axes[0, 1].set_xlabel('Frame'), axes[0, 1].set_ylabel('Data')
        axes[1, 0].plot(Lsjd)
        axes[1, 0].set_xlabel('Frame'), axes[1, 0].set_ylabel('Data')
        axes[1, 1].plot(Rsjd)
        axes[1, 1].set_xlabel('Frame'), axes[1, 1].set_ylabel('Data')
        axes[2, 0].plot(abs(Lmjd - Lsjd))
        axes[2, 0].set_ylim([0, 0.15])
        axes[2, 0].set_xlabel('Frame'), axes[2, 0].set_ylabel('Data')
        axes[2, 1].plot(abs(Rmjd - Rsjd))
        axes[2, 1].set_ylim([0, 0.15])
        axes[2, 1].set_xlabel('Frame'), axes[2, 1].set_ylabel('Data')

        plt.show()
