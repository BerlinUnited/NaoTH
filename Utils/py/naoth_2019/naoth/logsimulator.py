import asyncio
import logging
import os
import re
import enum

logger = logging.getLogger(__name__)


class LogSimulator:
    def __init__(self, log_file, config_folder, port=5401, executable=None):
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
        self.port = port

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
        return [self.executable, '-b', '--port', str(self.port), self.log_file]

    def working_dir(self):
        """
        :returns working directory to start the log simulator with the Config folder given at class creation
        """
        return os.path.dirname(self.config_folder)

    """
    def execute(self, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE):
        if self.executable is None:
            raise ValueError('Executable was not set!')
        self.process = subprocess.Popen(self.program_args(),
                                        cwd=self.working_dir(),
                                        stdout=stdout, stderr=stderr, stdin=stdin, encoding='utf-8')
    """

    async def create_process(self):
        if self.executable is None:
            raise ValueError('Executable was not set!')
        pipe = asyncio.subprocess.PIPE
        process = await asyncio.create_subprocess_exec(*self.program_args(), cwd=self.working_dir(),
                                                       stdout=pipe, stderr=pipe, stdin=pipe)
        return LogSimInstance(process)


class LogSimInstance:
    def __init__(self, process):
        self.process = process

        # pattern of frame info output
        self.frame_pattern = re.compile(r"""
        \[
        (?P<current>[0-9]+) # current frame number
        \|
        (?P<first>[0-9]+) # first frame number of log file
        -
        (?P<last>[0-9]+) # last frame number of log file
        ]""", re.VERBOSE)

        self.logfile_representations = []

        self.first_frame = None
        self.last_frame = None
        self.current_frame = None

    async def advance(self):
        self.process.stdin.write(b'd')
        while True:
            line = await self.process.stdout.readline()
            line = line.decode()
            match = self.frame_pattern.match(line)
            if match:
                self.current_frame = int(match.group('current'))
                self.first_frame = int(match.group('first'))
                self.last_frame = int(match.group('last'))
                return

    async def play(self):
        self.process.stdin.write(b'p')
        while True:
            line = await self.process.stdout.readline()
            line = line.decode()
            match = self.frame_pattern.match(line)
            if match:
                self.current_frame = int(match.group('current'))
                self.first_frame = int(match.group('first'))
                self.last_frame = int(match.group('last'))
                if self.current_frame == self.last_frame:
                    return

    async def boot(self):
        """
        Wait for log simulator instance to start and parse information output
        """
        # parser modes
        class MODE(enum.Enum):
            REPRESENTATIONS = enum.auto()
            MISC = enum.auto()

        current_mode = MODE.MISC
        welcome_read = False

        # read stdout
        while True:
            line = await self.process.stdout.readline()
            line = line.decode()

            # set current parser mode
            if line.startswith('Welcome'):
                welcome_read = True
                current_mode = MODE.MISC
                continue
            elif line.startswith('Representations contained in the logfile:'):
                current_mode = MODE.REPRESENTATIONS
                continue
            elif not line.strip() or re.match('---+', line):
                current_mode = MODE.MISC
                continue

            # parse output
            if current_mode == MODE.REPRESENTATIONS:
                self.logfile_representations.append(line.strip())

            # make sure this pattern comes after the welcome screen
            if welcome_read:
                match = self.frame_pattern.match(line)
                if match:
                    self.current_frame = int(match.group('current'))
                    self.first_frame = int(match.group('first'))
                    self.last_frame = int(match.group('last'))
                    # boot complete
                    break

    """
    async def read_stdout(self):
        while True:
            line = await self.process.stdout.readline()
            line = line.decode()
            if self.process.stdout.at_eof():
                return
            match = self.frame_pattern.match(line)
            if match:
                self.current_frame = int(match.group('current'))
                self.first_frame = int(match.group('first'))
                self.last_frame = int(match.group('last'))

    async def read_stderr(self):
        while True:
            line = await self.process.stderr.readline()
            line = line.decode()
            if self.process.stdout.at_eof():
                return
    """

    async def shutdown(self):
        """
        Attempting graceful shutdown of log simulator process.
        """
        # sometimes log simulator needs more than one quit call
        self.process.stdin.write(b'qqq')
        await self.process.stdout.read()
        await self.process.wait()
