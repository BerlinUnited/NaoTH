import numpy as np
from numpy import linalg as LA
import math
import random

class Ransac:

    def __init__(self, iterations, threshDist, inlierRatio):
        self.iterations = iterations
        self.threshDist = threshDist
        self.inlierRatio = inlierRatio

        self.cluster = []

    def unit_vector(self, vector):
        return vector / np.linalg.norm(vector)

    def angle_between(self, v1, v2):
        v1_u = self.unit_vector(v1)
        v2_u = self.unit_vector(v2)
        return np.clip(np.dot(v1_u, v2_u), -1.0, 1.0)

    def buildCluster(self, data):
        # canopy clustering
        self.cluster = []

        # loose angle
        t1 = math.cos(math.radians(10))
        # tight angle
        t2 = math.cos(math.radians(5))

        startPoint = data.pop()
        edgelList = data
        currCluster = [startPoint]
        self.cluster.append(currCluster)

        while 1:
            newEdgelList = []
            startOrientation = (startPoint[2], startPoint[3])

            for edgel in edgelList:
                orientation = (edgel[2], edgel[3])

                cos_angle = self.angle_between(startOrientation, orientation)

                if cos_angle > t2:
                    currCluster.append(edgel)
                elif cos_angle > t1:
                    currCluster.append(edgel)
                    newEdgelList.append(edgel)
                else:
                    newEdgelList.append(edgel)

            if not newEdgelList:
                break
            startPoint = newEdgelList.pop()
            currCluster = [startPoint]
            self.cluster.append(currCluster)
            edgelList = newEdgelList

    def getOutlier(self, data):
        if not data:
            return None, None, None

        self.buildCluster(data)

        data = max(self.cluster, key=len)

        #print(len(data), len(max(self.cluster, key=len)))


        print(len(data))

        bestParameter1 = 0
        bestParameter2 = 0
        bestIn = []
        bestOut = []

        for i in range(self.iterations):
            # ----------
            # Prediction
            # ----------

            # pick two random points
            pick = random.sample(range(0, len(data)), 2)
            sampleA = data[pick[0]]
            sampleA = (sampleA[0], sampleA[1])
            sampleB = data[pick[1]]
            sampleB = (sampleB[0], sampleB[1])

            # ----
            # Test
            # ----

            # calculate distances of all pointes to the line
            inlier = []
            outlier = []
            lineVec = np.subtract(sampleB, sampleA)
            for edgel in data:
                point = (edgel[0], edgel[1])
                d = LA.norm(np.cross(lineVec, np.subtract(sampleA, point))) / LA.norm(lineVec)

                if abs(d) <= self.threshDist:
                    inlier.append(point)
                else:
                    outlier.append(point)

            #print(len(inlier))

            if len(inlier) >= math.floor(self.inlierRatio * len(data)) and len(inlier) > len(bestIn):
                print("Found something", len(data), self.inlierRatio, self.inlierRatio * len(data))

                bestIn = inlier
                bestOut = outlier

                # create line out of sample
                x = sampleB[0] - sampleA[0]
                if x:
                    m = (sampleB[1] - sampleA[1]) / x
                    b = sampleB[1] - (m * sampleB[0])

                    bestParameter1 = m
                    bestParameter2 = b
                else:
                    # vertical line
                    bestParameter1 = sampleA[0]
                    bestParameter2 = None

                #data = inlier

        print("PARAMETER:", bestParameter1, bestParameter2, "\nINLIER:", len(bestIn))

        return bestParameter1, bestParameter2, bestOut


# test
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

    ransac = Ransac(50, 0.4, 1/6)

    for i in range(3):
        # run ransac
        bestParameter1, bestParameter2, outlier = ransac.getOutlier(outlier)

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
