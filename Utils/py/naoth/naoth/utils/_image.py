import numpy as _np

class Image:

    @staticmethod
    def yuv2rgb(yuv):
        """
        Converts yuv888 image to rgb
        :param yuv: image to convert as numpy array
        :return: rgb numpy array
        """
        # BUG: values do not range between 0 and 255
        m = _np.array([[1.0, 1.0, 1.0],
                      [-0.000007154783816076815, -0.3441331386566162, 1.7720025777816772],
                      [1.4019975662231445, -0.7141380310058594, 0.00001542569043522235]])

        rgb = _np.dot(yuv, m)
        rgb[:, :, 0] -= 179.45477266423404
        rgb[:, :, 1] += 135.45870971679688
        rgb[:, :, 2] -= 226.8183044444304

        return rgb

    @staticmethod
    def ycbcr_from_proto(message):
        # read each channel of yuv422 separately
        yuv422 = _np.fromstring(message.data, dtype=_np.uint8)
        y = yuv422[0::2]
        u = yuv422[1::4]
        v = yuv422[3::4]

        # convert from yuv422 to yuv888
        yuv888 = _np.zeros(message.height * message.width * 3, dtype=_np.uint8)
        yuv888[0::3] = y
        yuv888[1::6] = u
        yuv888[2::6] = v
        yuv888[4::6] = u
        yuv888[5::6] = v

        return yuv888.reshape((message.height, message.width, 3))

    @staticmethod
    def rgb_image_from_proto(message):
        """
        Reads image data and converts it to rgb

        :param message: image data from frame
        :returns rgb numpy array
        """
        yuv888 = Image.ycbcr_from_proto(message)

        return Image.yuv2rgb(yuv888)

    @staticmethod
    def brightness_image_from_proto(message):
        yuv422 = _np.fromstring(message.data, dtype=_np.uint8)

        # get only the brightness channel
        y = yuv422[0::2]

        return y.reshape((message.height, message.width))
