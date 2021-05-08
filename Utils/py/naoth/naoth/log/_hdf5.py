import types
import numpy as np

from ._experimental_parser import Parser
from ._experimental_reader import Reader
from ..constants import Joints
from ..pb import (
    CommonTypes_pb2,
    Framework_Representations_pb2
)

"""
    TODO describe use case
"""


def transform_log_to_hdf5(log_path, representations):
    # init parser
    log_parser = Parser()
    log = Reader(log_path, log_parser)

    # step 1: split frames into their single representations and put them in a array
    splitted_log = {}
    for rep in representations:
        splitted_log[rep] = list([f[rep] for f in log])

    # step 2: flatten the splitted Log
    # each representation is a dict with the members as key
    # each key refers to list of values
    flat_log = flatten(splitted_log)

    return flat_log


def flatten(step):
    return_step = {}
    for rep in list(step.keys()):
        return_step[rep] = flatten_list_of_objects(step[rep])

    return return_step


def flatten_list_of_objects(obj_list):
    return_dict = {}
    # special handling of joint data
    if isinstance(obj_list[0], (Framework_Representations_pb2.JointData,
                                Framework_Representations_pb2.SensorJointData)):
        return flatten_joint_data(obj_list)
    if isinstance(obj_list[0], Framework_Representations_pb2.FSRData):
        return flatten_fsr_data(obj_list)

    # TODO: replace with inspect.getmembers(obj_list, lambda x: not (inspect.ismethod(x) or inspect.isfunction(x) or inspect.isbuiltin(x) or inspect.isclass(x)))
    #       shouldn't use dir function
    member = [f for f in dir(obj_list[0]) if f[0] != '_' and f[0].islower()]
    member = [m for m in member if not m.startswith(("im_", "co_", "func_",))]
    member = [m for m in member if not m.startswith(("denominator", "imag", "numerator", "real"))]
    member = [m for m in member if
              not isinstance(obj_list[0].__getattribute__(m), (types.BuiltinFunctionType, types.MethodType))]

    for m in member:
        if isinstance(obj_list[0].__getattribute__(m), (bool, int, float, complex)):  # check for 'primitive' types
            return_dict[m] = [f.__getattribute__(m) for f in obj_list]
        elif isinstance(obj_list[0].__getattribute__(m), CommonTypes_pb2.Pose3D):
            return_dict[m] = {'rotation': [parse_rotation_matrix(f.__getattribute__(m).rotation) for f in obj_list],
                              'translation': flatten_list_of_objects(
                                  [f.__getattribute__(m).translation for f in obj_list])}
        elif hasattr(obj_list[0].__getattribute__(m), '__len__'):  # it is some kind of list
            if len(obj_list[0].__getattribute__(m)) == 0:  # it's an empty list?
                continue
            # TODO: correct handling for cases like list of vector3d and alike
            return_dict[m] = [[g for g in f.__getattribute__(m)] for f in
                              obj_list]  # assume that this list-like member has a different length in each frame
        else:  # flatten class until primitive...
            return_dict[m] = flatten_list_of_objects([f.__getattribute__(m, ) for f in obj_list])

    return return_dict


def flatten_joint_data(obj_list):
    return_dict = {}

    member = [f for f in dir(obj_list[0]) if f[0] != '_' and f[0].islower()]
    member = [m for m in member if not m.startswith(("im_", "co_", "func_",))]
    member = [m for m in member if not m.startswith(("denominator", "imag", "numerator", "real"))]
    member = [m for m in member if
              not isinstance(obj_list[0].__getattribute__(m), (types.BuiltinFunctionType, types.MethodType))]

    for m in member:
        if isinstance(obj_list[0].__getattribute__(m), Framework_Representations_pb2.JointData):
            return_dict[m] = flatten_joint_data([f.__getattribute__(m, ) for f in obj_list])
        else:
            return_dict[m] = {}
            joint_values = list(zip(*[[g for g in f.__getattribute__(m)] for f in obj_list]))
            for key, value in Joints.items():
                return_dict[m][value] = joint_values[key]

    return return_dict


def flatten_fsr_data(obj_list):
    return_dict = {}

    member = [f for f in dir(obj_list[0]) if f[0] != '_' and f[0].islower()]
    member = [m for m in member if not m.startswith(("im_", "co_", "func_",))]
    member = [m for m in member if not m.startswith(("denominator", "imag", "numerator", "real"))]
    member = [m for m in member if
              not isinstance(obj_list[0].__getattribute__(m), (types.BuiltinFunctionType, types.MethodType))]

    for m in member:
        if isinstance(obj_list[0].__getattribute__(m), (bool, int, float, complex)):  # check for 'primitive' types
            return_dict[m] = [f.__getattribute__(m) for f in obj_list]
        elif hasattr(obj_list[0].__getattribute__(m), '__len__'):  # it is some kind of list
            if len(obj_list[0].__getattribute__(m)) == 0:  # it's an empty list?
                continue
            return_dict[m] = {}
            # assume that this list-like member as the same length in each frame
            # => assume that each entry has a special meaning so rearrange them
            for idx, values in enumerate(zip(*[[g for g in f.__getattribute__(m)] for f in obj_list])):
                return_dict[m][str(idx)] = list(values)
        else:  # flatten class until primitive...
            return_dict[m] = flatten_list_of_objects([f.__getattribute__(m, ) for f in obj_list])

    return return_dict


def parse_rotation_matrix(rot):
    row1 = [rot[0].x, rot[1].x, rot[2].x]
    row2 = [rot[0].y, rot[1].y, rot[2].y]
    row3 = [rot[0].z, rot[1].z, rot[2].z]
    return np.matrix([row1, row2, row3])
