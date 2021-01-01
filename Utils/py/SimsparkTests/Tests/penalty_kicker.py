import math
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
        super().__init__()
        self.simspark_application = args.simspark
        self.simspark_start_instance = not args.no_simspark

        self.agent_application = args.agent
        self.agent_config_dir = args.config
        self.agent_start_instance = not args.no_agent

        self.simspark = None
        self.agent = None
        self.parser = BehaviorParser()  # create parser for the agent behavior

        self.last_ball = {'x': 0, 'y': 0}  # helper var, in order to determine if the ball is still moving
        self.frame_info = Framework_Representations_pb2.FrameInfo()

    def setUp(self):
        self.logger.debug('Init & start SimsparkController')
        self.simspark = SimsparkController(self.simspark_application, start_instance=self.simspark_start_instance)
        self.simspark.start()
        self.logger.debug('Wait for simspark connection')
        self.simspark.wait_connected()  # wait for the monitor to be connected

        self.logger.debug('Init & start AgentController')
        self.agent = AgentController(self.agent_application,
                                     self.agent_config_dir,
                                     number=3, start_instance=self.agent_start_instance)
        self.agent.start()
        self.logger.debug('Wait for agent connection')
        self.agent.wait_connected()  # wait for the agent to be fully started

        # in order to use the parser, we have to retrieve the complete behavior first
        self.logger.debug('Init behavior parser')
        self.parser.init(self.agent.behavior(True).result())

        self.logger.debug('Wait for simspark agent to be ready')
        # it takes sometimes a while until simspark got the correct player number
        wait_for(lambda: self.simspark.get_robot(3) is not None, 0.3)
        # wait until the player is on the field
        wait_for(lambda: self.simspark.get_robot(3)['z'] <= 0.2, 0.3)

        self.logger.debug('Set agent behavior')
        # set the behavior of the agent
        self.agent.agent('penalty')
        # dis-/enable some modules for the simspark simulation
        self.agent.module('PathPlanner', False)
        self.agent.module('PathPlanner2018', True)
        self.agent.module('SituationPriorProvider', False)
        self.agent.module('MonteCarloSelfLocator', False)
        self.agent.module('OdometrySelfLocator', False)
        self.agent.module('GPS_SelfLocator', True)
        # put the ball into the game
        self.simspark.cmd_dropball()

    def tearDown(self):
        # stop the agent
        if self.agent:
            self.logger.debug('Stop agent')
            self.agent.stop()
        # stop the simulation/simspark
        if self.simspark:
            self.logger.debug('Stop simspark')
            self.simspark.stop()

    def test_penalty_kick(self):
        # check if the correct behavior is set
        if self.agent.agent().result().decode() != 'penalty':
            self.logger.error('Agent has the wrong behavior')
            return False

        # first place the robot! If we run this test multiple times, the robot could stand on the penalty mark and if
        # the ball gets placed first, it would 'jump' away.
        self.simspark.cmd_agentMove(3, 2, 0, r=-90)  # place the robot in front of penalty mark
        self.simspark.cmd_ballPos(2.64, 0)           # place the ball on the penalty mark

        # if we do this test multiple times, the simulation commands are sometimes a little bit delayed
        wait_for(lambda: self.simspark.get_ball()['x'] - 2.64 <= 0.01, 0.3)

        # put the robot in play mode
        self.agent.debugrequest('gamecontroller:game_state:play', True)

        end = time.monotonic() + 120
        # wait for robot touches the ball (the ball moved!)
        while not (self.simspark.get_ball()['x'] - 2.64 > 0.01) and time.monotonic() <= end:
            self.agent.representation('FrameInfo', binary=True).add_done_callback(self.frame_info_parser)
            self.agent.behavior().add_done_callback(self.behavior_logger)

            time.sleep(0.3)

        self.last_ball = {'x': self.simspark.get_ball()['x'], 'y': self.simspark.get_ball()['y']}

        # its a simulation of a penalty kick - the ball should be touched only once; stop robot and wait for ball
        self.agent.debugrequest('gamecontroller:game_state:play', False)
        wait_for(self.ball_out_or_doesnt_move, 0.3, 0.1, 3600.0)

        robot = self.simspark.get_robot(3)
        ball = self.simspark.get_ball()
        successful_test = ball['x'] > 4.5 and abs(ball['y']) < 0.75

        # log the result
        self.logger.info('Robot is at {},{}'.format(robot['x'], robot['y']))
        self.logger.info('Ball is at {},{}'.format(ball['x'], ball['y']))
        self.logger.info('GOAL!!!!' if successful_test else 'missed ... :(')

        return successful_test

    def ball_out_or_doesnt_move(self):
        _ = self.simspark.get_ball()

        ball_moved = math.sqrt(math.pow(_['x'] - self.last_ball['x'], 2) + math.pow(_['y'] - self.last_ball['y'], 2)) > 0.0
        if _['x'] < 4.5 and abs(_['y']) < 3.0 and ball_moved:
            self.last_ball['x'] = _['x']
            self.last_ball['y'] = _['y']
            return False
        return True

    def frame_info_parser(self, cmd: DebugCommand):
        self.frame_info.ParseFromString(cmd.result())

    def behavior_logger(self, cmd: DebugCommand):
        self.parser.parse('BehaviorStateSparse', cmd.result())
        if self.parser.isActiveOption('penalty_kicker'):
            self.logger.debug('{} @ {}: {}'.format(
                self.frame_info.frameNumber,
                self.frame_info.time,
                self.parser.getActiveOptionState('penalty_kicker')
            ))
