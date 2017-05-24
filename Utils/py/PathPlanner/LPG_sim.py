import LPG
from random import randint

#LPG.compute_waypoints_LPG([3000, 0])

#obstacles = [(1000, 0, 400)] #[(), ()] ansonsten fehler
obstacles = [(1600, 0, 400), (2000, -500, 400)]
target = [3500, 0]
#target = [randint(-45000, 45000)/10, randint(-30000, 30000)/10]

# plot
while True:
    #target = [randint(-45000, 45000)/10, randint(-30000, 30000)/10]
    LPG.draw(obstacles, target, "LPG")
