import argparse
import logging
import os
import shutil
import time

from AgentController import AgentController, Command
from SimsparkController import SimsparkController


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

    a = AgentController(args.agent, args.config, start_instance=not args.no_agent)
    a.start()
    a.started.wait() # wait for the agent to be fully startedps

    logging.info('Some Simspark commands')

    # some tests commands
    s.cmd_dropball()
    s.cmd_ballPos()
    s.cmd_agentPos(1, -1, 0)

    a.send_command(Command('Cognition:debugrequest:set',[('gamecontroller:play','on'.encode())]))

    time.sleep(4)

    a.send_command(Command('Cognition:debugrequest:set', [('gamecontroller:play', 'off'.encode())]))
    #a.send_command(Command('Cognition:representation:get', ['DebugRequest']))

    logging.info('waiting ..')
    while s.is_alive():
        time.sleep(1)

    a.cancel()
    s.cancel()

    a.join()
    s.join()
    #'''


    print('DONE')