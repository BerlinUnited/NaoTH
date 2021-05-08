import numpy as _np


class Camera:
    """
    Represents the camera info of a nao with additional methods for the geometry.
    """

    def __init__(self, width=640, height=480, opening_angle_diagonal=_np.radians(72.6)):
        self._width = width
        self._height = height
        self._opening_angle_diagonal = opening_angle_diagonal
        self._optical_center = (0, 0)
        self._focal_length = 0

        self._update_vars()

    def _update_vars(self):
        """Calculates the focal length and the optical center of this camera.
        It is sufficient to calculate those values once or only if one of the other values changes.
        """
        self._focal_length = (0.5 * _np.sqrt(self._width ** 2 + self._height ** 2)) \
                             / _np.tan(0.5 * self._opening_angle_diagonal)

        self._optical_center = self._width / 2, self._height / 2

    @property
    def width(self):
        """Returns the width of this camera image."""
        return self._width

    @width.setter
    def width(self, new_width):
        """Sets the new width of the camera image."""
        self._width = new_width
        self._update_vars()

    @property
    def height(self):
        """Returns the height of this camera image."""
        return self._height

    @height.setter
    def height(self, new_height):
        """Sets the new height of the camera image."""
        self._height = new_height
        self._update_vars()

    @property
    def opening_angle_diagonal(self):
        """Returns the opening angle diagonal of this camera."""
        return self._opening_angle_diagonal

    @opening_angle_diagonal.setter
    def opening_angle_diagonal(self, new_opening_angle_diagonal):
        """Sets the new opening angle diagonal of this camera"""
        self._opening_angle_diagonal = new_opening_angle_diagonal
        self._update_vars()

    def focal_length(self):
        """Returns the focal length of the camera."""
        return self._focal_length

    def optical_center(self):
        """Returns the optical center of the camera image as tuple (half width, half height)."""
        return self._optical_center

    def calculate_artificial_horizon(self, c_rot):
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
        x2 = self._width - 1

        v1 = self._focal_length
        v2 = self._width / 2
        v3 = self._height / 2

        y1 = v3 + (r31 * v1 + r32 * v2) / r33
        y2 = v3 + (r31 * v1 - r32 * v2) / r33

        # Mirror ends of horizon if Camera rotated to the left
        if (c_rot @ _np.array([0, 0, 1]))[2] < 0:
            x1, x2 = x2, x1
            y1, y2 = y2, y1

        return _np.array((x1, y1)), _np.array((x2, y2))

    def project(self, x, y, c_rot, c_trans):
        """
        Project an image point onto the ground plane using the camera matrix
        :param x in image
        :param y in image
        :param c_rot: 3x3 camera rotation numpy array
        :param c_trans: 3d camera translation numpy array
        :return: point on the ground plane as 2d numpy array
        """

        half_x, half_y = self._optical_center

        pixel_v = _np.array((
            self._focal_length,
            half_x - x,
            half_y - y
        ))

        v_rot = c_rot @ pixel_v

        field_v = v_rot[:2] * (c_trans[2] / (-v_rot[2]))
        field_v += c_trans[:2]

        return field_v

    def relative_point_to_image(self, point, c_rot, c_trans):
        if len(point) == 2:
            point = _np.array((*point, 0))
        elif len(point) != 3:
            raise ValueError('Given point must have a dimension of 2 of 3')

        epsilon = 1e-13

        # vector: O --> point (in camera coordinates)
        vector_to_point = _np.linalg.inv(c_rot) @ (point - c_trans)

        # the point is behind the camera plane
        if vector_to_point[0] <= epsilon:
            return None

        factor = self.focal_length() / vector_to_point[0]
        center_x, center_y = self._optical_center

        point_in_image = _np.array((
                - (vector_to_point[1] * factor) + 0.5 + center_x
                - (vector_to_point[2] * factor) + 0.5 + center_y
        ))

        return point_in_image

    def __str__(self):
        return 'CAMERA INFO\n' \
               'width={}\n' \
               'height={}\n' \
               'opening_angle_diagonal={}'.format(self.width, self.height, self.opening_angle_diagonal)

class LogCamera(Camera):
    """
    CameraInfo parsed from a log file frame
    """

    def __init__(self, camera_info_data):
        Camera.__init__(self, camera_info_data.resolutionWidth,
                            camera_info_data.resolutionHeight,
                            camera_info_data.openingAngleDiagonal)

        self.data = camera_info_data

        # self.pixel_size = camera_info_data.pixelSize
        # self.focus = camera_info_data.focus

    def __getattr__(self, attr):
        return getattr(self.data, attr)

    def __str__(self):
        return 'CAMERA INFO\n' \
               'width={}\n' \
               'height={}\n' \
               'opening_angle_diagonal={}\n' \
               'pixelSize={}\n' \
               'focus={}'.format(self.width, self.height, self.opening_angle_diagonal, self.pixelSize, self.focus)
