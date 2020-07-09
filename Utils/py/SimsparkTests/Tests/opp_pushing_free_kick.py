import math

from naoth.log._parser import BehaviorParser

from SimsparkController import SimsparkController
from Utils import *


def position_and_execute(s, a, parser, pos, ball):
    # first place the robot! If we run this test multiple times, the robot could stand on the penalty mark and if the
    # ball gets placed first, it would 'jump' away.
    s.cmd_agentMove(a.number, pos[0], pos[1], r=pos[2])  # place the robot
    s.cmd_ballPos(ball[0], ball[1])  # place the ball

    # if we do this test multiple times, the simulation commands are sometimes a little bit delayed
    wait_for(lambda: s.get_ball()['x'] - ball[0] <= 0.01, 0.3)

    # put the robot in play mode
    a.debugrequest('gamecontroller:game_state:play', True)

    start = time.monotonic()
    # max. 90sec for this test (NOTE: walking of the robot in the simulation is not smooth, therefore we wait longer)
    while time.monotonic() <= (start + 90):
        parser.parse(a.behavior().result())
        if parser.isActiveOption('free_kick_opp'):
            #print(parser.getActiveOptionState('free_kick_opp'))
            if parser.getActiveOptionState('free_kick_opp') == 'done' and time.monotonic() > (start + 10):
                #print(parser.symbols.decimal('ball.distance'))
                start = 0
                break

    # its a simulation of a free kick - the ball should be touched only once
    a.debugrequest('gamecontroller:game_state:play', False).result()
    # wait until the robot sits (hip is below 0.2)!
    wait_for(lambda: s.get_robot(a.number)['z'] <= 0.2, 0.3, max_time=5.0)

    return evaluate_position(s, a, ball)

def evaluate_position(s, a, ball):
    # first check if the ball was moved/touched
    s_ball = s.get_ball()
    if abs(s_ball['x'] - ball[0]) + abs(s_ball['y']-ball[1]) > 0.1:
        logging.info('Ball touched!')
        return False

    robot = s.get_robot(a.number)

    # check if the robot is away from the ball (min. 75cm)
    if math.sqrt(math.pow(ball[0] - robot['x'], 2) + math.pow(ball[1] - robot['y'], 2)) < 0.75:
        logging.info("Robot is to close to the ball!")
        return False

    # check if the robot is somewhere near the line between ball and goal
    # TODO: doesn't work as expected! :/
    if abs(ball[1]/(ball[0]+4.5) - robot['y']/(robot['x']+4.5))>0.1:
        logging.info("Robot doesn't block a goal kick!")
        return False

    return True

def opp_pushing_free_kick(args):
    return_code = 0

    s = SimsparkController(args.simspark, start_instance=not args.no_simspark)
    s.start()
    s.wait_connected() # wait for the monitor to be connected

    a = AgentController(args.agent, args.config, number=3, start_instance=not args.no_agent)
    a.start()
    logging.info('Wait for agent connection')
    a.wait_connected()  # wait for the agent to be fully started

    # create parser for the agents behavior
    parser = BehaviorParser()
    # in order to use the parser, we have to retrieve the complete behavior first
    parser.init(a.behavior(True).result())

    # it takes sometimes a while until simspark got the correct player number
    wait_for(lambda: s.get_robot(a.number) is not None, 0.3)
    # wait until the player is on the field
    wait_for(lambda: s.get_robot(a.number)['z'] <= 0.3, 0.3, max_time=5.0)

    logging.info('Some Simspark commands')

    # eg. for en-/disabling a module
    a.agent('soccer_agent')

    # prepare the test
    s.cmd_dropball()  # put the ball into the game
    a.debugrequest('gamecontroller:set_play:pushing_free_kick', True)

    positions = {
        'left_far':    [ (-1.5, 1.5),  (-2,  1.5, -90), (-2, 1.9, -90), (-1.5, 2, -180), (-1.2, 1.8, 140), (-1.2, 1.15, 40), (-1.2, 1.5, 90) ],
        'left_close':  [ (-3.5, 1.5),  (-4,  1.5, -90), (-4,  2, -90), (-3.5, 2, -180), (-3.2, 1.8, 140), (-3.2, 1.15, 40), (-3.2, 1.5, 90) ],
        'right_far':   [ (-1.5, -1.5), (-2, -1.5, -90), (-2, -2, -90), (-1.5, -2, 0), (-1.2, -1.8, 45), (-1.2, -1.15, 125), (-1.2, -1.5, 90) ],
        'right_close': [ (-3.5, -1.5), (-4, -1.5, -90), (-4, -2, -90), (-3.5, -2, 0), (-3.2, -1.8, 60), (-3.2, -1.15, 135), (-3.2, -1.5, 90) ]
    }
    for _ in positions:
        for robot in positions[_][1:]:
            if not position_and_execute(s, a, parser, robot, positions[_][0]):
                logging.error("Failed opponent free kick (%s): ball@%s, robot@%s", _, str(positions[_][0]), str(robot))
                return_code += 1
            time.sleep(0.5)

    a.stop()
    s.stop()

    return return_code == 0