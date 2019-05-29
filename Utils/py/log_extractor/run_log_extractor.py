import os
import sys

from PyQt5 import QtWidgets

from extractor.extract_mrl import ExtractGlobalLocalization
from extractor.recapture import LogRecapture
from log_extractor import LogExtractorWidget

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(description='Tool to apply information extractions on multiple log files')
    parser.add_argument('-e', '--executable', help='path to log simulator executable')
    args = parser.parse_args()

    if args.executable is not None:
        os.environ['LOG_SIMULATOR_PATH'] = args.executable

    # setup logging
    import naoth.logging_config.default as log

    log.configure()

    app = QtWidgets.QApplication(sys.argv)

    log_filter = LogExtractorWidget()
    log_filter.register_extractor(ExtractGlobalLocalization())
    log_filter.register_extractor(LogRecapture())

    # create main window
    window = QtWidgets.QMainWindow()
    window.setWindowTitle('NaoTH - Log File Extractor')
    window.setCentralWidget(log_filter)
    window.resize(1000, 800)
    window.show()

    sys.exit(app.exec_())
