#!/usr/bin/env python3
# -*- coding: utf8 -*-

import argparse
import logging
import shutil
import os

import Tests
from Utils import remove_simspark_logfile


def parseArguments():
    """
    Parses the application arguments and returns the values as Namespace.
    Errors during parsing and showing the help message is also handled here.

    :return: the Namespace with the parsed arguments
    """

    # retrieve default path
    naothsoccer = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../NaoTHSoccer'))

    parser = argparse.ArgumentParser(
        description='Performs some tests with simspark.\n'
                    'The exit code indicates the success or failure of the tests. If a test fails, the exit code is increased.\n'
                    'If an error occurred during the execution of a test, the exit code is increased by 100.\n'
                    'For eg., if 4 tests are scheduled, one fails and another throws an exception, the exit code is "101".',
        epilog= "Example:\n"
                "\t{0} -t PenaltyKicker\n\n"
                "\tExecutes the 'PenaltyKicker' test.\n\n"
                "\t{0} -v -ns -na -t PenaltyKicker\n\n"
                "\tExecutes the 'PenaltyKicker' test, but doesn't start the simspark nor the agent application. Both must be already started! Also the output is more verbose.\n\n"
                "\n\nNOTE:\n"
                "For 'headless' execution you have to modify the 'rcssserverspl.rb' of your simspark installation and set '$enableInternalMonitor = false'."
                "".format(os.path.basename(__file__)),
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument('-s', '--simspark', default='simspark', action='store', help="The path to the simspark application. By default it is searched for 'simspark' in the system path.")
    parser.add_argument('-a', '--agent', default=naothsoccer+'/dist/Native/naoth-simspark', action='store', help="The path to the simspark agent application.")
    parser.add_argument('-c', '--config', default=naothsoccer, action='store', help="The path to the agent's config.")
    parser.add_argument('-v', '--verbose', action='count', default=0, help="Set the verbosity level")
    parser.add_argument('-t', '--test', nargs='+', action='store', help="The tests which should be executed.")
    parser.add_argument('-l', '--list-tests', action='store_true', help="List all available tests and exits.")

    parser.add_argument('-ns', '--no-simspark', action='store_true', help="No simspark application is started, but there's still one required, which can be connected too.")
    parser.add_argument('-na', '--no-agent', action='store_true', help="No simspark agent application is started, but there's still one (or more) required, which can be connected too.")

    return parser.parse_args()


if __name__ == "__main__":
    args = parseArguments()

    # configure the log level
    levels = [logging.WARNING, logging.INFO, logging.DEBUG]
    level = levels[min(len(levels) - 1, args.verbose)]  # capped to number of levels
    logging.basicConfig(level=level)

    if args.list_tests:
        print('Available test cases:')
        for t in Tests.cases:
            print('    {:<30}    {}'.format(t[:30], Tests.cases[t][1]))

    if not shutil.which(args.simspark) and not args.no_simspark:
        logging.error('Can not find simspark application!')
        exit(1)

    if not shutil.which(args.agent) and not args.no_agent:
        logging.error('Can not find simspark agent application!')
        exit(1)

    if not os.path.isdir(os.path.join(args.config, 'Config')):
        logging.error('Can not find "Config" directory!')
        exit(1)

    exit_code = 0
    if args.test:
        for name in args.test:
            if name in Tests.cases:
                case = Tests.cases[name][0](args)
                try:
                    case.setUp()

                    print('Run test case {}'.format(name))
                    print('-' * 70)

                    result = case.run()

                    print('-' * 70)
                    print('{}: {} of {} test were successful'.format(name, result[1], result[0]))
                except KeyboardInterrupt:
                    logging.info('Cancelled all remaining tests')
                    exit_code += 100
                    break
                except:
                    logging.error('An error occurred while executing test "%s"', name)
                    exit_code += 100
                    # print the traceback for easier debugging
                    import traceback
                    traceback.print_exc()
                finally:
                    # stop everything in the end
                    case.tearDown()
            else:
                logging.warning('Unknown test: "%s"', name)
    else:
        logging.warning('Nothing to test.')

    # if one is created, it gets removed!
    remove_simspark_logfile()

    exit(exit_code)
