import math

from naoth import BehaviorParser, Framework_Representations_pb2

from AgentController import AgentController
from SimsparkController import SimsparkController
from Utils import *

def roles_test(args):
    s = SimsparkController(args.simspark, not args.no_simspark)
    s.start()
    s.connected.wait() # wait for the monitor to be connected

    agents = []
    for n in range(1,6):
        agents.append(AgentController(args.agent, args.config, number=n, start_instance=not args.no_agent))

    for a in agents: a.start()
    for a in agents: a.started.wait() # wait for the agent to be fully started

    # it takes sometimes a while until simspark got the correct player number
    for a in agents: wait_for(lambda: s.get_robot(a.number) is not None, 0.3)
    # wait until the player is on the field
    for a in agents: wait_for(lambda: s.get_robot(a.number)['z'] <= 0.4, 0.3)

    logging.info('Enable/Disable some modules')

    # eg. for en-/disabling a module
    #a.module('VirtualVisionProcessor', False)
    for a in agents:
        a.agent('role_spl_soccer')
        a.module('PathPlanner', False)
        a.module('PathPlanner2018', True)
        a.module('SituationPriorProvider', False)
        a.module('MonteCarloSelfLocator', False)
        a.module('OdometrySelfLocator', False)
        a.module('GPS_SelfLocator', True)

    logging.info('Start test run')

    logging.debug('Set robot to initial position')
    s.cmd_agentMove(1, -3.5, 3.2, r=-180)
    s.cmd_agentMove(2, -2.5, 3.2, r=-180)
    s.cmd_agentMove(3, -1.5, 3.2, r=-180)
    s.cmd_agentMove(4, -3.0, -3.2, r=0)
    s.cmd_agentMove(5, -2.0, -3.2, r=0)

    time.sleep(0.5)

    # READY
    for a in agents: a.debugrequest('gamecontroller:game_state:ready', True)

    # simulation is slow, give the robots some time to reach their kickoff position
    time.sleep(120)

    # SET
    for a in agents: a.debugrequest('gamecontroller:game_state:set', True)
    for a in agents: a.debugrequest('gamecontroller:game_state:ready', False)


    # put the robot in play mode
    for a in agents: a.debugrequest('gamecontroller:game_state:play', True)
    for a in agents: a.debugrequest('gamecontroller:game_state:set', False)

    '''

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
        wait_for(ball_out_or_doesnt_move, 0.3, 0.1, s, last_ball)

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
    '''

    logging.info('Finished test run')

    #for a in agents: a.debugrequest('gamecontroller:game_state:ready', False)
    #for a in agents: a.debugrequest('gamecontroller:game_state:set', False)
    for a in agents: a.debugrequest('gamecontroller:game_state:play', False)
    time.sleep(3)

    for a in agents: a.cancel()
    s.cancel()

    for a in agents: a.join()
    s.join()

    # return True if the test was successful
    return True