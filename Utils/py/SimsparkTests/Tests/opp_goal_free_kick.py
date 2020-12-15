import math
import time

from naoth.log._parser import BehaviorParser

from SimsparkController import SimsparkController
from AgentController import AgentController
from Utils import *


class OpponentGoalFreekick(TestRun):
    """Tests, whether the robot handles the opponent goal free kick correctly."""

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
        self.agent.debugrequest('gamecontroller:set_play:goal_free_kick', True)
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

        # the robot should forget the previous ball!
        self.agent.debugrequest('MultiKalmanBallLocator:remove_all_models', True)
        time.sleep(0.5)
        self.agent.debugrequest('MultiKalmanBallLocator:remove_all_models', False)

        # if we do this test multiple times, the simulation commands are sometimes a little bit delayed
        wait_for(lambda: self.simspark.get_ball()['x'] - ball[0] <= 0.01, 0.3)

        # put the robot in play mode
        self.agent.debugrequest('gamecontroller:game_state:play', True)

        start = time.monotonic()
        # max. 90sec for this test (NOTE: walking of the robot in the simulation is not good, therefore we wait longer)
        while time.monotonic() <= (start + 90):
            self.parser.parse('BehaviorStateSparse', self.agent.behavior().result())
            if self.parser.isActiveOption('free_kick_simple'):
                '''
                print(parser.symbols.boolean('ball.know_where_itis'),
                      parser.symbols.decimal('ball.preview.x'),
                      parser.symbols.decimal('ball.preview.y'),
                      parser.symbols.decimal('ball.x'),
                      parser.symbols.decimal('ball.y'),
                      parser.getActiveOptionState('free_kick_opp_goal')
                )
                '''
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

        return True

    def test_right_goal_pos_01(self):
        return self.position_and_execute((3.5, 2.5, 90), (3.0, -1.0))

    def test_right_goal_pos_02(self):
        return self.position_and_execute((3.5, 1.5, 90), (3.0, -1.0))

    def test_right_goal_pos_03(self):
        return self.position_and_execute((3.5, 0.5, 90), (3.0, -1.0))

    def test_right_goal_pos_04(self):
        return self.position_and_execute((3.5, -0.5, 90), (3.0, -1.0))

    def test_right_goal_pos_05(self):
        return self.position_and_execute((3.5, -1.5, 90), (3.0, -1.0))

    def test_right_goal_pos_06(self):
        return self.position_and_execute((3.5, -2.5, 90), (3.0, -1.0))

    def test_right_goal_pos_07(self):
        return self.position_and_execute((3.0, 2.5, 90), (3.0, -1.0))

    def test_right_goal_pos_08(self):
        return self.position_and_execute((3.0, 1.5, 90), (3.0, -1.0))

    def test_right_goal_pos_09(self):
        return self.position_and_execute((3.0, 0.5, 90), (3.0, -1.0))

    def test_right_goal_pos_10(self):
        return self.position_and_execute((3.0, -0.5, 90), (3.0, -1.0))

    def test_right_goal_pos_11(self):
        return self.position_and_execute((3.0, -1.5, 90), (3.0, -1.0))

    def test_right_goal_pos_12(self):
        return self.position_and_execute((3.0, -2.5, 90), (3.0, -1.0))

    def test_right_goal_pos_13(self):
        return self.position_and_execute((2.5, 2.0, -90), (3.0, -1.0))

    def test_right_goal_pos_14(self):
        return self.position_and_execute((2.5, 1.5, -90), (3.0, -1.0))

    def test_right_goal_pos_15(self):
        return self.position_and_execute((2.5, 0.5, -90), (3.0, -1.0))

    def test_right_goal_pos_16(self):
        return self.position_and_execute((2.5, -0.5, -90), (3.0, -1.0))

    def test_right_goal_pos_17(self):
        return self.position_and_execute((2.5, -1.5, -90), (3.0, -1.0))

    def test_right_goal_pos_18(self):
        return self.position_and_execute((2.5, -2.5, -90), (3.0, -1.0))

    def test_right_goal_pos_19(self):
        return self.position_and_execute((1.5, 2.0, -90), (3.0, -1.0))

    def test_right_goal_pos_20(self):
        return self.position_and_execute((1.5, 0.0, -90), (3.0, -1.0))

    def test_right_goal_pos_21(self):
        return self.position_and_execute((1.5, -2.0, -90), (3.0, -1.0))

    def test_right_goal_pos_22(self):
        return self.position_and_execute((0.5, 0.0, 90), (3.0, -1.0))

    def test_right_goal_pos_23(self):
        return self.position_and_execute((-1.5, 1.5, 90), (3.0, -1.0))

    def test_right_goal_pos_24(self):
        return self.position_and_execute((-1.5, -1.5, 90), (3.0, -1.0))

    def test_right_goal_pos_25(self):
        return self.position_and_execute((-3.5, 0.0, 90), (3.0, -1.0))

    def test_right_goal_pos_26(self):
        return self.position_and_execute((2.7, -1.0, -90), (3.0, -1.0))

    def test_right_goal_pos_27(self):
        return self.position_and_execute((2.7, -0.6, -90), (3.0, -1.0))

    def test_right_goal_pos_28(self):
        return self.position_and_execute((2.9, -0.6, -90), (3.0, -1.0))

    def test_right_goal_pos_29(self):
        return self.position_and_execute((3.2, -0.6, -90), (3.0, -1.0))

    def test_right_goal_pos_30(self):
        return self.position_and_execute((3.2, -0.9, -90), (3.0, -1.0))

    def test_right_goal_pos_31(self):
        return self.position_and_execute((3.2, -1.1, -90), (3.0, -1.0))

    def test_right_goal_pos_32(self):
        return self.position_and_execute((3.2, -1.3, -90), (3.0, -1.0))

    def test_right_goal_pos_33(self):
        return self.position_and_execute((3.0, -1.3, -90), (3.0, -1.0))

    def test_right_goal_pos_34(self):
        return self.position_and_execute((2.8, -1.3, -90), (3.0, -1.0))

    def test_right_goal_pos_35(self):
        return self.position_and_execute((2.7, -1.1, -90), (3.0, -1.0))

    def test_right_goal_pos_36(self):
        return self.position_and_execute((2.8, -1.3, -90), (3.0, -1.0))

    def test_right_goal_pos_37(self):
        return self.position_and_execute((4.0, -1.0, 90), (3.0, -1.0))

    def test_right_goal_pos_38(self):
        return self.position_and_execute((4.0, -0.8, 90), (3.0, -1.0))

    def test_right_goal_pos_39(self):
        return self.position_and_execute((4.0, -1.2, 90), (3.0, -1.0))

    def test_right_goal_pos_40(self):
        return self.position_and_execute((4.0, -2.5, 90), (3.0, -1.0))

    def test_right_goal_pos_41(self):
        return self.position_and_execute((2.7, 1.0, -90), (3.0, -1.0))

    def test_left_goal_pos_01(self):
        return self.position_and_execute((3.5, 2.5, 90), (3.0, 1.0))

    def test_left_goal_pos_02(self):
        return self.position_and_execute((3.5, 1.5, 90), (3.0, 1.0))

    def test_left_goal_pos_03(self):
        return self.position_and_execute((3.5, 0.5, 90), (3.0, 1.0))

    def test_left_goal_pos_04(self):
        return self.position_and_execute((3.5, -0.5, 90), (3.0, 1.0))

    def test_left_goal_pos_05(self):
        return self.position_and_execute((3.5, -1.5, 90), (3.0, 1.0))

    def test_left_goal_pos_06(self):
        return self.position_and_execute((3.5, -2.5, 90), (3.0, 1.0))

    def test_left_goal_pos_07(self):
        return self.position_and_execute((3.0, 2.5, 90), (3.0, 1.0))

    def test_left_goal_pos_08(self):
        return self.position_and_execute((3.0, 1.5, 90), (3.0, 1.0))

    def test_left_goal_pos_09(self):
        return self.position_and_execute((3.0, 0.5, 90), (3.0, 1.0))

    def test_left_goal_pos_10(self):
        return self.position_and_execute((3.0, -0.5, 90), (3.0, 1.0))

    def test_left_goal_pos_11(self):
        return self.position_and_execute((3.0, -1.5, 90), (3.0, 1.0))

    def test_left_goal_pos_12(self):
        return self.position_and_execute((3.0, -2.5, 90), (3.0, 1.0))

    def test_left_goal_pos_13(self):
        return self.position_and_execute((2.5, 2.0, -90), (3.0, 1.0))

    def test_left_goal_pos_14(self):
        return self.position_and_execute((2.5, 1.5, -90), (3.0, 1.0))

    def test_left_goal_pos_15(self):
        return self.position_and_execute((2.5, 0.5, -90), (3.0, 1.0))

    def test_left_goal_pos_16(self):
        return self.position_and_execute((2.5, -0.5, -90), (3.0, 1.0))

    def test_left_goal_pos_17(self):
        return self.position_and_execute((2.5, -1.5, -90), (3.0, 1.0))

    def test_left_goal_pos_18(self):
        return self.position_and_execute((2.5, -2.5, -90), (3.0, 1.0))

    def test_left_goal_pos_19(self):
        return self.position_and_execute((1.5, 2.0, -90), (3.0, 1.0))

    def test_left_goal_pos_20(self):
        return self.position_and_execute((1.5, 0.0, -90), (3.0, 1.0))

    def test_left_goal_pos_21(self):
        return self.position_and_execute((1.5, -2.0, -90), (3.0, 1.0))

    def test_left_goal_pos_22(self):
        return self.position_and_execute((0.5, 0.0, 90), (3.0, 1.0))

    def test_left_goal_pos_23(self):
        return self.position_and_execute((-1.5, 1.5, 90), (3.0, 1.0))

    def test_left_goal_pos_24(self):
        return self.position_and_execute((-1.5, -1.5, 90), (3.0, 1.0))

    def test_left_goal_pos_25(self):
        return self.position_and_execute((-3.5, 0.0, 90), (3.0, 1.0))

    def test_left_goal_pos_26(self):
        return self.position_and_execute((2.7, 1.0, -90), (3.0, 1.0))

    def test_left_goal_pos_27(self):
        return self.position_and_execute((2.7, 0.6, -90), (3.0, 1.0))

    def test_left_goal_pos_28(self):
        return self.position_and_execute((2.9, 0.6, -90), (3.0, 1.0))

    def test_left_goal_pos_29(self):
        return self.position_and_execute((3.2, 0.6, -90), (3.0, 1.0))

    def test_left_goal_pos_30(self):
        return self.position_and_execute((3.2, 0.9, -90), (3.0, 1.0))

    def test_left_goal_pos_31(self):
        return self.position_and_execute((3.2, 1.1, -90), (3.0, 1.0))

    def test_left_goal_pos_32(self):
        return self.position_and_execute((3.2, 1.3, -90), (3.0, 1.0))

    def test_left_goal_pos_33(self):
        return self.position_and_execute((3.0, 1.3, -90), (3.0, 1.0))

    def test_left_goal_pos_34(self):
        return self.position_and_execute((2.8, 1.3, -90), (3.0, 1.0))

    def test_left_goal_pos_35(self):
        return self.position_and_execute((2.8, 1.3, -90), (3.0, 1.0))

    def test_left_goal_pos_36(self):
        return self.position_and_execute((2.7, 1.1, -90), (3.0, 1.0))

    def test_left_goal_pos_37(self):
        return self.position_and_execute((2.8, 1.3, -90), (3.0, 1.0))

    def test_left_goal_pos_38(self):
        return self.position_and_execute((4.0, 1.0, 90), (3.0, 1.0))

    def test_left_goal_pos_39(self):
        return self.position_and_execute((4.0, 0.8, 90), (3.0, 1.0))

    def test_left_goal_pos_40(self):
        return self.position_and_execute((4.0, 1.2, 90), (3.0, 1.0))

    def test_left_goal_pos_41(self):
        return self.position_and_execute((4.0, 2.5, 90), (3.0, 1.0))
