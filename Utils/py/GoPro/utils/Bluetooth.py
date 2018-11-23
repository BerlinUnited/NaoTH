import os
import subprocess
import threading
from utils import Logger

# setup logger for bluetooth related logs
logger = Logger.getLogger("Bluetooth")

class Bluetooth(threading.Thread):

    def __init__(self, mac, enable_signals:bool=False):
        super().__init__()

        self.mac = mac
        self.msg = ''
        self.process = None
        self.enable_signals = enable_signals

    def run(self):
        with subprocess.Popen(
                ['gatttool', '-i', 'hci0', '-t', 'random', '-b', self.mac, '--char-write-req', '--handle', '0x002f',
                 '--value', self.msg], stdout=subprocess.PIPE, stderr=subprocess.PIPE, preexec_fn=self.__preexec_fn) as self.process:
            # retrieve output
            outs, errs = self.process.communicate()
            # log outputs
            if errs:
                logger.warning(errs.decode('utf-8').strip())
            if outs:
                logger.debug(outs.decode('utf-8').strip())

    def __preexec_fn(self):
        # NOTE: we have to use 'os.setpgrp()' to prevent signals from being propagated to the child process
        #       only the parent process should handle the signals!
        if not self.enable_signals:
            os.setpgrp()

    def cancel(self):
        if self.process is not None and self.process.poll() is None:
            self.process.terminate()

    def setWifiOn(self):
        self.msg = '03170101'
        return self


    def setWifiOff(self):
        self.msg = '03170100'
        return self


    def setBeepOn(self):
        self.msg = '03160101'
        return self


    def setBeepOff(self):
        self.msg = '03160100'
        return self
