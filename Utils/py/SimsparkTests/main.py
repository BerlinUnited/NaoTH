import argparse
import logging
import multiprocessing
import shutil
import os

import Tests
from Utils import remove_simspark_logfile


def parseArguments():
    # retrieve default path
    naothsoccer = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../NaoTHSoccer'))

    parser = argparse.ArgumentParser(
        description='Performs some tests with simspark.\n'
                    'The exit code indicates the success or failure of the tests. If a test fails, the exit code is increased.\n'
                    'If an error occurred during the execution of a test, the exit code is increased by 100.\n'
                    'For eg., if 4 tests are scheduled, one fails and another throws an exception, the exit code is "101".',
        epilog= "Example:\n"
                "\t{0}\n"
                "\n\nNOTE:\n"
                "For 'headless' execution you have to modify the 'rcssserverspl.rb' of your simspark installation and set '$enableInternalMonitor = false'."
                "".format(os.path.basename(__file__)),
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument('-s', '--simspark', default='simspark', action='store', help="")
    parser.add_argument('-a', '--agent', default=naothsoccer+'/dist/Native/naoth-simspark', action='store', help="")
    parser.add_argument('-c', '--config', default=naothsoccer, action='store', help="")
    parser.add_argument('-v', '--verbose', action='store_true', help="")
    parser.add_argument('-t', '--test', nargs='+', action='store', help="")

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

    exit_code = 0

    if args.test:
        for name in args.test:
            if name in Tests.functions:
                try:
                    exit_code += 1 if Tests.functions[name](args) else 0
                except:
                    logging.error('An error occurred while executing test "%s"', name)
                    exit_code += 100

                still_active = multiprocessing.active_children()
                if still_active:
                    logging.warning('Still some processes running - they get killed!')
                    for p in still_active:
                        p.terminate()
                        p.join(1)
                        if p.is_alive():
                            p.kill()
            else:
                logging.warning('Unknown test: "%s"', name)

    # if one is created, it gets removed!
    remove_simspark_logfile()

    exit(exit_code)