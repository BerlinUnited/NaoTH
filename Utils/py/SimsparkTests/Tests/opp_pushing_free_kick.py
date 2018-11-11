import math

from naoth import BehaviorParser, Framework_Representations_pb2

from AgentController import AgentController
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
    pending_cmds = {}
    # max. 90sec for this test
    while time.monotonic() <= (start + 90):
        pending_cmds[a.behavior()] = 'Behavior'
        time.sleep(0.3)

        for p in pending_cmds:
            data = a.command_result(p)
            if data:
                if pending_cmds[p] == 'Behavior':
                    parser.parse(data)
                    if parser.isActiveOption('free_kick_opp'):
                        print(parser.getActiveOptionState('free_kick_opp'))
                        if parser.getActiveOptionState('free_kick_opp') == 'done' and time.monotonic() > (start + 10):
                            print(parser.symbols.decimal('ball.distance'))
                            start = 0
                            break

    # its a simulation of a free kick - the ball should be touched only once
    wait_for_cmd(a, a.debugrequest('gamecontroller:game_state:play', False))
    # wait until the robot sits (hip is below 0.2)!
    wait_for(lambda: s.get_robot(a.number)['z'] <= 0.2, 0.3, max_time=5.0)

def evaluate_position(s, a):
    logging.info('evaluate situation')
    #{'type': 'nao', 'team': 'Left', 'number': 3, 'x': -2.25136, 'y': 1.31149, 'z': 0.285937, 'r': 0.3561912312607644}

def opp_pushing_free_kick(args):
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
    a.debugrequest('gamecontroller:set_play:pushing_free_kick', True)

    ball = (-1.5, 1.5)

    position_and_execute(s, a, parser, (-2, 1.5, -90), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-2, 1.9, -90), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-1.5, 2, -180), ball)
    evaluate_position(s, a)
    time.sleep(0.5)
    
    position_and_execute(s, a, parser, (-1.2, 1.8, 140), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-1.2, 1.15, 40), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-1.2, 1.5, 90), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    # next ball position
    ball = (-3.5, 1.5)

    position_and_execute(s, a, parser, (-4, 1.5, -90), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-4, 2, -90), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-3.5, 2, -180), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-3.2, 1.8, 140), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-3.2, 1.15, 40), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-3.2, 1.5, 90), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    # next ball position
    ball = (-1.5, -1.5)

    position_and_execute(s, a, parser, (-2, -1.5, -90), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-2, -2, -90), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-1.5, -2, 0), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-1.2, -1.8, 45), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-1.2, -1.15, 125), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-1.2, -1.5, 90), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    # next ball position
    ball = (-3.5, -1.5)

    position_and_execute(s, a, parser, (-4, -1.5, -90), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-4, -2, -90), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-3.5, -2, 0), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-3.2, -1.8, 60), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-3.2, -1.15, 135), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    position_and_execute(s, a, parser, (-3.2, -1.5, 90), ball)
    evaluate_position(s, a)
    time.sleep(0.5)

    a.cancel()
    s.cancel()

    a.join()
    s.join()

    return 0