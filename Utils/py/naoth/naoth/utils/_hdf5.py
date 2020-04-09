import types

# naoth imports
from ..log import Reader
from ..log import Parser
from ..pb import (
    CommonTypes_pb2,
    Framework_Representations_pb2
)

import numpy as np

JointID = {}
JointID["HeadPitch"] = 0
JointID["HeadYaw"] = 1
JointID["RShoulderRoll"] = 2
JointID["LShoulderRoll"] = 3
JointID["RShoulderPitch"] = 4
JointID["LShoulderPitch"] = 5
JointID["RElbowRoll"] = 6
JointID["LElbowRoll"] = 7
JointID["RElbowYaw"] = 8
JointID["LElbowYaw"] = 9
JointID["RHipYawPitch"] = 10
JointID["LHipYawPitch"] = 11
JointID["RHipPitch"] = 12
JointID["LHipPitch"] = 13
JointID["RHipRoll"] = 14
JointID["LHipRoll"] = 15
JointID["RKneePitch"] = 16
JointID["LKneePitch"] = 17
JointID["RAnklePitch"] = 18
JointID["LAnklePitch"] = 19
JointID["RAnkleRoll"] = 20
JointID["LAnkleRoll"] = 21

JointNames = {}
JointNames[0] = "HeadPitch"
JointNames[1] = "HeadYaw"
JointNames[2] = "RShoulderRoll"
JointNames[3] = "LShoulderRoll"
JointNames[4] = "RShoulderPitch"
JointNames[5] = "LShoulderPitch"
JointNames[6] = "RElbowRoll"
JointNames[7] = "LElbowRoll"
JointNames[8] = "RElbowYaw"
JointNames[9] = "LElbowYaw"
JointNames[10] = "RHipYawPitch"
JointNames[11] = "LHipYawPitch"
JointNames[12] = "RHipPitch"
JointNames[13] = "LHipPitch"
JointNames[14] = "RHipRoll"
JointNames[15] = "LHipRoll"
JointNames[16] = "RKneePitch"
JointNames[17] = "LKneePitch"
JointNames[18] = "RAnklePitch"
JointNames[19] = "LAnklePitch"
JointNames[20] = "RAnkleRoll"
JointNames[21] = "LAnkleRoll"


def transform_log_to_hdf5(log_path, representations):
    # init parser
    log_parser = Parser()
    log_parser.register("MotorJointData", "JointData")

    log = Reader(log_path, log_parser)

    # step 1: split frames into their single representations and put them in a array
    splitted_log = {}
    for rep in representations:
        splitted_log[rep] = list([f[rep] for f in log])

    # step 2: flatten the splitted Log
    # each representation is a dict with the members as key
    # each key refers to list of values
    flattend_log = flatten(splitted_log)

    return flattend_log


def flatten(step):
    return_step = {}
    for rep in list(step.keys()):
        return_step[rep] = flattenListOfObjects(step[rep])

    return return_step


def flattenListOfObjects(obj_list):
    return_dict = {}
    # special handling of joint data
    if isinstance(obj_list[0], (Framework_Representations_pb2.JointData,
                                Framework_Representations_pb2.SensorJointData)):
        return flattenJointData(obj_list)
    if isinstance(obj_list[0], Framework_Representations_pb2.FSRData):
        return flattenFSRData(obj_list)

    # TODO: replace with inspect.getmembers(obj_list, lambda x: not (inspect.ismethod(x) or inspect.isfunction(x) or inspect.isbuiltin(x) or inspect.isclass(x)))
    #       shouldn't use dir function
    member = [f for f in dir(obj_list[0]) if f[0] != '_' and f[0].islower()]
    member = [m for m in member if not m.startswith(("im_", "co_", "func_",))]
    member = [m for m in member if not m.startswith(("denominator", "imag", "numerator", "real"))]
    member = [m for m in member if not isinstance(obj_list[0].__getattribute__(m), (types.BuiltinFunctionType, types.MethodType))]

    for m in member:
        if isinstance(obj_list[0].__getattribute__(m), (bool, int, float, complex)):  # check for 'primitive' types
            return_dict[m] = [f.__getattribute__(m) for f in obj_list]
        elif isinstance(obj_list[0].__getattribute__(m), CommonTypes_pb2.Pose3D):
            return_dict[m] = {'rotation' : [parseRotationMatrix(f.__getattribute__(m).rotation) for f in obj_list],
                              'translation': flattenListOfObjects([f.__getattribute__(m).translation for f in obj_list])}
        elif hasattr(obj_list[0].__getattribute__(m), '__len__'):  # it is some kind of list
            if len(obj_list[0].__getattribute__(m)) == 0:          # it's an empty list?
                continue
            # TODO: correct handling for cases like list of vector3d and alike
            return_dict[m] = [[g for g in f.__getattribute__(m)] for f in obj_list] # assume that this list-like member has a different length in each frame
        else:  # flatten class until primitive...
            return_dict[m] = flattenListOfObjects([f.__getattribute__(m,) for f in obj_list])

    return return_dict


def flattenJointData(obj_list):
    return_dict = {}

    member = [f for f in dir(obj_list[0]) if f[0] != '_' and f[0].islower()]
    member = [m for m in member if not m.startswith(("im_", "co_", "func_",))]
    member = [m for m in member if not m.startswith(("denominator", "imag", "numerator", "real"))]
    member = [m for m in member if not isinstance(obj_list[0].__getattribute__(m), (types.BuiltinFunctionType, types.MethodType))]

    for m in member:
        if isinstance(obj_list[0].__getattribute__(m), Framework_Representations_pb2.JointData):
            return_dict[m] = flattenJointData([f.__getattribute__(m,) for f in obj_list])
        else:
            return_dict[m] = {}
            joint_values = list(zip(*[[g for g in f.__getattribute__(m)] for f in obj_list]))
            for i in JointNames:
                return_dict[m][JointNames[i]] = joint_values[i]

    return return_dict


def flattenFSRData(obj_list):
    return_dict = {}

    member = [f for f in dir(obj_list[0]) if f[0] != '_' and f[0].islower()]
    member = [m for m in member if not m.startswith(("im_", "co_", "func_",))]
    member = [m for m in member if not m.startswith(("denominator", "imag", "numerator", "real"))]
    member = [m for m in member if not isinstance(obj_list[0].__getattribute__(m), (types.BuiltinFunctionType, types.MethodType))]

    for m in member:
        if isinstance(obj_list[0].__getattribute__(m), (bool, int, float, complex)): # check for 'primitive' types
            return_dict[m] = [f.__getattribute__(m) for f in obj_list]
        elif hasattr(obj_list[0].__getattribute__(m), '__len__'):  # it is some kind of list
            if len(obj_list[0].__getattribute__(m)) == 0:          # it's an empty list?
                continue
            return_dict[m] = {}
            # assume that this list-like member as the same length in each frame
            # => assume that each entry has a special meaning so rearrange them
            for idx, values in enumerate(zip(*[[g for g in f.__getattribute__(m)] for f in obj_list])):
                return_dict[m][str(idx)] = list(values)
        else:  # flatten class until primitive...
            return_dict[m] = flattenListOfObjects([f.__getattribute__(m,) for f in obj_list])

    return return_dict


def parseRotationMatrix(rot):
    row1 = [rot[0].x, rot[1].x, rot[2].x]
    row2 = [rot[0].y, rot[1].y, rot[2].y]
    row3 = [rot[0].z, rot[1].z, rot[2].z]
    return np.matrix([row1, row2, row3])
