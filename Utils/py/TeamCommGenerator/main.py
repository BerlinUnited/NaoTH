#!/usr/bin/env python3

import sys
import argparse
import signal

import utils


def parseArguments():
    parser = argparse.ArgumentParser(description='Starts a TeamCommMessage generator', epilog="Example: {} -n 10.0.4.255 -p 10004".format(sys.argv[0]))
    parser.add_argument('-n', '--net', action='store', default='localhost', help='the host / net, where the messages are send to; e.g. 10.0.4.255; default: localhost')
    parser.add_argument('-p', '--port', action='store', default=10000, type=int, help='the port number where the messages are send to; default: 10000')
    parser.add_argument('-d', '--delay', action='store', default=0.4, type=float, help='sets the time between two messages (in seconds)), default: 0.4')
    parser.add_argument('--player', action='store', type=int, help='sets the player number of the generated messages')

    return parser.parse_args()

# NOTE: doesn't work with cmd.Cmd !?
def signalHandler(sig, frm):
    global tc
    if signal.SIGINT == sig:
        tc.running = False

#
if '__main__' == __name__:
    args = parseArguments()

    signal.signal(signal.SIGINT, signalHandler)

    tc = utils.TeamCommGenerator(args.net, args.port, args.delay)  # "10.0.4.255"

    # import config and set known values
    try:
        import config
        for i in config.__dict__:
            if not i.startswith("__") and tc.hasMessageField(i):
                tc.setMessageField(i, getattr(config, i))
    except:
        print("Error importing config file! Ignoring...")

    # sets the player number, given as argument
    if args.player:
        tc.setMessageField("playerNumber", args.player)

    tc.start()

    utils.CommandParser(tc).cmdloop()

    tc.join()

    print("bye, bye!")
