# script for reward functions

def simple_reward_and_termination(world, state):
    """
    simple reward function which checks whether the ball is in one of the goal
        - this returns a final reward and sets the termination state to True
        - else the reward is -1 and termination state is False

    :param state: state object as in state.py
    :return: reward (float), termination state (bool)
    """


    if world.in_opp_goal(state.position, state.ball_position):
        return 100.  # reward = 100
    elif world.in_own_goal(state.position, state.ball_position):
        return -100.  # reward = -100

    return -1  # reward = -1



"""
/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

plenty of room for more reward functions

/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
"""
