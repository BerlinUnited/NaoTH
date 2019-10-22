#!/usr/bin/env python3

"""Simple frontend for the naoth logsimulator, supporting drag and drop functionality for log files and config folders.

Usage:
    run_logsimulator_ui.py [-e | --executable <logsimulator_path>]

    run_logsimulator_ui.py -h | --help

Options:
    -e --executable     Path to logsimulator executable, usually "[NaoTH-Repo]/NaoTHSoccer/dist/Native/logsimulator".
                        You don't need to supply this argument, if the LOG_SIMULATOR_PATH environment variable is set
                        to the executable path.
                        The GUI also supports drag and drop of the log simulator executable.

    -h --help           Show this screen.
"""

import logging
import os
import sys
from tempfile import TemporaryDirectory
from zipfile import ZipFile

from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtGui import QTextCursor

import naoth
from naoth.logsimulator import LogSimulator

logger = logging.getLogger(__name__)


class LogSimulatorWidget(QtWidgets.QTextEdit):
    def __init__(self, parent=None, logsimulator_path=None, scroll_back=10000):
        super().__init__(parent=parent)
        self.setAcceptDrops(True)
        self.setReadOnly(True)

        # set base font size
        font = QtGui.QFont("Monospace")
        font.setStyleHint(QtGui.QFont.TypeWriter)
        font.setPointSize(12)
        self.setFont(font)

        self.scroll_back = scroll_back
        self.scroll_back_buffer = 1000

        self.append('Awaiting log file and config folder... (Drag & Drop here)')

        self.tmp_dir = None
        self.config_folder = None
        self.log_file = None

        self.logsimulator_path = logsimulator_path

        self.log_sim = None
        self.process = QtCore.QProcess(self)
        self._connect_events()

        self.awaiting_executable = False
        self.awaiting_restart = False

    def number_of_lines(self):
        """
        :returns number if displayed lines
        """
        cursor = self.textCursor()
        self.moveCursor(QTextCursor.End)
        return cursor.blockNumber()

    def trim_history(self):
        """
        Reduces history if to many lines are displayed.
        """
        line_count = self.number_of_lines()
        if line_count > self.scroll_back + self.scroll_back_buffer:
            cursor = self.textCursor()

            lines_to_delete = line_count - self.scroll_back

            cursor.movePosition(QTextCursor.Start)
            cursor.movePosition(QTextCursor.Down, QTextCursor.KeepAnchor, lines_to_delete)
            cursor.movePosition(QTextCursor.StartOfLine, QTextCursor.KeepAnchor)

            cursor.removeSelectedText()

    def _connect_events(self):
        self.process.readyRead.connect(self._output_ready)
        self.process.readyReadStandardError.connect(self._error_ready)
        self.process.finished.connect(self._process_finished)

    def _output_ready(self):
        output = bytes(self.process.readAll()).decode('utf-8')

        self.append(output)
        self.trim_history()

        # parse frame info
        """
        frame_info_match = LogSimInstance.frame_pattern.match(output)
        if frame_info_match:
            current_frame = int(frame_info_match.group('current'))
            first_frame = int(frame_info_match.group('first'))
            last_frame = int(frame_info_match.group('last'))

            print(current_frame, first_frame, last_frame)
        """

    def _error_ready(self):
        output = bytes(self.process.readAllStandardError()).decode('utf-8')
        self.append(f'<font color="DeepPink">{output}</font><br>')

        self.trim_history()

    def _process_finished(self, exit_status):
        self.stop()
        logger.info('LogSimulator was closed.')
        self.append('Press <enter> to restart log simulator or drop another log file or config folder.')
        self.awaiting_restart = True

    def keyPressEvent(self, event):
        if self.awaiting_restart and event.key() == QtCore.Qt.Key.Key_Return:
            self.awaiting_restart = False
            self.clear()
            self.start()
        elif event.text():
            # Pass key events to the log simulator
            logger.debug(f'Key {event.text()} pressed.')
            byte_array = QtCore.QByteArray()
            byte_array.append(event.text().lower())
            self.process.write(byte_array)
            event.accept()

    def dragEnterEvent(self, event):
        if event.mimeData().hasUrls():
            event.accept()
        else:
            event.ignore()

    def dragMoveEvent(self, event):
        if event.mimeData().hasUrls():
            event.setDropAction(QtCore.Qt.CopyAction)
            event.accept()
        else:
            event.ignore()

    @staticmethod
    def parse_urls(event):
        if event.mimeData().hasUrls():
            event.accept()
            for url in event.mimeData().urls():
                yield str(url.toLocalFile())
        else:
            event.ignore()

    def dropEvent(self, event):
        messages = []

        # stop previous running simulator
        if self.process.state() == QtCore.QProcess.Running:
            self.stop()

        if self.awaiting_executable:
            # get log simulator executable path from drop event
            for path in self.parse_urls(event):
                if os.path.isfile(path) and os.access(path, os.X_OK):
                    self.log_sim.executable = path
                    message = f'Registered executable "{path}".'
                    self.awaiting_executable = False
                else:
                    message = f'"{path}" is not a valid executable!'
                logger.info(message)
                messages.append(message)
        else:
            # get log file and config folder path from drop event
            for path in self.parse_urls(event):
                if os.path.isfile(path):
                    if path.endswith('.zip'):
                        # unzip config.zip
                        self.tmp_dir = TemporaryDirectory(prefix='logsim_cwd_')
                        logger.info(f'Creating temporary directory "{self.tmp_dir.name}" to unzip {path}...')
                        with ZipFile(path) as config_zip:
                            config_zip.extractall(self.tmp_dir.name)
                        self.config_folder = os.path.join(self.tmp_dir.name, 'Config')
                        message = f'Registered config folder "{self.config_folder}".'
                    else:
                        # log file
                        self.log_file = path
                        message = f'Registered log file "{self.log_file}".'
                    logger.info(message)
                elif os.path.isdir(path):
                    # config folder
                    self.config_folder = path
                    message = f'Registered config folder "{self.config_folder}".'
                    logger.info(message)
                else:
                    message = f'Unrecognized path "{path}"!'
                    logger.warning(message)
                messages.append(message)

        if self.log_file is None:
            messages.append('Awaiting log file...')
        if self.config_folder is None:
            messages.append('Awaiting config folder...')

        # start log simulator if all necessary files are available
        if self.log_file and self.config_folder:
            self.log_sim = LogSimulator(self.log_file, self.config_folder, executable=self.logsimulator_path)

            # check if executable is available
            if self.log_sim.executable:
                # start simulator
                self.clear()
                self.start()
            else:
                self.awaiting_executable = True
                message = 'LogSimulator executable was not found!'
                logger.warning(message)
                messages.append(message)
                messages.append('Drag & Drop here OR supply as argument "-e <path>" '
                                'OR set LOG_SIMULATOR_PATH environment variable!')

        # display messages
        self.clear()
        self.append('\n'.join(messages))

    def start(self):
        self.process.setWorkingDirectory(self.log_sim.working_dir())
        args = self.log_sim.program_args()
        logger.info(f'Starting log simulator with {args}...')
        if len(args) > 1:
            self.process.start(args[0], args[1:])
        else:
            self.process.start(args[0])

    def stop(self):
        logger.info('Stopping log simulator...')
        self.process.finished.disconnect()
        self.process.kill()
        self.process = QtCore.QProcess(self)
        self._connect_events()

    def exit(self):
        if self.process.state() == QtCore.QProcess.Running:
            logger.info('Ending log simulator process...')
            self.process.kill()


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, simulator_widget):
        super().__init__()

        self.simulator_widget = simulator_widget
        self.setCentralWidget(simulator_widget)
        self.resize(1000, 800)
        self.show()

    def closeEvent(self, event):
        self.simulator_widget.exit()
        event.accept()


if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(description='Simple naoth logsimulator frontend, '
                                                 'enabling drag & drop functionality for log files and config folders')

    def is_file(path):
        if not os.path.isfile(path):
            parser.error(f'Given argument "{path}" is not a valid file path, '
                         f'compile the log simulator first!')
            return
        if not os.access(path, os.X_OK):
            parser.error(f'Given argument "{path}" is not executable!')
            return
        return path

    parser.add_argument('-e', '--executable', help='path to logsimulator executable', type=is_file)
    args = parser.parse_args()

    naoth.setup_logger()

    app = QtWidgets.QApplication(sys.argv)

    simulator_ui = LogSimulatorWidget(logsimulator_path=args.executable)

    # create main window
    window = MainWindow(simulator_ui)

    sys.exit(app.exec_())
