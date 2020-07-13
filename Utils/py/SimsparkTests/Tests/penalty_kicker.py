"""
Tests the penalty kicker ability of the robot.
"""

import math
import functools
import logging
import time

from naoth.log._parser import BehaviorParser
from naoth.pb import Framework_Representations_pb2, CommonTypes_pb2, Messages_pb2
from naoth.utils import DebugCommand

from SimsparkController import SimsparkController
from AgentController import AgentController
from Utils import *

def ball_out_or_doesnt_move(s: SimsparkController, last_ball):
    _ = s.get_ball()

    if _['x'] < 4.5 and abs(_['y']) < 3.0 and math.sqrt(math.pow(_['x'] - last_ball['x'], 2) + math.pow(_['y'] - last_ball['y'], 2)) > 0.0:
        last_ball['x'] = _['x']
        last_ball['y'] = _['y']
        return False
    return True

def frame_info_printer(cmd: DebugCommand):
    fi = Framework_Representations_pb2.FrameInfo()
    fi.ParseFromString(cmd.result())
    print(cmd.id, ':', fi.frameNumber, '@', fi.time)

def behavior_printer(parser, cmd: DebugCommand):
    parser.parse('BehaviorStateSparse', cmd.result())
    if parser.isActiveOption('penalty_kicker'):
        print(parser.getActiveOptionState('penalty_kicker'))

def penalty_kicker(args):
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
    wait_for(lambda: s.get_robot(3) is not None, 0.3)
    # wait until the player is on the field
    wait_for(lambda: s.get_robot(3)['z'] <= 0.2, 0.3)

    logging.info('Some Simspark commands')

    # eg. for en-/disabling a module
    #a.module('VirtualVisionProcessor', False)
    a.agent('penalty')

    # some tests commands
    for i in range(1):
        # prepare the test
        s.cmd_dropball()                # put the ball into the game
        # first place the robot! If we run this test multiple times, the robot could stand on the penalty mark and if the
        # ball gets placed first, it would 'jump' away.
        s.cmd_agentMove(3, 2, 0, r=-90) # place the robot in front of penalty mark
        s.cmd_ballPos(2.64, 0)          # place the ball on the penalty mark

        # if we do this test multiple times, the simulation commands are sometimes a little bit delayed
        wait_for(lambda: s.get_ball()['x'] - 2.64 <= 0.01, 0.3)

        # put the robot in play mode
        a.debugrequest('gamecontroller:game_state:play', True)

        # wait for robot touches the ball (the ball moved!)
        #wait_for(lambda: s.get_ball()['x'] - 2.64 > 0.01, 0.3)
        while not (s.get_ball()['x'] - 2.64 > 0.01):
            a.representation('FrameInfo', binary=True).add_done_callback(frame_info_printer)
            a.behavior().add_done_callback(functools.partial(behavior_printer, parser))

            time.sleep(0.3)

        # its a simulation of a free kick - the ball should be touched only once
        a.debugrequest('gamecontroller:game_state:play', False)

        # HACK!: to preserve the previous ball location in subsequent function calls, we use a dict in global namespace.
        #        Changes to the dict's elements doesn't change the reference to the dict itself - in contrast to other types
        # TODO: is there a better way?
        last_ball = {'x': s.get_ball()['x'], 'y': s.get_ball()['y']}
        wait_for(ball_out_or_doesnt_move, 0.3, 0.1, 3600.0, s, last_ball)

        # print some infos
        robot = s.get_robot(3)
        print('Robot is at {},{}'.format(robot['x'], robot['y']))
        ball = s.get_ball()
        print('Ball is at {},{}'.format(ball['x'], ball['y']))

        # print the result
        if ball['x'] > 4.5 and abs(ball['y']) < 0.75:
            print('GOAL!!!!')
        else:
            print('missed ... :(')

        # wait before starting next round
        time.sleep(1)

    a.stop()
    s.stop()

    # return True if the test was successful
    return True