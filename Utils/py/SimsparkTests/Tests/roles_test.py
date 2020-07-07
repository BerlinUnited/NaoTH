from SimsparkController import SimsparkController
from Utils import *

def roles_test(args):
    s = SimsparkController(args.simspark, not args.no_simspark)
    s.start()
    s.connected.wait() # wait for the monitor to be connected

    agents = []
    for n in range(1,6):
        agents.append(AgentController(args.agent, args.config, number=n, start_instance=not args.no_agent))

    logging.info('Created agent instances')

    for a in agents:
        a.start()
        a.wait_connected()

    #for a in agents: a.wait_connected() # wait for the agent to be fully started

    # it takes sometimes a while until simspark got the correct player number
    for a in agents: wait_for(lambda: s.get_robot(a.number) is not None, 0.3)
    # wait until the player is on the field
    for a in agents: wait_for(lambda: s.get_robot(a.number)['z'] <= 0.4, 0.3)

    logging.info('Enable/Disable some modules')

    # eg. for en-/disabling a module
    #a.module('VirtualVisionProcessor', False)
    for a in agents:
        a.agent('soccer_agent')
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
    logging.info('READY')
    for a in agents: a.debugrequest('gamecontroller:game_state:ready', True)

    # simulation is slow, give the robots some time to reach their kickoff position
    time.sleep(120)

    # SET
    logging.info('SET')
    for a in agents: a.debugrequest('gamecontroller:game_state:set', True)  # set gamestate to 'SET'
    for a in agents: a.debugrequest('gamecontroller:game_state:ready', False)  # ... and reset the 'READY' state

    time.sleep(5)

    # put the robot in play mode
    logging.info('PLAY')
    for a in agents: a.debugrequest('gamecontroller:game_state:play', True)  # set the gamestate to 'PLAY'
    for a in agents: a.debugrequest('gamecontroller:game_state:set', False)  # ... and reset the 'SET' state

    logging.info('wait for 30s ...')
    time.sleep(30)

    logging.info('FINISH')
    for a in agents: a.debugrequest('gamecontroller:game_state:finish', True)  # set the gamestate to 'FINISH'
    for a in agents: a.debugrequest('gamecontroller:game_state:ready', False)  # ... and make sure, everything else is reset
    for a in agents: a.debugrequest('gamecontroller:game_state:set', False)  # ... and make sure, everything else is reset
    for a in agents: a.debugrequest('gamecontroller:game_state:play', False)  # ... and make sure, everything else is reset

    for a in agents: a.stop()
    s.cancel()
    s.join()

    # return True if the test was successful
    return True