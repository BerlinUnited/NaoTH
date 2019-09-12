import numpy as np


def v3_to_np(v3):
    """
    Creates numpy array from log file Vector3
    :param v3: 3d vector with x, y, z value access
    :returns numpy array of given vector
    """
    return np.array((v3.x, v3.y, v3.z))


def v2_to_np(v2):
    """
    Creates numpy array from log file Vector2
    :param v2: 2d vector with x, y value access
    :returns numpy array of given vector
    """
    return np.array((v2.x, v2.y))


def cm_to_np(cm):
    """
    :param cm: log file camera matrix
    :return: translation and rotation as numpy array
    """
    trans = v3_to_np(cm.pose.translation)

    rot = np.column_stack(tuple(map(v3_to_np, cm.pose.rotation)))
    return trans, rot
