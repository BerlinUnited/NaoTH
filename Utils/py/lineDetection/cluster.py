import numpy as np
from numpy import linalg as LA
import math

class Cluster:

    @staticmethod
    def unit_vector(vector):
        return vector / np.linalg.norm(vector)

    @staticmethod
    def angle_between(v1, v2):
        v1_u = Cluster.unit_vector(v1)
        v2_u = Cluster.unit_vector(v2)
        return math.acos(np.clip(np.dot(v1_u, v2_u), -1.0, 1.0))

    @staticmethod
    def buildCluster(data):
        # canopy clustering
        cluster = []

        # loose angle
        t1 = math.radians(10)
        # tight angle
        t2 = math.radians(5)

        startPoint = data.pop()
        edgelList = data
        currCluster = [startPoint]
        cluster.append(currCluster)

        while 1:
            newEdgelList = []
            startOrientation = (startPoint[2], startPoint[3])

            for edgel in edgelList:
                orientation = (edgel[2], edgel[3])

                cos_angle = Cluster.angle_between(startOrientation, orientation)

                if cos_angle < t2:
                    currCluster.append(edgel)
                #elif cos_angle > t1:
                #    currCluster.append(edgel)
                #    newEdgelList.append(edgel)
                else:
                    newEdgelList.append(edgel)

            if not newEdgelList:
                break
            startPoint = newEdgelList.pop()
            currCluster = [startPoint]
            cluster.append(currCluster)
            edgelList = newEdgelList

        return cluster
