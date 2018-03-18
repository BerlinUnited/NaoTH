#!/usr/bin/env python3
import os, sys, argparse

from PyQt5.QtWidgets import QApplication

from ConfigViewerWidget import Widget

class NonFolderError(Exception):
    '''
    Raised when we expect a folder (directory) and did not get one
    '''
    pass

class FolderExistsAction(argparse.Action):
    '''
    Custom action: verify the argument to be a folder (directory). If
    not, raise a NonFolderError exception.
    '''

    def verify_folder_existence(self, folder_name):
        '''
        Return folder_name if exists. Throw NonFolderError if not.
        '''
        if not os.path.isdir(folder_name):
            raise NonFolderError('ERROR: {0} is not a folder'.format(folder_name))

        return os.path.abspath(folder_name)

    def __call__(self, parser, namespace, values, option_string=None):
        if type(values) == list:
            folders = map(self.verify_folder_existence, values)
        else:
            folders = self.verify_folder_existence(values)

        # Add the attribute
        setattr(namespace, self.dest, folders)

def parseArguments():
    parser = argparse.ArgumentParser(description='Shows the module configuration')
    parser.add_argument('config_dir', default='../../../NaoTHSoccer/Config', nargs='?', action=FolderExistsAction, help='the config directory')

    return parser.parse_args()

if __name__ == "__main__":
    args = parseArguments()

    app = QApplication(sys.argv)
    win = Widget(args.config_dir)
    win.show()
    sys.exit(app.exec_())