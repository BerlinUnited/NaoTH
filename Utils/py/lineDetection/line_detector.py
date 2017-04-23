import numpy as np
import random

from ransac import Ransac

#from cluster import Cluster

# Ransac: iterations, threshDist, minInlier
ransac = Ransac(30, 200, 12)

def detectLines(edgelFrame):
    data = edgelFrame

    bestParameter1, bestParameter2, inlier, outlier = ransac.getOutlier(data)

    param1 = [bestParameter1]
    param2 = [bestParameter2]

    for i in range(2):
        bestParameter1, bestParameter2, inlier, outlier = ransac.getOutlier(outlier)
        param1.append(bestParameter1)
        param2.append(bestParameter2)

    return param1, param2
