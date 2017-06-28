import numpy as np
from sys import argv
from matplotlib import pyplot as plt
if len(argv) == 1:
    sys.exit()

filenames = []
for k in range(1, len(argv)):
    filenames.append(argv[k])

robot_pos  = []
targets    = []
obstacles  = []
paths_LPG  = []
paths_B    = []
paths_naiv = []
times_LPG  = []
times_B    = []
times_naiv = []

for k in filenames:
    file = open(k, 'r')
    all = np.load(file)

    robot_pos  += all[0]
    targets    += all[1]
    obstacles  += all[2]
    paths_LPG  += all[3]
    paths_B    += all[4]
    paths_naiv += all[5]
    times_LPG  += all[6]
    times_B    += all[7]
    times_naiv += all[8]

print("Anzahl der Daten: " + str(len(robot_pos)))
print("")

LPG_dist  = []
B_dist    = []
naiv_dist = []

LPG_smooth  = []
B_smooth    = []
naiv_smooth = []

dead_LPG  = 0
dead_B    = 0
dead_naiv = 0

for k in range(0, len(robot_pos)):

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
        smooth += angle
    LPG_dist.append(dist)
    LPG_smooth.append(smooth * 1000 / dist)

    dist   = 0
    smooth = 0
    for l in range(0, len(paths_B[k]) - 1):
        dist += np.linalg.norm(np.array(paths_B[k][l+1]) - np.array(paths_B[k][l]))
        angle = np.absolute(np.arctan2(paths_B[k][l+1][1], paths_B[k][l+1][0]) - np.arctan2(paths_B[k][l][1], paths_B[k][l][0]))
        smooth += angle
    B_dist.append(dist)
    B_smooth.append(smooth * 1000 / dist)

    dist   = 0
    smooth = 0
    for l in range(0, len(paths_naiv[k]) - 1):
        dist += np.linalg.norm(np.array(paths_naiv[k][l+1]) - np.array(paths_naiv[k][l]))
        angle = np.absolute(np.arctan2(paths_naiv[k][l+1][1], paths_naiv[k][l+1][0]) - np.arctan2(paths_naiv[k][l][1], paths_naiv[k][l][0]))
        smooth += np.absolute(angle)
    naiv_dist.append(dist)
    naiv_smooth.append(smooth * 1000 / dist)

median_LPG_dist      = np.median(LPG_dist)
median_B_dist      = np.median(B_dist)
median_naiv_dist   = np.median(naiv_dist)
median_LPG_smooth  = np.median(LPG_smooth)
median_B_smooth    = np.median(B_smooth)
median_naiv_smooth = np.median(naiv_smooth)

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

print("")
print("---------")
print("")
print("Relative path lengths")
relative_B_dist   = np.divide(B_dist, naiv_dist)
relative_LPG_dist = np.divide(LPG_dist, naiv_dist)
print("LPG    " + str(np.mean(relative_LPG_dist)))
print("BISEC: " + str(np.mean(relative_B_dist)))

print("")
print("---------")
print("")
print("Relative path smoothness")
relative_B_smooth   = np.divide(B_smooth, naiv_smooth)
relative_LPG_smooth = np.divide(LPG_smooth, naiv_smooth)
print("LPG    " + str(np.mean(relative_LPG_smooth)))
print("BISEC: " + str(np.mean(relative_B_smooth)))

print("")
print("---------")
print("")
print("Standardabweichung Laenge")

std_B_dist = np.std(B_dist)
std_LPG_dist = np.std(LPG_dist)
std_naiv_dist = np.std(naiv_dist)

print("LPG:   " + str(std_LPG_dist))
print("BISEC: " + str(std_B_dist))
print("naive: " + str(std_naiv_dist))

print("")
print("Standardabweichung Glattheit")

std_B_smooth = np.std(B_smooth)
std_LPG_smooth = np.std(LPG_smooth)
std_naiv_smooth = np.std(naiv_smooth)

print("LPG:   " + str(std_LPG_smooth))
print("BISEC: " + str(std_B_smooth))
print("naive: " + str(std_naiv_smooth))


plt.plot(naiv_dist, LPG_dist, '.')
plt.plot(naiv_dist, B_dist, '+')
plt.plot([0, 6000], [0, 6000])
plt.show()

plt.plot(naiv_smooth, LPG_smooth, '.')
plt.plot(naiv_smooth, B_smooth, '+')
plt.plot([0, 16], [0, 16])
plt.show()
