import math
import logging
import time

from naoth.log._parser import BehaviorParser
from naoth.pb import Framework_Representations_pb2
from naoth.utils import DebugCommand

from SimsparkController import SimsparkController
from AgentController import AgentController
from Utils import *


class PenaltyKicker(TestRun):
    """Tests the penalty kicker ability of the robot."""

    def __init__(self, args):
        self.simspark_application = args.simspark
        self.simspark_start_instance = not args.no_simspark

        self.agent_application = args.agent
        self.agent_config_dir = args.config
        self.agent_start_instance = not args.no_agent

        self.simspark = None
        self.agent = None
        self.parser = BehaviorParser()  # create parser for the agent behavior

    def setUp(self):
        self.simspark = SimsparkController(self.simspark_application, start_instance=self.simspark_start_instance)
        self.simspark.start()
        self.simspark.wait_connected()  # wait for the monitor to be connected

        self.agent = AgentController(self.agent_application,
                                     self.agent_config_dir,
                                     number=3, start_instance=self.agent_start_instance)
        self.agent.start()
        logging.info('Wait for agent connection')
        self.agent.wait_connected()  # wait for the agent to be fully started

        # in order to use the parser, we have to retrieve the complete behavior first
        self.parser.init(self.agent.behavior(True).result())

        # it takes sometimes a while until simspark got the correct player number
        wait_for(lambda: self.simspark.get_robot(3) is not None, 0.3)
        # wait until the player is on the field
        wait_for(lambda: self.simspark.get_robot(3)['z'] <= 0.2, 0.3)

        # set the behavior of the agent
        self.agent.agent('penalty')
        # put the ball into the game
        self.simspark.cmd_dropball()

    def tearDown(self):
        # stop the agent
        if self.agent:
            self.agent.stop()
        # stop the simulation/simspark
        if self.simspark:
            self.simspark.stop()

    def test_penalty_kick(self):
        # first place the robot! If we run this test multiple times, the robot could stand on the penalty mark and if
        # the ball gets placed first, it would 'jump' away.
        self.simspark.cmd_agentMove(3, 2, 0, r=-90)  # place the robot in front of penalty mark
        self.simspark.cmd_ballPos(2.64, 0)           # place the ball on the penalty mark

        # if we do this test multiple times, the simulation commands are sometimes a little bit delayed
        wait_for(lambda: self.simspark.get_ball()['x'] - 2.64 <= 0.01, 0.3)

        # put the robot in play mode
        self.agent.debugrequest('gamecontroller:game_state:play', True)

        # wait for robot touches the ball (the ball moved!)
        while not (self.simspark.get_ball()['x'] - 2.64 > 0.01):
            self.agent.representation('FrameInfo', binary=True).add_done_callback(self.frame_info_printer)
            self.agent.behavior().add_done_callback(self.behavior_printer)

            time.sleep(0.3)

        # its a simulation of a free kick - the ball should be touched only once
        self.agent.debugrequest('gamecontroller:game_state:play', False)

        # HACK!: to preserve the previous ball location in subsequent function calls, we use a dict in global namespace.
        #        Changes to the dict's elements doesn't change the reference to the dict itself - in contrast to other types
        # TODO: is there a better way?
        last_ball = {'x': self.simspark.get_ball()['x'], 'y': self.simspark.get_ball()['y']}
        wait_for(self.ball_out_or_doesnt_move, 0.3, 0.1, 3600.0, last_ball)

        # print some infos
        robot = self.simspark.get_robot(3)
        print('Robot is at {},{}'.format(robot['x'], robot['y']))
        ball = self.simspark.get_ball()
        print('Ball is at {},{}'.format(ball['x'], ball['y']))

        # print the result
        successful_test = ball['x'] > 4.5 and abs(ball['y']) < 0.75
        print('GOAL!!!!' if successful_test else 'missed ... :(')

        return successful_test

    def ball_out_or_doesnt_move(self, last_ball):
        _ = self.simspark.get_ball()

        ball_moved = math.sqrt(math.pow(_['x'] - last_ball['x'], 2) + math.pow(_['y'] - last_ball['y'], 2)) > 0.0
        if _['x'] < 4.5 and abs(_['y']) < 3.0 and ball_moved:
            last_ball['x'] = _['x']
            last_ball['y'] = _['y']
            return False
        return True

    def frame_info_printer(self, cmd: DebugCommand):
        fi = Framework_Representations_pb2.FrameInfo()
        fi.ParseFromString(cmd.result())
        print(cmd.id, ':', fi.frameNumber, '@', fi.time)

    def behavior_printer(self, cmd: DebugCommand):
        self.parser.parse('BehaviorStateSparse', cmd.result())
        if self.parser.isActiveOption('penalty_kicker'):
            print(self.parser.getActiveOptionState('penalty_kicker'))
