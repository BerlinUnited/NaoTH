import math
from tools import action as a
from tools import Simulation as Sim
from naoth import math2d as m2d
from tools import field_info as field


class State:
    def __init__(self):
        self.pose = m2d.Pose2D()
        self.pose.translation = m2d.Vector2(4200, 0)
        self.pose.rotation = math.radians(0)

        self.ball_position = m2d.Vector2(100.0, 0.0)

        self.obstacle_list = ([])  # is in global coordinates

    def update_pos(self, glob_pos, rotation):
        self.pose.translation = glob_pos
        self.pose.rotation = rotation


def main():
    # This takes hours
    state = State()

    cell_width = 100
    no_action = a.Action("none", 0, 0, 0, 0)
    kick_short = a.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)
    sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)

    action_list = [no_action, kick_short, sidekick_left, sidekick_right]

    with open('decision-simulate_every_pos.txt', 'w') as f:
        # xrange is only slightly faster in python 2.7
        for rot in range(0, 360, 90):
            for x in range(int(-field.x_field_length*0.5)+cell_width, int(field.x_field_length*0.5), 2*cell_width):
                for y in range(int(-field.y_field_length*0.5)+cell_width, int(field.y_field_length*0.5), 2*cell_width):
                    state.update_pos(m2d.Vector2(x, y), rotation=rot)
                    actions_consequences = []
                    # Simulate Consequences
                    for action in action_list:
                        single_consequence = a.ActionResults([])
                        actions_consequences.append(Sim.simulate_consequences(action, single_consequence, state, a.num_particles))

                    # Decide best action
                    best_action = Sim.decide_smart(actions_consequences, state)

                    f.write('{}\t'.format(rot))
                    f.write('{}\t'.format(x))
                    f.write('{}\t'.format(y))
                    f.write('{}\n'.format(best_action))

if __name__ == "__main__":
    '''
        This should show the same behavior as the SimulationTest Module
        Note: in front of the goalposts the real robot also uses it's US Sensors, i think???
    '''
    main()
