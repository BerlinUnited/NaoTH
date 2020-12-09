import math

from ransac import Ransac, Result


class Line:
    def __init__(self, params, x_range, y_range, error):
        self.params = params
        self.x_range = x_range
        self.y_range = y_range

        self.error = error


# Ransac: iterations, threshDist, minInlier, threshAngle
ransac = Ransac(20, 50, 10, math.radians(5))


def detectLines(edgelFrame):
    lines = []

    outlier = edgelFrame
    while 1:
        result = ransac.getOutlier(outlier)
        if not result:
            break

        error = math.pow(math.e, -0.1 * result.error / len(result.inlier) )

        line = Line(result.getLine(), result.x_range, result.y_range, error)
        lines.append(line)

        outlier = result.outlier

    return lines
