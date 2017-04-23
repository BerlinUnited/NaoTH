import numpy as np
from numpy import linalg as LA
import math
import random

class Ransac:

    def __init__(self, iterations, threshDist, minInlier, threshAngle):
        self.iterations = iterations
        self.threshDist = threshDist
        self.minInlier = minInlier

        self.threshAngle = threshAngle

    @staticmethod
    def __unit_vector(vector):
        return vector / np.linalg.norm(vector)

    @staticmethod
    def __angle_between(v1, v2):
        v1_u = Ransac.__unit_vector(v1)
        v2_u = Ransac.__unit_vector(v2)
        return math.acos(np.clip(np.dot(v1_u, v2_u), -1.0, 1.0))

    def getOutlier(self, data):
        if not len(data) > 2:
            return None, None, None, data, None, None

        bestParameter1 = 0
        bestParameter2 = 0
        x_range = (0,0)
        y_range = (0,0)
        bestIn = []
        bestOut = []

        for i in range(self.iterations):
            # ----------
            # Prediction
            # ----------

            # pick two random points
            pick = random.sample(range(0, len(data)), 2)
            sampleA = data[pick[0]]
            sampleA_direction = (sampleA[2], sampleA[3])
            sampleA = (sampleA[0], sampleA[1])
            sampleB = data[pick[1]]
            sampleB_direction = (sampleB[2], sampleB[3])
            sampleB = (sampleB[0], sampleB[1])

            # ----
            # Test
            # ----

            if Ransac.__angle_between(sampleA_direction, sampleB_direction) > self.threshAngle:
                continue

            lineNormal = (sampleA_direction[0] + sampleB_direction[0], sampleA_direction[1] + sampleB_direction[1])

            # calculate distances of all pointes to the line
            inlier = []
            outlier = []
            lineVec = np.subtract(sampleB, sampleA)

            maxX = data[0][0]
            maxY = data[0][1]
            minX = data[0][0]
            minY = data[0][1]

            for edgel in data:
                direction = (edgel[2], edgel[3])
                if Ransac.__angle_between(lineNormal, direction) > self.threshDist:
                    outlier.append(edgel)
                    continue

                point = (edgel[0], edgel[1])
                d = LA.norm(np.cross(lineVec, np.subtract(sampleA, point))) / LA.norm(lineVec)

                if abs(d) <= self.threshDist:
                    inlier.append(edgel)

                    # find endpoints
                    if edgel[0] > maxX:
                        maxX = edgel[0]
                    elif edgel[0] < minX:
                        minX = edgel[0]
                    if edgel[1] > maxY:
                        maxY = edgel[1]
                    elif edgel[1] < minY:
                        minY = edgel[1]
                else:
                    outlier.append(edgel)

            #print(len(inlier))

            if len(inlier) >= self.minInlier and len(inlier) > len(bestIn):
                #print("Found something", len(inlier))
                x_range = (minX, maxX)
                y_range = (minY, maxY)

                bestIn = inlier
                bestOut = outlier

                # create line out of sample
                x = sampleB[0] - sampleA[0]
                if x:
                    m = (sampleB[1] - sampleA[1]) / float(x)
                    b = sampleB[1] - (m * sampleB[0])

                    bestParameter1 = m
                    bestParameter2 = b
                else:
                    # vertical line
                    bestParameter1 = sampleA[0]
                    bestParameter2 = None

                #data = inlier

        #print("PARAMETER:", bestParameter1, bestParameter2, "\nINLIER:", len(bestIn))

        return bestParameter1, bestParameter2, bestIn, bestOut, x_range, y_range


# test TODO: Doesn't work, because lack of edgel directions
if __name__ == '__main__':
    import matplotlib.pyplot as plt

    def __createLineSample(parameter1, parameter2):
        line = [np.array( (line_samples[i], random.uniform( (parameter1*line_samples[i] + parameter2) - 0.2, (parameter1*line_samples[i] + parameter2) + 0.2)) ) for i in range(len(line_samples))]
        return line

    line_samples = [i for i in np.arange(0,10,0.1)]

    # create data
    noise = [np.array( (random.uniform(0, 10), random.uniform(0, 10)) ) for i in range(250)]
    data = __createLineSample(1,0) + __createLineSample(-1, 10) + __createLineSample(0,2) + noise

    random.shuffle(data)
    #data = sorted(data,  key=lambda elem: elem[0])

    print("LEN:", len(data))

    # draw points
    plt.scatter(*zip(*data))

    lines = []
    outlier = data

    ransac = Ransac(20, 0.4, 1/6)

    for i in range(3):
        # run ransac
        bestParameter1, bestParameter2, inlier, outlier = ransac.getOutlier(outlier)

        plt.scatter(*zip(*outlier))

        lines.append( (bestParameter1, bestParameter2) )
        data = outlier

    # draw lines
    curve_samples = [i for i in np.arange(0,10,0.1)]
    for parameter in lines:
        curve = [parameter[0] * i + parameter[1] for i in curve_samples]

        plt.plot(curve_samples, curve)

    plt.show()

"""
# fit and draw line
p = np.polyfit(*zip(*bestIn) , 1)
curve_samples = np.arange(0, 10, 0.1)
curve = [p[0]*i + p[1] for i in curve_samples]
"""
