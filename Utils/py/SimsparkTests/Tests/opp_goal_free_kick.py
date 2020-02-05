import math

from naoth.log import BehaviorParser

from SimsparkController import SimsparkController
from Utils import *


def position_and_execute(s, a, parser, pos, ball):
    # first place the robot! If we run this test multiple times, the robot could stand on the penalty mark and if the
    # ball gets placed first, it would 'jump' away.
    s.cmd_agentMove(a.number, pos[0], pos[1], r=pos[2])  # place the robot
    s.cmd_ballPos(ball[0], ball[1])  # place the ball

    # the robot should forget the previous ball!
    a.debugrequest('MultiKalmanBallLocator:remove_all_models', True)
    time.sleep(0.5)
    a.debugrequest('MultiKalmanBallLocator:remove_all_models', False)

    # if we do this test multiple times, the simulation commands are sometimes a little bit delayed
    wait_for(lambda: s.get_ball()['x'] - ball[0] <= 0.01, 0.3)

    # put the robot in play mode
    a.debugrequest('gamecontroller:game_state:play', True)

    start = time.monotonic()
    pending_cmds = {}
    # max. 90sec for this test (NOTE: walking of the robot in the simulation is not smooth, therefore we wait longer)
    while time.monotonic() <= (start + 90):
        pending_cmds[a.behavior()] = 'Behavior'
        time.sleep(0.3)

        for p in pending_cmds:
            data = a.command_result(p)
            if data:
                if pending_cmds[p] == 'Behavior':
                    parser.parse(data)
                    #print(parser.getActiveOptions())
                    if parser.isActiveOption('free_kick_opp_goal'):
                        '''
                        print(parser.symbols.boolean('ball.know_where_itis'),
                              parser.symbols.decimal('ball.preview.x'),
                              parser.symbols.decimal('ball.preview.y'),
                              parser.symbols.decimal('ball.x'),
                              parser.symbols.decimal('ball.y'),
                              parser.getActiveOptionState('free_kick_opp_goal')
                        )
                        '''
                        if parser.getActiveOptionState('free_kick_opp_goal') == 'idle' and time.monotonic() > (start + 10):
                            #print(parser.symbols.decimal('ball.distance'))
                            start = 0
                            break

    # its a simulation of a free kick - the ball should be touched only once
    wait_for_cmd(a, a.debugrequest('gamecontroller:game_state:play', False))
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

    return True

def opp_goal_free_kick(args):
    return_code = 0

    s = SimsparkController(args.simspark, not args.no_simspark)
    s.start()
    s.connected.wait() # wait for the monitor to be connected

    a = AgentController(args.agent, args.config, number=3, start_instance=not args.no_agent)
    a.start()
    a.started.wait() # wait for the agent to be fully started

    # create parser for the agents behavior
    parser = BehaviorParser()
    # in order to use the parser, we have to retrieve the complete behavior first
    parser.init(wait_for_cmd(a, a.behavior(True)))

    # it takes sometimes a while until simspark got the correct player number
    wait_for(lambda: s.get_robot(a.number) is not None, 0.3)
    # wait until the player is on the field
    wait_for(lambda: s.get_robot(a.number)['z'] <= 0.3, 0.3, max_time=5.0)

    logging.info('Some Simspark commands')

    # eg. for en-/disabling a module
    a.agent('path_spl_soccer')

    # prepare the test
    s.cmd_dropball()  # put the ball into the game
    a.debugrequest('gamecontroller:secondaryTime:30', True)
    a.debugrequest('gamecontroller:set_play:goal_free_kick', True)

    positions = {
        'right_goal_pos': [(3.0, -1.0),  # ball position
                           (3.5, 2.5, 90), (3.5, 1.5, 90), (3.5, 0.5, 90), (3.5, -0.5, 90), (3.5, -1.5, 90), (3.5, -2.5, 90),
                           (3.0, 2.5, 90), (3.0, 1.5, 90), (3.0, 0.5, 90), (3.0, -0.5, 90), (3.0, -1.5, 90), (3.0, -2.5, 90),
                           (2.5, 2.0, -90), (2.5, 1.5, -90), (2.5, 0.5, -90), (2.5, -0.5, -90), (2.5, -1.5, -90), (2.5, -2.5, -90),
                           (1.5, 2.0, -90), (1.5, 0.0, -90), (1.5, -2.0, -90),
                           (0.5, 0.0, 90),
                           (-1.5, 1.5, 90), (-1.5, -1.5, 90),
                           (-3.5, 0.0, 90),
                           (2.7, -1.0, -90), (2.7, -0.6, -90), (2.9, -0.6, -90), (3.2, -0.6, -90), (3.2, -0.9, -90), (3.2, -1.1, -90),
                           (3.2, -1.3, -90), (3.0, -1.3, -90), (2.8, -1.3, -90),
                           (2.7, -1.1, -90), (2.8, -1.3, -90), (4.0, -1.0, 90), (4.0, -0.8, 90), (4.0, -1.2, 90),
                           (4.0, -2.5, 90), (2.7, 1.0, -90)
        ],
        'left_goal_pos': [ (3.0, 1.0), # ball position
                           (3.5, 2.5, 90),
                           (3.5, 1.5, 90), (3.5, 0.5, 90), (3.5, -0.5, 90),
                           (3.5, -1.5, 90), (3.5, -2.5, 90),
                           (3.0, 2.5, 90), (3.0, 1.5, 90), (3.0, 0.5, 90), (3.0, -0.5, 90), (3.0, -1.5, 90), (3.0, -2.5, 90),
                           (2.5, 2.0, -90), (2.5, 1.5, -90), (2.5, 0.5, -90), (2.5, -0.5, -90), (2.5, -1.5, -90), (2.5, -2.5, -90),
                           (1.5, 2.0, -90), (1.5, 0.0, -90), (1.5, -2.0, -90),
                           (0.5, 0.0, 90),
                           (-1.5, 1.5, 90), (-1.5, -1.5, 90),
                           (-3.5, 0.0, 90),
                           (2.7, 1.0, -90), (2.7, 0.6, -90), (2.9, 0.6, -90), (3.2, 0.6, -90), (3.2, 0.9, -90),
                           (3.2, 1.1, -90), (3.2, 1.3, -90), (3.0, 1.3, -90), (2.8, 1.3, -90), (2.8, 1.3, -90),
                           (2.7, 1.1, -90), (2.8, 1.3, -90), (4.0, 1.0, 90),  (4.0, 0.8, 90),
                           (4.0, 1.2, 90),
                           (4.0, 2.5, 90)
        ]
    }
    for _ in positions:
        for robot in positions[_][1:]:
            if not position_and_execute(s, a, parser, robot, positions[_][0]):
                logging.error("Failed opponent free kick (%s): ball@%s, robot@%s", _, str(positions[_][0]), str(robot))
                return_code += 1
            time.sleep(0.5)

    # TODO: position_for_free_kick (deactivate vision)
    # TODO: move_away_from_ball
    # TODO: move_away_from_teamball

    a.cancel()
    s.cancel()

    a.join()
    s.join()

    return return_code == 0