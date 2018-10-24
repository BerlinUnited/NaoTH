import argparse
import logging
import math
import os
import shutil

from Utils import *
from AgentController import AgentController
from SimsparkController import SimsparkController
from naoth import Framework_Representations_pb2, BehaviorParser

def parseArguments():
    parser = argparse.ArgumentParser(
        description='Performs some tests with simspark.',
        epilog= "Example:\n"
                "\t{0}\n"
                "".format(os.path.basename(__file__)),
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument('-s', '--simspark', default='simspark', action='store', help="")
    parser.add_argument('-a', '--agent', default='../../../NaoTHSoccer/dist/Native/naoth-simspark', action='store', help="")
    parser.add_argument('-c', '--config', default='../../../NaoTHSoccer/', action='store', help="")
    parser.add_argument('-v', '--verbose', action='store_true', help="")

    parser.add_argument('-ns', '--no-simspark', action='store_true', help="")
    parser.add_argument('-na', '--no-agent', action='store_true', help="")

    return parser.parse_args()


def ball_out_or_doesnt_move():
    _ = s.get_ball()
    if _['x'] < 4.5 and abs(_['y']) < 3.0 and math.sqrt(math.pow(_['x'] - last_ball['b']['x'], 2) + math.pow(_['y'] - last_ball['b']['y'], 2)) > 0.0:
        last_ball['b'] = _
        return False
    return True

if __name__ == "__main__":
    args = parseArguments()
    #print(args)

    if args.verbose: logging.basicConfig(level=logging.DEBUG)

    if not shutil.which(args.simspark) and not args.no_simspark:
        logging.error('Can not find simspark application!')
        exit(1)

    if not shutil.which(args.agent) and not args.no_agent:
        logging.error('Can not find simspark agent application!')
        exit(1)

    # TODO: check if the config directory is correct!
    # TODO: arguments for "headless" usage

    #'''
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
    wait_for(lambda: s.get_robot(3) is not None, 0.3)
    # wait until the player is on the field
    wait_for(lambda: s.get_robot(3)['z'] <= 0.2, 0.3)

    logging.info('Some Simspark commands')

    # eg. for en-/disabling a module
    #a.module('VirtualVisionProcessor', False)
    a.agent('path_spl_soccer')

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
        a.debugrequest('gamecontroller:play', True)

        pending_cmds = {}
        # wait for robot touches the ball (the ball moved!)
        #wait_for(lambda: s.get_ball()['x'] - 2.64 > 0.01, 0.3)
        while not (s.get_ball()['x'] - 2.64 > 0.01):
            pending_cmds[a.representation('FrameInfo', binary=True)] = 'FrameInfo'
            pending_cmds[a.behavior()] = 'Behavior'

            time.sleep(0.3)

            for p in pending_cmds:
                data = a.command_result(p)
                if data:
                    if pending_cmds[p] == 'FrameInfo':
                        fi = Framework_Representations_pb2.FrameInfo()
                        fi.ParseFromString(data)
                        print(p, ':', fi.frameNumber, '@', fi.time)
                    elif pending_cmds[p] == 'Behavior':
                        parser.parse(data)
                        if parser.isActiveOption('path_striker2018'):
                            #print(parser.getActiveOptions())
                            #print(parser.getActiveStates())
                            print(parser.getActiveOptionState('path_striker2018'))

        # its a simulation of a free kick - the ball should be touched only once
        a.debugrequest('gamecontroller:play', False)

        # HACK!: to preserve the previous ball location in subsequent function calls, we use a dict in global namespace.
        #        Changes to the dict's elements doesn't change the reference to the dict itself - in contrast to other types
        # TODO: is there a better way?
        last_ball = {'b': s.get_ball()}
        wait_for(ball_out_or_doesnt_move, 0.3, 0.1)

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

    a.cancel()
    s.cancel()

    a.join()
    s.join()
    #'''

    print('DONE')