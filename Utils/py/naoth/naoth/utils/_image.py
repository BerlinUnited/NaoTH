import numpy as _np


class Image:

    def __init__(self, proto, width=640, height=480, depth=3):
        self._width = width
        self._height = height
        self._depth = depth
        
        self._yuv422 = _np.fromstring(proto, dtype=_np.uint8)
        self._yuv888 = None
        self._rgb = None

    @property
    def yuv422(self):
        """Get the image as YUV422 numpy array"""
        return self._yuv422

    @property
    def yuv888(self):
        """Get the image as YUV888 numpy array"""
        if not self._yuv888:
            self._yuv422_to_yuv888()
        return self._yuv888

    def _yuv422_to_yuv888(self):
        # read each channel of yuv422 separately
        y = self._yuv422[0::2]
        u = self._yuv422[1::4]
        v = self._yuv422[3::4]

        # convert from yuv422 to yuv888
        self._yuv888 = _np.zeros(self._width * self._height * self._depth, dtype=_np.uint8)
        self._yuv888[0::3] = y
        self._yuv888[1::6] = u
        self._yuv888[2::6] = v
        self._yuv888[4::6] = u
        self._yuv888[5::6] = v

        self._yuv888.reshape((self._height, self._width, self._depth))

    @property
    def rgb(self):
        """Get the image as rgb numpy array."""
        if not self._rgb:
            self._yuv888_to_rgb()

        return self._rgb

    def _yuv888_to_rgb(self):
        """Converts yuv888 image to rgb"""
        # BUG: values do not range between 0 and 255
        m = _np.array([[1.0, 1.0, 1.0],
                      [-0.000007154783816076815, -0.3441331386566162, 1.7720025777816772],
                      [1.4019975662231445, -0.7141380310058594, 0.00001542569043522235]])

        self._rgb = _np.dot(self.yuv888, m)
        self._rgb[:, :, 0] -= 179.45477266423404
        self._rgb[:, :, 1] += 135.45870971679688
        self._rgb[:, :, 2] -= 226.8183044444304

    @property
    def brightness(self):
        """Get only the brightness channel of the image as numpy array."""
        y = self._yuv422[0::2]
        return y.reshape((self._height, self._width))
