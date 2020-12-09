import numpy as np
from numpy import linalg as LA
import math
import random


class Result:
    def __init__(self):
        self.sampleA = None
        self.sampleB = None
        self.inlier = []
        self.outlier = []
        self.x_range = None
        self.y_range = None

        self.error = 0

    # create line out of sample
    def getLine(self):
        x = self.sampleB[0] - self.sampleA[0]
        if x:
            m = (self.sampleB[1] - self.sampleA[1]) / float(x)
            b = self.sampleB[1] - (m * self.sampleB[0])

            return (m, b)
        else:
            return (self.sampleA[0],)


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
            return None

        result = Result()

        for i in range(self.iterations):
            # ----------
            # Prediction
            # ----------

            # pick two random points
            pick = random.sample(range(0, len(data)), 2)

            sample = data[pick[0]]
            sampleA = (sample[0], sample[1])
            sampleA_direction = (sample[2], sample[3])

            sample = data[pick[1]]
            sampleB = (sample[0], sample[1])
            sampleB_direction = (sample[2], sample[3])

            # ----
            # Test
            # ----

            if Ransac.__angle_between(sampleA_direction, sampleB_direction) > self.threshAngle:
                # not same orientation!
                continue

            lineNormal = (sampleA_direction[0] + sampleB_direction[0], sampleA_direction[1] + sampleB_direction[1])

            lineVec = np.subtract(sampleB, sampleA)

            outlier = []
            inlier = []

            maxX = data[0][0]
            maxY = data[0][1]
            minX = data[0][0]
            minY = data[0][1]

            error = 0

            for edgel in data:
                direction = (edgel[2], edgel[3])
                if Ransac.__angle_between(lineNormal, direction) > self.threshDist:
                    # not same orientation!
                    outlier.append(edgel)
                    continue

                point = (edgel[0], edgel[1])
                d = LA.norm(np.cross(lineVec, np.subtract(sampleA, point))) / LA.norm(lineVec)

                if abs(d) <= self.threshDist:
                    inlier.append(edgel)

                    error += abs(d)

                    # find endpoints
                    maxX = max(maxX, edgel[0])
                    maxY = max(maxY, edgel[1])
                    minX = min(minX, edgel[0])
                    minY = min(minY, edgel[1])
                else:
                    outlier.append(edgel)

            #print(len(inlier))

            if len(inlier) >= self.minInlier and len(inlier) > len(result.inlier):
                #print("Found something", len(inlier))
                result.x_range = (minX, maxX)
                result.y_range = (minY, maxY)

                result.inlier = inlier
                result.outlier = outlier

                result.sampleA = sampleA
                result.sampleB = sampleB

                result.error = error

        if not result.inlier:
            return None

        return result
