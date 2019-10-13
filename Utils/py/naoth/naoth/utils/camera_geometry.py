import numpy as np
import numpy.linalg as la


class CameraInfo:
    """
    CameraInfo with default values
    """
    def __init__(self, width=640, height=480, opening_angle_diagonal=np.radians(72.6)):
        self.width = width
        self.height = height
        self.opening_angle_diagonal = opening_angle_diagonal

    def focal_length(self):
        """
        :returns focal length of the camera
        """
        d2 = self.width * self.width + self.height * self.height
        half_diagonal = 0.5 * np.sqrt(d2)

        return half_diagonal / np.tan(0.5 * self.opening_angle_diagonal)

    def optical_center(self):
        return self.width / 2, self.height / 2

    def __str__(self):
        return f'CAMERA INFO\n' \
               f'width={self.width}\n' \
               f'height={self.height}\n' \
               f'opening_angle_diagonal={self.opening_angle_diagonal}'


class LogCameraInfo(CameraInfo):
    """
    CameraInfo parsed from a log file frame
    """
    def __init__(self, camera_info_data):
        CameraInfo.__init__(self, camera_info_data.resolutionWidth,
                            camera_info_data.resolutionHeight,
                            camera_info_data.openingAngleDiagonal)

        self.data = camera_info_data

        #self.pixel_size = camera_info_data.pixelSize
        #self.focus = camera_info_data.focus

    def __getattr__(self, attr):
        return getattr(self.data, attr)

    def __str__(self):
        return f'CAMERA INFO\n' \
               f'width={self.width}\n' \
               f'height={self.height}\n' \
               f'opening_angle_diagonal={self.opening_angle_diagonal}\n' \
               f'pixelSize={self.pixelSize}\n' \
               f'focus={self.focus}'


def calculate_artificial_horizon(c_rot, camera_info: CameraInfo):
    """
    Calculate the vanishing line of the camera in image coordinates
    (below this line pixels can be projected onto the ground)
    :param c_rot: camera rotation matrix
    :return: Start and end point tuple defining the vanishing line in image coordinates
    """
    r31 = c_rot[2, 0]
    r32 = c_rot[2, 1]
    r33 = c_rot[2, 2]

    if r33 == 0:
        r33 = 1e-5

    x1 = 0
    x2 = camera_info.width - 1

    v1 = camera_info.focal_length()
    v2 = camera_info.width / 2
    v3 = camera_info.height / 2

    y1 = v3 + (r31 * v1 + r32 * v2) / r33
    y2 = v3 + (r31 * v1 - r32 * v2) / r33

    # Mirror ends of horizon if Camera rotated to the left
    if (c_rot @ np.array([0, 0, 1]))[2] < 0:
        x1, x2 = x2, x1
        y1, y2 = y2, y1

    return np.array((x1, y1)), np.array((x2, y2))


def project(x, y, c_rot, c_trans, camera_info: CameraInfo):
    """
    Project an image point onto the ground plane using the camera matrix
    :param x in image
    :param y in image
    :param c_rot: 3x3 camera rotation numpy array
    :param c_trans: 3d camera translation numpy array
    :return: point on the ground plane as 2d numpy array
    """

    half_x, half_y = camera_info.optical_center()

    pixel_v = np.array((
        camera_info.focal_length(),
        half_x - x,
        half_y - y
    ))

    v_rot = c_rot @ pixel_v

    field_v = v_rot[:2] * (c_trans[2] / (-v_rot[2]))
    field_v += c_trans[:2]

    return field_v


def relative_point_to_image(point, c_rot, c_trans, camera_info: CameraInfo):
    if len(point) == 2:
        point = np.array((*point, 0))
    elif len(point) != 3:
        raise ValueError('Given point must have a dimension of 2 of 3')

    epsilon = 1e-13

    # vector: O --> point (in camera coordinates)
    vector_to_point = la.inv(c_rot) @ (point - c_trans)

    # the point is behind the camera plane
    if vector_to_point[0] <= epsilon:
        return None

    factor = camera_info.focal_length() / vector_to_point[0]
    center_x, center_y = camera_info.optical_center()

    point_in_image = np.array((
        - (vector_to_point[1] * factor) + 0.5 + center_x
        - (vector_to_point[2] * factor) + 0.5 + center_y
    ))

    return point_in_image
