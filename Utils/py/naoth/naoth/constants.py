Joints = {0: "HeadPitch", 1: "HeadYaw", 2: "RShoulderRoll", 3: "LShoulderRoll", 4: "RShoulderPitch",
          5: "LShoulderPitch", 6: "RElbowRoll", 7: "LElbowRoll", 8: "RElbowYaw", 9: "LElbowYaw", 10: "RHipYawPitch",
          11: "LHipYawPitch", 12: "RHipPitch", 13: "LHipPitch", 14: "RHipRoll", 15: "LHipRoll", 16: "RKneePitch",
          17: "LKneePitch", 18: "RAnklePitch", 19: "LAnklePitch", 20: "RAnkleRoll", 21: "LAnkleRoll"}


def get_joint_id(val):
    for key, value in Joints.items():
        if val == value:
            return key
