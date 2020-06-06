import numpy as _np


class Image:

    def __init__(self, proto, width=640, height=480, depth=3):
        #
        self._width = width
        self._height = height
        self._depth = depth
        
        self._yuv422 = _np.fromstring(proto, dtype=_np.uint8)
        self._yuv444 = None
        self._rgb = None

    @property
    def yuv422(self):
        """Get the image as YUV422 numpy array"""
        return self._yuv422

    @property
    def yuv444(self):
        """Get the image as YUV444 numpy array"""
        if self._yuv444 is None:
            self._yuv422_to_yuv444()
        return self._yuv444

    def _yuv422_to_yuv444(self):
        # read each channel of yuv422 separately
        y = self._yuv422[0::2]
        u = self._yuv422[1::4]
        v = self._yuv422[3::4]

        # convert from yuv422 to yuv444
        self._yuv444 = _np.zeros(self._width * self._height * self._depth, dtype=_np.uint8)
        self._yuv444[0::3] = y
        self._yuv444[1::6] = u
        self._yuv444[2::6] = v
        self._yuv444[4::6] = u
        self._yuv444[5::6] = v

    @property
    def rgb(self):
        """Get the image as rgb numpy array."""
        if self._rgb is None:
            self._yuv444_to_rgb()

        return self._rgb

    def _yuv444_to_rgb(self):
        """Converts yuv444 image to rgb888"""
        m = _np.array([
            [ 1.0,                      1.0,                1.0                   ],
            [-0.000007154783816076815, -0.3441331386566162, 1.7720025777816772    ],
            [ 1.4019975662231445,      -0.7141380310058594, 0.00001542569043522235]
        ])

        rgb = _np.dot(self.yuv444.reshape((self._height, self._width, self._depth)), m)
        rgb[:, :, 0] -= 179.45477266423404
        rgb[:, :, 1] += 135.45870971679688
        rgb[:, :, 2] -= 226.8183044444304

        self._rgb = rgb.clip(0, 255).astype(_np.uint8)

    @property
    def brightness(self):
        """Get only the brightness channel (luma, luminance) of the image as numpy array."""
        y = self._yuv422[0::2]
        return y.reshape((self._height, self._width))

    @property
    def luma(self):
        """Get only the brightness channel (luma, luminance) of the image as numpy array."""
        return self.brightness

    @property
    def chroma_b(self):
        """Get only the chrominance blue channel (blue projection/difference) of the image as numpy array."""
        b = self.yuv444[1::3]
        return b.reshape((self._height, self._width))

    @property
    def chroma_r(self):
        """Get only the chrominance red channel (red projection/difference) of the image as numpy array."""
        r = self.yuv444[2::3]
        return r.reshape((self._height, self._width))
