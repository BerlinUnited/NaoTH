import logging
import os
import subprocess

logger = logging.getLogger(__name__)


class LogSimulator:
    def __init__(self, log_file, config_folder, executable=None):
        """
        Creates log simulator frontend.

        :param log_file: naoth log file
        :param config_folder: log file corresponding Config folder
        :param executable: path to logsimulator executable, if None it is read
                           from the LOG_SIMULATOR_PATH environment variable
        """
        if not os.path.isfile(log_file):
            raise ValueError(f'Given log file argument {log_file} is not a valid file path!')
        if not os.path.isdir(config_folder):
            raise ValueError(f'Given config folder argument {config_folder} is not a valid folder path!')

        self.log_file = log_file
        self.config_folder = config_folder

        self.executable = os.getenv('LOG_SIMULATOR_PATH') if executable is None else executable
        if self.executable is None:
            logger.warning('Environment variable LOG_SIMULATOR_PATH is not set.')
        elif os.path.exists(self.executable):
            if not os.path.isfile(self.executable) and not os.access(self.executable, os.X_OK):
                logger.warning(f'{self.executable} is not executable!')
        else:
            logger.warning(f'{self.executable} does not exist, you need to compile the log simulator first.')

        self.process = None

    def program_args(self):
        """
        :returns parameters necessary to start the log simulator in backend mode
                 with the log file given at class creation
        """
        return [self.executable, '-b', self.log_file]

    def working_dir(self):
        """
        :returns working directory to start the log simulator with the Config folder given at class creation
        """
        return os.path.dirname(self.config_folder)

    def execute(self, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE):
        if self.executable is None:
            raise ValueError('Executable was not set!')
        self.process = subprocess.Popen(self.program_args(),
                                        cwd=self.working_dir(),
                                        stdout=stdout, stderr=stderr, stdin=stdin, encoding='utf-8')
