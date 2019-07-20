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


def focal_length():
    """
    :returns focal length of the camera
    """
    # TODO: get intrinsic parameters from a config file or "CameraInfo" if available
    width = 640
    height = 480
    openining_angle_diagonal = 72.6 / 180 * np.pi

    d2 = width * width + height * height
    half_diagonal = 0.5 * np.sqrt(d2)

    return half_diagonal / np.tan(0.5 * openining_angle_diagonal)


def project(x, y, c_rot, c_trans):
    """
    Project an image point onto the ground plane using the camera matrix
    :param x in image
    :param y in image
    :param c_rot: 3x3 camera rotation numpy array
    :param c_trans: 3d camera translation numpy array
    :return: point on the ground plane as 2d numpy array
    """
    # TODO: get intrinsic parameters from a config file or "CameraInfo" if available
    pixel_v = np.array((
        focal_length(),
        320 - x,
        240 - y
    ))

    v_rot = c_rot @ pixel_v

    field_v = v_rot[:2] * (c_trans[2] / (-v_rot[2]))
    field_v += c_trans[:2]

    return field_v
