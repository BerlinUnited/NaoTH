import numpy as np
import random

from ransac import Ransac

#from cluster import Cluster

# Ransac: iterations, threshDist, minInlier
ransac = Ransac(20, 100, 12)

def detectLines(edgelFrame):
    data = edgelFrame

    bestParameter1, bestParameter2, inlier, outlier = ransac.getOutlier(data)

    param1 = [bestParameter1]
    param2 = [bestParameter2]

    while 1:
        if not bestParameter1:
            break

        bestParameter1, bestParameter2, inlier, outlier = ransac.getOutlier(outlier)

        param1.append(bestParameter1)
        param2.append(bestParameter2)

    return param1, param2
