import numpy as np
import BISEC as B


def compute_gait(start, target, obstacles):
    hit_obstacle = None

    direction = np.array(target) - np.array(start)
    distance = np.linalg.norm(direction)
    gait_vec = direction / distance
    max_steplength = min(60, max(-60, distance))
    gait = gait_vec * max_steplength

    start = (start[0] + gait[0], start[1] + gait[1])

    for k in obstacles:
        if B.dist(start, k) <= k[2] + B.robot_radius:
            if hit_obstacle is None:
                hit_obstacle = k
            else:
                if B.dist(start, k) < B.dist(start, hit_obstacle):
                    hit_obstacle = k

    if not hit_obstacle:
        return gait
    else:
        l = (gait_vec[1] * -60, gait_vec[0] * 60)
        r = (gait_vec[1] * 60, gait_vec[0] * -60)

        d = (hit_obstacle[0] - start[0]) * (target[1] - start[1]) - (
                    hit_obstacle[1] - start[1]) * (target[0] - start[0])
        if d < 0:
            return r
        else:
            return l
