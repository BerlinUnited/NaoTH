import LPG
from random import randint

#LPG.compute_waypoints_LPG([3000, 0])

obstacles = [[1000, 2500], [-4500, -3000], [500, 2245], [-1600, -1000]]
#target = [-1500, -2500]
target = [randint(-45000, 45000)/10, randint(-30000, 30000)/10]

# plot
while True:
    target = [randint(-45000, 45000)/10, randint(-30000, 30000)/10]
    LPG.draw(obstacles, target, "LPG")
