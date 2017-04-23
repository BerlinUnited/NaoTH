import numpy as np
import random

from ransac import Ransac

#from cluster import Cluster

# Ransac: iterations, threshDist, minInlier
ransac = Ransac(20, 50, 10)

def detectLines(edgelFrame):
    data = edgelFrame

    bestParameter1, bestParameter2, inlier, outlier, x_r, y_r = ransac.getOutlier(data)

    param1 = [bestParameter1]
    param2 = [bestParameter2]

    x_range = [x_r]
    y_range = [y_r]

    while 1:
        if not bestParameter1:
            break

        bestParameter1, bestParameter2, inlier, outlier, x_r, y_r = ransac.getOutlier(outlier)

        param1.append(bestParameter1)
        param2.append(bestParameter2)

        x_range.append(x_r)
        y_range.append(y_r)

    return param1, param2, x_range, y_range
