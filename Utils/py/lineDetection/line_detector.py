import numpy as np
import random

import math

from ransac import Ransac, Result

# Ransac: iterations, threshDist, minInlier, threshAngle
ransac = Ransac(20, 50, 10, math.radians(5))

def detectLines(edgelFrame):
    lines = []

    outlier = edgelFrame
    while 1:
        result = ransac.getOutlier(outlier)
        if not result:
            break
        line = result.getLine()
        lines.append( (line[0], line[1], result.x_range, result.y_range) )

        outlier = result.outlier

    return lines
