import random
import numpy as np
from numpy import linalg as LA
import math
import matplotlib.pyplot as plt

# parameter
iterations = 50
threshDist = 0.4
inlierRatio = 1/6
lineCount = 4

line_samples = [i for i in np.arange(0,10,0.1)]

def createLineSample(parameter1, parameter2):
    line = [np.array( (line_samples[i], random.uniform( (parameter1*line_samples[i] + parameter2)   -0.2, (parameter1*line_samples[i] + parameter2)   +0.2)) ) for i in range(len(line_samples))]
    return line


def getOutlier(data):
    bestParameter1 = 0
    bestParameter2 = 0
    bestIn = []
    bestOut = []

    for i in range(iterations):
        # pick two random points
        pick = random.sample(range(0, len(data)), 2)
        sampleA = data[pick[0]]
        sampleB = data[pick[1]]

        # calculate distances of all pointes to the line
        inlier = []
        outlier = []
        lineVec = np.subtract(sampleB, sampleA)
        for point in data:
            d = LA.norm(np.cross(lineVec, np.subtract(sampleA, point))) / LA.norm(lineVec)

            if abs(d) <= threshDist:
                inlier.append(point)
            else:
                outlier.append(point)
        print("INLIER:", len(inlier))


        if len(inlier) >= math.floor(inlierRatio * len(data)) and len(inlier) > len(bestIn):
            bestIn = inlier
            bestOut = outlier

            # create line out of smaple
            m = (sampleB[1]-sampleA[1]) / (sampleB[0] - sampleA[0])
            b = sampleB[1] - (m * sampleB[0])

            bestParameter1 = m
            bestParameter2 = b

            #data = inlier

    print("PARAMETER:", bestParameter1, bestParameter2, "\nINLIER:", len(bestIn))

    return bestParameter1, bestParameter2, bestOut


# create data
noise = [np.array( (random.uniform(0, 10), random.uniform(0, 10)) ) for i in range(250)]
data = createLineSample(1,0) + createLineSample(-1, 10) + createLineSample(0,2) + noise

random.shuffle(data)
#data = sorted(data,  key=lambda elem: elem[0])

print("LEN:", len(data))


# draw points
plt.scatter(*zip(*data))

lines = []
outlier = data

for i in range(lineCount):
    # run ransac
    bestParameter1, bestParameter2, outlier = getOutlier(outlier)

    plt.scatter(*zip(*outlier))

    lines.append( (bestParameter1, bestParameter2) )
    data = outlier


# draw lines
curve_samples = [i for i in np.arange(0,10,0.1)]
for parameter in lines:
    curve = [parameter[0] * i + parameter[1] for i in curve_samples]

    plt.plot(curve_samples, curve)

"""
# fit and draw line
p = np.polyfit(*zip(*bestIn) , 1)
curve_samples = np.arange(0, 10, 0.1)
curve = [p[0]*i + p[1] for i in curve_samples]
"""

plt.show()
