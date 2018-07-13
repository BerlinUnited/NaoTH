#!/usr/bin/env python3
import os, sys, argparse

from PyQt5.QtWidgets import QApplication

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

    return parser.parse_args()

if __name__ == "__main__":
    args = parseArguments()

    app = QApplication(sys.argv)
    win = Widget(args.config_dir)
    win.show()
    sys.exit(app.exec_())