#!/usr/bin/env python3
import os, sys, argparse

from PyQt5.QtWidgets import QApplication

import Utils
from ConfigViewerWidget import Widget

class ArgumentFileParser(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        if os.path.exists(values):
            if os.path.isdir(values):
                if os.access(values, os.R_OK):
                    setattr(namespace,self.dest, os.path.abspath(values))
                else:
                    raise argparse.ArgumentTypeError("Directory is not readable: {}".format(values))
            elif os.path.isfile(values):
                if os.access(values, os.R_OK):
                    setattr(namespace, self.dest, os.path.abspath(values))
                else:
                    raise argparse.ArgumentTypeError("File is not readable: {}".format(values))
            else:
                raise argparse.ArgumentTypeError("Unknown file type: {}".format(values))
        else:
            raise argparse.ArgumentTypeError("File or directory doesn't exist: {}".format(values))

def parseArguments():
    source_path = os.path.dirname(os.path.realpath(__file__))

    parser = argparse.ArgumentParser(description='Shows the module configuration')
    parser.add_argument('config_dir', default=source_path+'/../../../NaoTHSoccer/Config', nargs='?', action=ArgumentFileParser, help='the config directory')
    parser.add_argument('--check-team', action='store_true', default=False, help='checks the team configuration')
    parser.add_argument('--compare', action=ArgumentFileParser, help='the config directory')
    parser.add_argument('-v', '--verbose', action='store_true', default=False, help='Enables verbose output')

    return parser.parse_args()


if __name__ == "__main__":
    args = parseArguments()

    if args.check_team:
        Utils.compare_configs(args.config_dir, args.verbose)
    elif args.compare:
        Utils.compare_config_pair(args.compare, args.config_dir, args.verbose)
    else:
        app = QApplication(sys.argv)
        win = Widget(args.config_dir)
        win.show()
        sys.exit(app.exec_())