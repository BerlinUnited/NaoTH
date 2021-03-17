import math
import time

from naoth.log._parser import BehaviorParser

from SimsparkController import SimsparkController
from AgentController import AgentController
from Utils import *


class OpponentPushingFreekick(TestRun):
    """Tests, whether the robot handles the opponent pushing free kick correctly."""

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

    def setUp(self):
        self.simspark = SimsparkController(self.simspark_application, start_instance=self.simspark_start_instance)
        self.simspark.start()
        self.simspark.wait_connected()  # wait for the monitor to be connected

        self.agent = AgentController(self.agent_application,
                                     self.agent_config_dir,
                                     number=3, start_instance=self.agent_start_instance)
        self.agent.start()
        self.logger.info('Wait for agent connection')
        self.agent.wait_connected()  # wait for the agent to be fully started

        # in order to use the parser, we have to retrieve the complete behavior first
        self.parser.init(self.agent.behavior(True).result())

        # it takes sometimes a while until simspark got the correct player number
        wait_for(lambda: self.simspark.get_robot(self.agent.number) is not None, 0.3)
        # wait until the player is on the field
        wait_for(lambda: self.simspark.get_robot(self.agent.number)['z'] <= 0.3, 0.3, max_time=5.0)

        # set the current soccer behavior
        self.agent.agent('soccer_agent')
        # dis-/enable some modules for the simspark simulation
        self.agent.module('PathPlanner', False)
        self.agent.module('PathPlanner2018', True)
        self.agent.module('SituationPriorProvider', False)
        self.agent.module('MonteCarloSelfLocator', False)
        self.agent.module('OdometrySelfLocator', False)
        self.agent.module('GPS_SelfLocator', True)

        # prepare the test
        self.simspark.cmd_dropball()  # put the ball into the game
        self.agent.debugrequest('gamecontroller:secondaryTime:30', True)
        self.agent.debugrequest('gamecontroller:set_play:pushing_free_kick', True)
        self.agent.debugrequest('gamecontroller:kickoff:own', False)
        self.agent.debugrequest('gamecontroller:kickoff:opp', True)
        # disable dynamic role decision
        self.agent.module('RoleDecisionAssignmentDistance', False)
        self.agent.module('RoleDecisionAssignmentStatic', True)

    def tearDown(self):
        # stop the agent
        if self.agent:
            self.agent.stop()
        # stop the simulation/simspark
        if self.simspark:
            self.simspark.stop()

    def position_and_execute(self, pos, ball):
        # check if the correct behavior is set
        if self.agent.agent().result().decode() != 'soccer_agent':
            return False

        # first place the robot! If we run this test multiple times, the robot could stand on the penalty mark and if
        # the ball gets placed first, it would 'jump' away.
        self.simspark.cmd_agentMove(self.agent.number, pos[0], pos[1], r=pos[2])  # place the robot
        self.simspark.cmd_ballPos(ball[0], ball[1])  # place the ball

        # if we do this test multiple times, the simulation commands are sometimes a little bit delayed
        wait_for(lambda: self.simspark.get_ball()['x'] - ball[0] <= 0.01, 0.3)

        # put the robot in play mode
        self.agent.debugrequest('gamecontroller:game_state:play', True)

        start = time.monotonic()
        # max. 90sec for this test (NOTE: walking of the robot in the simulation is not good, therefore we wait longer)
        while time.monotonic() <= (start + 90):
            self.parser.parse('BehaviorStateSparse', self.agent.behavior().result())
            if self.parser.isActiveOption('free_kick_simple'):
                if self.parser.getActiveOptionState('free_kick_simple') == 'opp_free_kick_watch_ball' and time.monotonic() > (start + 10):
                    break

        # stop the robot
        self.agent.debugrequest('gamecontroller:game_state:play', False).result()
        # wait until the robot sits (hip is below 0.2)!
        wait_for(lambda: self.simspark.get_robot(self.agent.number)['z'] <= 0.2, 0.3, max_time=5.0)

        return self.evaluate_position(ball)

    def evaluate_position(self, ball):
        # first check if the ball was moved/touched
        s_ball = self.simspark.get_ball()
        if abs(s_ball['x'] - ball[0]) + abs(s_ball['y']-ball[1]) > 0.1:
            self.logger.error('Ball touched!')
            return False

        robot = self.simspark.get_robot(self.agent.number)

        # check if the robot is away from the ball (min. 75cm)
        if math.sqrt(math.pow(ball[0] - robot['x'], 2) + math.pow(ball[1] - robot['y'], 2)) < 0.75:
            self.logger.error("Robot is to close to the ball!")
            return False

        # TODO: with a more sophisticated behavior we could also check for some actions to counter the opp free kick
        #       eg. blocking a direct kick

        return True

    def test_left_far_1(self):
        return self.position_and_execute((-2,  1.5, -90), (-1.5, 1.5))

    def test_left_far_2(self):
        return self.position_and_execute((-2,  1.9, -90), (-1.5, 1.5))

    def test_left_far_3(self):
        return self.position_and_execute((-1.5, 2, -180), (-1.5, 1.5))

    def test_left_far_4(self):
        return self.position_and_execute((-1.2, 1.8, 140), (-1.5, 1.5))

    def test_left_far_5(self):
        return self.position_and_execute((-1.2, 1.15, 40), (-1.5, 1.5))

    def test_left_far_6(self):
        return self.position_and_execute((-1.2, 1.5, 90), (-1.5, 1.5))

    def test_left_close_1(self):
        return self.position_and_execute((-2, -1.5, -90), (-1.5, -1.5))

    def test_left_close_2(self):
        return self.position_and_execute((-2, -2, -90), (-1.5, -1.5))

    def test_left_close_3(self):
        return self.position_and_execute((-1.5, -2, 0), (-1.5, -1.5))

    def test_left_close_4(self):
        return self.position_and_execute((-1.2, -1.8, 45), (-1.5, -1.5))

    def test_left_close_5(self):
        return self.position_and_execute((-1.2, -1.15, 125), (-1.5, -1.5))

    def test_left_close_6(self):
        return self.position_and_execute((-1.2, -1.5, 90), (-1.5, -1.5))

    def test_right_far_1(self):
        return self.position_and_execute((-2, -1.5, -90), (-1.5, -1.5))

    def test_right_far_2(self):
        return self.position_and_execute((-2, -2, -90), (-1.5, -1.5))

    def test_right_far_3(self):
        return self.position_and_execute((-1.5, -2, 0), (-1.5, -1.5))

    def test_right_far_4(self):
        return self.position_and_execute((-1.2, -1.8, 45), (-1.5, -1.5))

    def test_right_far_5(self):
        return self.position_and_execute((-1.2, -1.15, 125), (-1.5, -1.5))

    def test_right_far_6(self):
        return self.position_and_execute((-1.2, -1.5, 90), (-1.5, -1.5))

    def test_right_close_1(self):
        return self.position_and_execute((-4, -1.5, -90), (-3.5, -1.5))

    def test_right_close_2(self):
        return self.position_and_execute((-4, -2, -90), (-3.5, -1.5))

    def test_right_close_3(self):
        return self.position_and_execute((-3.5, -2, 0), (-3.5, -1.5))

    def test_right_close_4(self):
        return self.position_and_execute((-3.2, -1.8, 60), (-3.5, -1.5))

    def test_right_close_5(self):
        return self.position_and_execute((-3.2, -1.15, 135), (-3.5, -1.5))

    def test_right_close_6(self):
        return self.position_and_execute((-3.2, -1.5, 90), (-3.5, -1.5))
