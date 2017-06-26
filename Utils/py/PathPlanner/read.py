import numpy as np
from sys import argv
from matplotlib import pyplot as plt
script, filename = argv

file = open(filename, 'r')
all = np.load(file)

robot_pos  = all[0]
targets    = all[1]
obstacles  = all[2]
paths_LPG  = all[3]
paths_B    = all[4]
paths_naiv = all[5]

if len(all) > 6:
    times_LPG  = all[6]
    times_B    = all[7]
    times_naiv = all[8]

LPG_dist  = []
B_dist    = []
naiv_dist = []

LPG_smooth  = []
B_smooth    = []
naiv_smooth = []

dead_LPG  = 0
dead_B    = 0
dead_naiv = 0

for k in range(0, len(all[0])):

    if len(paths_LPG[k]) < 2 or len(paths_B[k]) < 2 or len(paths_naiv[k]) < 2:
        if len(paths_LPG[k]) < 2:
            dead_LPG += 1
        if len(paths_B[k]) < 2:
            dead_B += 1
        if len(paths_naiv[k]) < 2:
            dead_naiv += 1
        continue

    dist   = 0
    smooth = 0
    for l in range(0, len(paths_LPG[k]) - 1):
        dist += np.linalg.norm(np.array(paths_LPG[k][l+1]) - np.array(paths_LPG[k][l]))
        angle = np.absolute(np.arctan2(paths_LPG[k][l+1][1], paths_LPG[k][l+1][0]) - np.arctan2(paths_LPG[k][l][1], paths_LPG[k][l][0]))
        """if (angle < 0):
            angle += 2 * np.pi"""
        smooth += angle
    LPG_dist.append(dist)
    LPG_smooth.append(smooth * 1000 / dist)

    dist   = 0
    smooth = 0
    for l in range(0, len(paths_B[k]) - 1):
        dist += np.linalg.norm(np.array(paths_B[k][l+1]) - np.array(paths_B[k][l]))
        angle = np.absolute(np.arctan2(paths_B[k][l+1][1], paths_B[k][l+1][0]) - np.arctan2(paths_B[k][l][1], paths_B[k][l][0]))
        """if (angle < 0):
            angle += 2 * np.pi"""
        smooth += angle
    B_dist.append(dist)
    B_smooth.append(smooth * 1000 / dist)

    dist   = 0
    smooth = 0
    for l in range(0, len(paths_naiv[k]) - 1):
        dist += np.linalg.norm(np.array(paths_naiv[k][l+1]) - np.array(paths_naiv[k][l]))
        angle = np.absolute(np.arctan2(paths_naiv[k][l+1][1], paths_naiv[k][l+1][0]) - np.arctan2(paths_naiv[k][l][1], paths_naiv[k][l][0]))
        """if (angle < 0):
            angle += 2 * np.pi"""
        smooth += np.absolute(angle)
    naiv_dist.append(dist)
    naiv_smooth.append(smooth * 1000 / dist)

avg_LPG_dist  = np.average(LPG_dist)
avg_B_dist    = np.average(B_dist)
avg_naiv_dist = np.average(naiv_dist)
avg_LPG_smooth  = np.average(LPG_smooth)
avg_B_smooth    = np.average(B_smooth)
avg_naiv_smooth = np.average(naiv_smooth)

print("Avg. Distance")
print("LPG:   " + str(avg_LPG_dist))
print("BISEC: "  + str(avg_B_dist))
print("naive: " + str(avg_naiv_dist))
print("")
print("Avg. Smoothness")
print("LPG:   " + str(avg_LPG_smooth))
print("BISEC: " + str(avg_B_smooth))
print("naive: " + str(avg_naiv_smooth))
print("")
print("--------")

median_LPG_dist      = np.median(LPG_dist)
median_B_dist      = np.median(B_dist)
median_naiv_dist   = np.median(naiv_dist)
median_LPG_smooth  = np.median(LPG_smooth)
median_B_smooth    = np.median(B_smooth)
median_naiv_smooth = np.median(naiv_smooth)
print("")
print("median Distance")
print("LPG:   " + str(median_LPG_dist))
print("BISEC: " + str(median_B_dist))
print("naive: " + str(median_naiv_dist))
print("")
print("median Smoothness")
print("LPG:   " + str(median_LPG_smooth))
print("BISEC: " + str(median_B_smooth))
print("naive: " + str(median_naiv_smooth))

print("")
print("---------")
print("")
print("Deadlocks")
print("LPG:   " + str(dead_LPG))
print("BISEC: " + str(dead_B))
print("naive: " + str(dead_naiv))
print("")
if len(all) > 6:
    print("---------")
    print("")
    print("Avg. Times")
    print(np.average(times_LPG))
    print(np.average(times_B))
    print(np.average(times_naiv))

relative_B   = np.divide(B_dist, naiv_dist)
relative_LPG = np.divide(LPG_dist, naiv_dist)
print(np.mean(relative_B))
print(np.mean(relative_LPG))

plt.plot(naiv_dist, LPG_dist, '.')
plt.plot(naiv_dist, B_dist, '+')
plt.plot([0, 6000], [0, 6000])
plt.show()
