import os
import sys

from PyQt5 import QtWidgets

from naoth.log_extractor.extractor.extract_mrl import ExtractGlobalLocalization
from naoth.log_extractor.extractor.recapture import LogRecapture
from naoth.log_extractor.log_extractor import LogExtractorWidget

if __name__ == '__main__':
    # setup logging
    import naoth.logging_config.default as log

    log.configure()

    os.environ['LOG_SIMULATOR_PATH'] = '/home/robert/Repository/NaoTH-2018/NaoTHSoccer/dist/Native/logsimulator'

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
