import time
from SimsparkController import SimsparkController
from AgentController import AgentController
from Utils import *

class RolesTest(TestRun):
    """Tests the walk in at the begin of a game."""

    def __init__(self, args):
        super().__init__()
        self.simspark_application = args.simspark
        self.simspark_start_instance = not args.no_simspark

        self.agent_application = args.agent
        self.agent_config_dir = args.config
        self.agent_start_instance = not args.no_agent

        self.simspark = None
        self.agents = []

        self.last_ball = {'x': 0, 'y': 0}  # helper var, in order to determine if the ball is still moving

    def setUp(self):
        self.simspark = SimsparkController(self.simspark_application, start_instance=self.simspark_start_instance)
        self.simspark.start()
        self.simspark.wait_connected()  # wait for the monitor to be connected

        for n in range(1, 6):
            self.agents.append(AgentController(self.agent_application, self.agent_config_dir, number=n, start_instance=self.agent_start_instance))

        for a in self.agents:
            a.start()
            a.wait_connected()  # wait for the agent to be fully started

        # it takes sometimes a while until simspark got the correct player number
        for a in self.agents: wait_for(lambda: self.simspark.get_robot(a.number) is not None, 0.3)
        # wait until the player is on the field
        for a in self.agents: wait_for(lambda: self.simspark.get_robot(a.number)['z'] <= 0.4, 0.3)

        # set the behavior of the agent
        for a in self.agents:
            a.agent('soccer_agent')
            a.module('PathPlanner', False)
            a.module('PathPlanner2018', True)
            a.module('SituationPriorProvider', False)
            a.module('MonteCarloSelfLocator', False)
            a.module('OdometrySelfLocator', False)
            a.module('GPS_SelfLocator', True)

    def tearDown(self):
        # stop the agent
        if self.agents:
            for a in self.agents: a.stop()
        # stop the simulation/simspark
        if self.simspark:
            self.simspark.stop()

    def test_walk_in(self):
        # check if all agents have the correct behavior set
        for a in self.agents:
            if a.agent().result().decode() != 'soccer_agent':
                return False

        self.logger.debug('Set robot to initial position')
        self.simspark.cmd_agentMove(1, -3.5, 3.2, r=-180)
        self.simspark.cmd_agentMove(2, -2.5, 3.2, r=-180)
        self.simspark.cmd_agentMove(3, -1.5, 3.2, r=-180)
        self.simspark.cmd_agentMove(4, -3.0, -3.2, r=0)
        self.simspark.cmd_agentMove(5, -2.0, -3.2, r=0)

        time.sleep(0.5)

        # READY
        self.logger.info('READY')
        for a in self.agents: a.debugrequest('gamecontroller:game_state:ready', True)

        # simulation is slow, give the robots some time to reach their kickoff position
        self.logger.info('wait for 120s ...')
        time.sleep(120)

        # SET
        self.logger.info('SET')
        for a in self.agents: a.debugrequest('gamecontroller:game_state:set', True)  # set gamestate to 'SET'
        for a in self.agents: a.debugrequest('gamecontroller:game_state:ready', False)  # ... and reset the 'READY' state

        time.sleep(5)

        # put the robot in play mode
        self.logger.info('PLAY')
        for a in self.agents: a.debugrequest('gamecontroller:game_state:play', True)  # set the gamestate to 'PLAY'
        for a in self.agents: a.debugrequest('gamecontroller:game_state:set', False)  # ... and reset the 'SET' state

        self.logger.info('wait for 30s ...')
        time.sleep(30)

        self.logger.info('FINISH')
        for a in self.agents: a.debugrequest('gamecontroller:game_state:finish', True)  # set the gamestate to 'FINISH'
        for a in self.agents: a.debugrequest('gamecontroller:game_state:ready', False)  # ... and make sure, everything else is reset
        for a in self.agents: a.debugrequest('gamecontroller:game_state:set', False)  # ... and make sure, everything else is reset
        for a in self.agents: a.debugrequest('gamecontroller:game_state:play', False)  # ... and make sure, everything else is reset

        return True
