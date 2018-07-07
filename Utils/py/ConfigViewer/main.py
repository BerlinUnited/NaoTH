#!/usr/bin/env python3
import glob
import os, sys, argparse
import re

from PyQt5.QtWidgets import QApplication

import Config
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

    return parser.parse_args()

def compare_dict(d1:dict, d2:dict, verbose:bool):
    # TODO: move to Config class?!
    r = {}
    k1 = set(d1.keys())
    k2 = set(d2.keys())
    ke = k1.intersection(k2)
    for k in ke:
        if type(d1[k]) == type(d2[k]):
            if isinstance(d1[k], dict):
                rs = compare_dict(d1[k], d2[k], verbose)
                if rs:
                    r[k] = rs
            elif d1[k] == d2[k]:
                pass
            else:
                r[k] = (d1[k], d2[k])
                if verbose:
                    print(k, 'values', (d1[k], d2[k]))
        else:
            if verbose:
                print(k, 'types', (d1[k], d2[k]))
            r[k] = (d1[k], d2[k])

    kd = k1.symmetric_difference(k2)
    if kd:
        if verbose:
            print('difference', kd)
        for k in kd:
            r[k] = d1[k] if k in d1 else d2[k]

    return r


if __name__ == "__main__":
    args = parseArguments()

    if args.check_team:
        if os.path.isdir(args.config_dir):
            print('check team configuration')
            regex = re.compile('\d{6}-\d{4}-(\S+)')
            configs = []
            for d in os.scandir(args.config_dir):
                if d.is_dir() and regex.fullmatch(d.name):
                    config_uri = glob.glob(d.path + '/[Cc]onfig*')
                    if config_uri:
                        configs.append(Config.readConfig(config_uri[0]))
            if len(configs) > 1:
                for i in range(len(configs)):
                    for j in range(i+1, len(configs)):
                        # TODO: determine robot if possible?!
                        c1 = configs[i].getConfigFor(platform='Nao', scheme=configs[i].getScheme())
                        c2 = configs[j].getConfigFor(platform='Nao', scheme=configs[j].getScheme())
                        r = compare_dict(c1, c2, True)

                        print('\n', configs[i].getName(), '\n', configs[j].getName(), sep='')
                        if r:
                            print('differences in ', list(r.keys()), '\n', r, sep='')
                        else:
                            print("OK")
            else:
                print('Found only 1 configuration!')
        else:
            print('The given URI isn\'t a directory!')

    else:
        app = QApplication(sys.argv)
        win = Widget(args.config_dir)
        win.show()
        sys.exit(app.exec_())