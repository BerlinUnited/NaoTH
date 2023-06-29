#! /usr/bin/env python3
import sys, os, time, logging
import argparse
import shutil
import sqlite3
import configparser
import signal
import textwrap
import multiprocessing
from typing import Optional

import naoth
from AgentController import AgentController
from SimsparkController import SimsparkController
from Utils import *

def parseArguments():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=textwrap.dedent("""\
        Starts a full simspark game 5vs5 for two halfs and logs some infos.

        The simspark configuration isn't modified and therefore used the same configuration as if simspark would be
        started by hand. In order to start simspark without monitor (for example), you have to modify the `rcssserverspl.rb`
        file and set `$enableInternalMonitor = false`.

        By default the current working directory is used to start the agent applications. If you want to start a 
        different simspark agent or with a different config, you must set this via arguments or config file.

        You can also create a config file for this script with default values by using the `--write-config` argument.
        In order to pre-set some config values use `--write-config` with other optional arguments.

        In order to log some data about the game, you have to specify the log name `--log my_log.db`. Currently only
        SQLite logs are avilable and files ending with .db are recognized and used. Otherwise some infos are only
        printed to stdout.

        Sending HUP signal to a running simulation prints the current run and score of the simulation.
        """),
        epilog=textwrap.dedent("""\
        Examples:
        \t\033[1mgame.py\033[0m
        \tSimply starts a game 5vs5 and uses the current directory for the agent config
        
        \t\033[1mgame.py -r 100 -a -c "Example test games" -l "results.db"\033[0m
        \tStarts 100 games in a row with alternating sides and logs some game infos and the comment to the 'result.db' database
        
        \t\033[1mgame.py -lc "/path/to/left/config" -rc "/path/to/right/config"\033[0m
        \tStarts a game with the same agent executable, but different configurations for the left and right team
        
        \t\033[1mgame.py -le "/path/to/left/naoth-simspark" -re "/path/to/right/naoth-simspark"\033[0m
        \tStarts a game with the different agent executables, but same configuration for the left and right team
        
        \t\033[1mgame.py -sp 3222 -ap 3111\033[0m
        \tStarts a game with different server and agent ports than the default
        \tThis is useful if you want to start multiple instances if this script at once, since each instance blocks the used ports
        
        \t\033[1mgame.py --write-config "my-config.cfg"\033[0m
        \tWrites the default config to the given file `my-config.cfg`
        
        \t\033[1mgame.py -r 100 -a -l "results.db" --write-config "my-config.cfg"\033[0m
        \tWrites the default config with 100 runs, alternating sides and the log file to the given file `my-config.cfg`
        
        \t\033[1mgame.py --config "my-config.cfg"\033[0m
        \tLoads the configuration from the given file and starts one (or more) games
        """)
    )

    if '--config' in sys.argv:
        parser.add_argument('--config', type=ArgsValidator.config, action='store',
                            help="Use a configuration file instead of the application arguments")
        parser.add_argument('-v', '--verbose', action='store_true', help="Print more debug informations to stdout.")
    elif len(sys.argv) <= 2 and '--write-config' in sys.argv:
        parser.add_argument('--write-config', action='store',
                            help="Writes the default config to the given file and exits; additional arguments are used instead of the default")
    else:
        parser.add_argument('-r', '--runs', type=int, default=1, action='store',
                            help="The number of runs (games), which should be performed")
        parser.add_argument('-l', '--log', type=ArgsValidator.is_log, action='store',
                            help="If some game results should be logged, set the log file where the results should be written to")
        parser.add_argument('-c', '--comment', default='', action='store',
                            help="A optional comment to describe this runs.")
        parser.add_argument('-a', '--alternate', action='store_true',
                            help="If used, the sides for the teams a switched for each run (default: false)")
        parser.add_argument('--no-sync', action='store_true',
                            help="Whether the simulation should not run synchronized (default is synchronized)")

        pause = parser.add_argument_group('Pause times', 'The time in seconds which should be waited after/before a game part. '
                                                         'If 0 (zero) is set, the game is paused until enter/return is pressed.')
        pause.add_argument('-bg', '--before-game', type=int, default=1, action='store',
                            help="The time before the game is started.")
        pause.add_argument('-ht', '--half-time', type=int, default=1, action='store',
                            help="The half time.")
        pause.add_argument('-ag', '--after-game', type=int, default=1, action='store',
                            help="The time after the game is finished.")

        simspark = parser.add_argument_group('SimSpark', 'The config for the simulation.')
        simspark.add_argument('-s', '--simspark', type=ArgsValidator.is_simspark, default='simspark', action='store',
                            help="The simspark executable (default: 'simspark')")
        simspark.add_argument('-ap', '--agent-port', type=int, action='store',
                            help="The port for agents to connect to simspark (default: 3100)")
        simspark.add_argument('-sp', '--server-port', type=int, action='store',
                            help="The port for monitors to connect to simspark (default: 3200)")

        left = parser.add_argument_group('Left Team', 'The config of the left team.')
        left.add_argument('-lc', '--left-config', type=ArgsValidator.is_config_dir, default='.', action='store',
                            help="The config directory for the left team (default: './')")
        left.add_argument('-le', '--left-exe', type=ArgsValidator.is_exe, default='./dist/Native/naoth-simspark', action='store',
                            help="The agent executable for the left team")
        left.add_argument('-lp', '--left-players', type=ArgsValidator.is_positive, default=5, action='store',
                            help="The the number of players for each team (default: 5)")
        left.add_argument('-ln', '--left-name', type=str, default='NaoTH', action='store',
                            help="The name of the left Team (default: 'NaoTH')")

        right = parser.add_argument_group('Right Team', 'The config of the right team.')
        right.add_argument('-rc', '--right-config', type=ArgsValidator.is_config_dir, default='.', action='store',
                            help="The config directory for the left team (default: './')")
        right.add_argument('-re', '--right-exe', type=ArgsValidator.is_exe, default='./dist/Native/naoth-simspark', action='store',
                            help="The agent executable for the right team (default: './dist/Native/naoth-simspark')")
        right.add_argument('-rp', '--right-players', type=ArgsValidator.is_positive, default=5, action='store',
                            help="The the number of players for each team (default: 5)")
        right.add_argument('-rn', '--right-name', type=str, default='BU', action='store',
                            help="The name of the right Team (default: 'BU')")

        parser.add_argument('--config', type=ArgsValidator.config, action='store',
                            help="Use a configuration file instead of the application arguments")
        parser.add_argument('--write-config', action='store',
                            help="Writes the given config to the file and exits; additional arguments are used instead of the default")
        parser.add_argument('-v', '--verbose', action='store_true', help="Print more debug informations to stdout.")

    return parser.parse_args()


class ArgsValidator:
    @staticmethod
    def is_positive(value):
        ivalue = int(value)
        if ivalue <= 0:
            raise argparse.ArgumentTypeError("%s is an invalid positive int value" % value)
        return ivalue

    @staticmethod
    def is_simspark(value):
        if not shutil.which(value):
            raise argparse.ArgumentTypeError("Can not find simspark application (%s)!" % value)
        return value

    @staticmethod
    def is_exe(value):
        if not shutil.which(value) and not shutil.which(value, path='./'):
            raise argparse.ArgumentTypeError("Can not find simspark agent application (%s)!" % value)
        return value

    @staticmethod
    def is_config_dir(value):
        if not os.path.isdir(os.path.join(value, 'Config')):
            raise argparse.ArgumentTypeError("Can not find config directory (%s)!" % value)
        return value

    @staticmethod
    def is_log(value: str):
        if not value.endswith('db'):
            raise argparse.ArgumentTypeError("Not a valid log target (%s)!" % value)

        if not os.path.exists(os.path.dirname(value)):
            raise argparse.ArgumentTypeError("Path to log target doesn't exist (%s)!" % value)

        return value

    @staticmethod
    def config(value):
        if not os.path.isfile(value):
            raise argparse.ArgumentTypeError("Configuration file doesn't exists (%s)!" % value)
        return value


class Config:
    class Team:
        def __init__(self, name: str):
            self._config_dir = '.'
            self._executable = 'naoth-simspark'
            self._name = name
            self._players = 5

        def validate(self):
            if not self.name:
                raise Exception('A "name" for the team must be set!')
            if not os.path.isdir(os.path.join(self.config_dir, 'Config')):
                raise Exception('Invalid config directory for {}!'.format(self._name))
            if not Config._validate_exe(self.executable):
                raise Exception('Can not find or execute agent application for {}!'.format(self._name))
            if not (self.players > 0):
                raise Exception('"players" of {} must be greater zero!'.format(self._name))

        def to_dict(self):
            return {'config': self._config_dir, 'exe': self._executable, 'name': self._name, 'players':  self._players}

        @property
        def config_dir(self): return self._config_dir

        @property
        def executable(self): return self._executable

        @property
        def name(self): return self._name

        @property
        def players(self): return self._players

    def __init__(self, conf):
        self._runs = 0
        self._sync = True
        self._log = ''
        self._comment = ''
        self._alternate = False
        self._pause = { 'beforeGame': 1, 'halfTime': 1, 'afterGame': 1 }
        self._simspark = {'exe': 'simspark', 'agent': None, 'server': None}
        self._left = Config.Team('NaoTH')
        self._right = Config.Team('BU')

        if isinstance(conf, str):
            self._parse_from_config(conf)
        else:
            self._parse_from_args(conf)

        self._validate()

    @staticmethod
    def _retrieve_path(base, path):
        if path:
            return path if os.path.isabs(path) else os.path.abspath(os.path.join(base, path))
        return ''

    def _parse_from_config(self, config: str):
        c = configparser.ConfigParser()
        c.read(config)
        base = os.path.dirname(config)

        self._runs = c.getint('general', 'RUNS')
        self._sync = c.getboolean('general', 'SYNC')
        self._log = self._retrieve_path(base, c.get('general', 'LOG', fallback=''))
        self._comment = c.get('general', 'COMMENT', fallback=None)
        self._alternate = c.getboolean('general', 'ALTERNATE')
        self._pause['beforeGame'] = c.getint('pause', 'beforeGame')
        self._pause['halfTime'] = c.getint('pause', 'halfTime')
        self._pause['afterGame'] = c.getint('pause', 'afterGame')
        self._simspark['exe'] = c.get('simspark', 'exe')
        self._simspark['agent'] = int(c.get('simspark', 'agent')) if c.get('simspark', 'agent') else None
        self._simspark['server'] = int(c.get('simspark', 'server')) if c.get('simspark', 'server') else None
        self._left._config_dir = self._retrieve_path(base, c.get('left', 'CONFIG'))
        self._left._executable = self._retrieve_path(base, c.get('left', 'EXE'))
        self._left._name = c.get('left', 'NAME')
        self._left._players = c.getint('left', 'PLAYERS')
        self._right._config_dir = self._retrieve_path(base, c.get('right', 'CONFIG'))
        self._right._executable = self._retrieve_path(base, c.get('right', 'EXE'))
        self._right._name = c.get('right', 'NAME')
        self._right._players = c.getint('right', 'PLAYERS')

    def _parse_from_args(self, args):
        base = os.path.curdir

        if 'runs' in args:
            self._runs = args.runs
        if 'no_sync' in args:
            self._sync = not args.no_sync
        if 'log' in args and args.log:
            self._log = self._retrieve_path(base, args.log)
        if 'comment' in args:
            self._comment = args.comment
        if 'alternate' in args:
            self._alternate = args.alternate
        if 'before_game' in args:
            self._pause['beforeGame'] = args.before_game
        if 'half_time' in args:
            self._pause['halfTime'] = args.half_time
        if 'after_game' in args:
            self._pause['afterGame'] = args.after_game
        if 'simspark' in args:
            self._simspark['exe'] = args.simspark
        if 'agent_port' in args:
            self._simspark['agent'] = args.agent_port
        if 'server_port' in args:
            self._simspark['server'] = args.server_port
        if 'left_config' in args:
            self._left._config_dir = self._retrieve_path(base, args.left_config)
        if 'left_exe' in args:
            self._left._executable = self._retrieve_path(base, args.left_exe)
        if 'left_name' in args:
            self._left._name = args.left_name
        if 'left_players' in args:
            self._left._players = args.left_players
        if 'right_config' in args:
            self._right._config_dir = self._retrieve_path(base, args.right_config)
        if 'right_exe' in args:
            self._right._executable = self._retrieve_path(base, args.right_exe)
        if 'right_name' in args:
            self._right._name = args.right_name
        if 'right_players' in args:
            self._right._players = args.right_players

    def _validate(self):
        if not self.runs >= 0:
            raise Exception('"runs" must be greater equal zero: {}'.format(self.runs))
        if not isinstance(self.sync, bool):
            raise Exception('"sync" must be a boolean: {}'.format(self.sync))
        if not isinstance(self.log, str):
            raise Exception('"log" must be a string: {}'.format(self.log))
        if not isinstance(self.comment, str):
            raise Exception('"comment" must be a string: {}'.format(self.comment))
        if not isinstance(self.alternate, bool):
            raise Exception('"alternate" must be a boolean: {}'.format(self.alternate))
        if not self._pause['beforeGame'] >= 0:
            raise Exception('"beforeGame" must be greater equal zero: {}'.format(self.runs))
        if not self._pause['halfTime'] >= 0:
            raise Exception('"halfTime" must be greater equal zero: {}'.format(self.runs))
        if not self._pause['afterGame'] >= 0:
            raise Exception('"afterGame" must be greater equal zero: {}'.format(self.runs))
        if not self._validate_exe(self.simspark_exe):
            raise Exception('Can not find or execute simspark application: {}'.format(self.simspark_exe))
        if not (self.simspark_agent_port is None or self.simspark_agent_port > 0):
            raise Exception('Invalid simspark agent port: {}'.format(self.simspark_agent_port))
        if not (self.simspark_server_port is None or self.simspark_server_port > 0):
            raise Exception('Invalid simspark server port: {}'.format(self.simspark_server_port))

        self.team_left.validate()
        self.team_right.validate()

    @staticmethod
    def _validate_exe(exe):
        return shutil.which(exe) or shutil.which(exe, path='./') or (os.path.isfile(exe) and os.access(exe, os.X_OK))

    def write(self, file: str):
        def to_str(v): return str(v) if v is not None else ''
        cp = configparser.ConfigParser()
        config = self.to_dict()
        for c in config:
            if isinstance(config[c], dict):
                cp.add_section(c)
                for sc in config[c]:
                    cp.set(c, sc, to_str(config[c][sc]))
            else:
                if not cp.has_section('general'):
                    cp.add_section('general')
                cp.set('general', c, to_str(config[c]))
            # Writing our configuration file to 'example.cfg'
            with open(file, 'w') as configfile:
                cp.write(configfile)

    def to_dict(self):
        return {
            'runs': self._runs,
            'sync': self._sync,
            'log': self._log,
            'comment': self._comment,
            'alternate': self._alternate,
            'pause': self._pause,
            'simspark': self._simspark,
            'left': self._left.to_dict(),
            'right': self._right.to_dict(),
        }

    def __repr__(self):
        return str(self.to_dict())

    def __str__(self):
        config = self.to_dict()
        result = "Configuration:\n"
        for c in config:

            if isinstance(config[c], dict):
                result += "\t{}:\n".format(c)
                for sc in config[c]:
                    result += "\t\t{}: {}\n".format(sc, config[c][sc])
            else:
                result += "\t{}: {}\n".format(c, config[c])

        return result

    @property
    def runs(self) -> int: return self._runs

    @property
    def sync(self) -> bool: return self._sync

    @property
    def log(self) -> str: return self._log

    @property
    def comment(self) -> str: return self._comment

    @property
    def alternate(self) -> bool: return self._alternate

    @property
    def pauseBeforeGame(self) -> int: return self._pause['beforeGame']

    @property
    def pauseHalfTime(self) -> int: return self._pause['halfTime']

    @property
    def pauseAfterGame(self) -> int: return self._pause['afterGame']

    @property
    def simspark_exe(self) -> str: return self._simspark['exe']

    @property
    def simspark_agent_port(self) -> Optional[int]: return self._simspark['agent']

    @property
    def simspark_server_port(self) -> Optional[int]: return self._simspark['server']

    @property
    def team_left(self) -> Team: return self._left

    @property
    def team_right(self) -> Team: return self._right


class Log:
    def new_run(self, config: Config):
        pass

    def log_ball(self, run, time, ball):
        pass

    def log_messages(self, run, time, message_cnt):
        pass

    def log_player(self, run, time, players):
        pass

    def half_result(self, run, left, right, half, score_left, score_right):
        pass

    def finish(self):
        pass


class LogDb(Log):
    def __init__(self, db):
        self.__db = sqlite3.connect(db)
        self.__db.execute("""
            CREATE TABLE IF NOT EXISTS runs (id INTEGER PRIMARY KEY AUTOINCREMENT, 
                                             timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL, 
                                             num_runs INTEGER NOT NULL, 
                                             sync BOOLEAN NOT NULL,  
                                             alternate BOOLEAN NOT NULL,  
                                             comment text)
        """)
        self.__db.execute("""
            CREATE TABLE IF NOT EXISTS games (id INTEGER PRIMARY KEY AUTOINCREMENT, 
                                              rid INTEGER NOT NULL, 
                                              timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL, 
                                              run INTEGER NOT NULL, 
                                              team_left VARCHAR(10) NOT NULL, 
                                              team_right VARCHAR(10) NOT NULL,
                                              players_left TINYINT NOT NULL, 
                                              players_right TINYINT NOT NULL, 
                                              half TINYINT NOT NULL, 
                                              score_left SMALLINT NOT NULL, 
                                              score_right SMALLINT NOT NULL, 
                                              FOREIGN KEY (rid) REFERENCES runs (id) ON DELETE CASCADE ON UPDATE NO ACTION)
        """)
        self.__db.execute("""
            CREATE TABLE IF NOT EXISTS positions (rid INTEGER NOT NULL,
                                                  run INTEGER NOT NULL,
                                                  game_time FLOAT NOT NULL,
                                                  type VARCHAR(5) NOT NULL,
                                                  team VARCHAR(10),
                                                  player SMALLINT, 
                                                  x FLOAT NOT NULL, 
                                                  y FLOAT NOT NULL, 
                                                  r FLOAT, 
                                                  PRIMARY KEY (rid, run, game_time, type, team, player), 
                                                  FOREIGN KEY (rid) REFERENCES runs (id) ON DELETE CASCADE ON UPDATE NO ACTION)
        """)
        self.__db.execute("""CREATE VIEW IF NOT EXISTS "game_results" AS 
                                SELECT 
                                    h1.rid, h1.run, h1.team_left, h1.team_right,
                                    h1.timestamp as half1_time, h1.score_left as half1_score_left, h1.score_right as half1_score_right, 
                                    h2.timestamp as half2_time, h2.score_left as half2_score_left, h2.score_right as half2_score_right,
                                    (h1.score_left + h2.score_left) as total_left, (h1.score_right + h2.score_right) as total_right
                                FROM games h1 
                                LEFT OUTER JOIN games h2 
                                ON h1.rid = h2.rid AND h1.run = h2.run AND h1.half = 1 AND h2.half = 2 
                                WHERE h2.id IS NOT NULL
        """)
        self.__cursor = None
        self.__rid = None

    def new_run(self, config: Config):
        self.__cursor = self.__db.cursor()

        self.__cursor.execute("INSERT INTO runs (num_runs, sync, alternate, comment) VALUES (?,?,?,?)",
                              [config.runs, config.sync, config.alternate, config.comment])
        self.__db.commit()
        self.__rid = self.__cursor.lastrowid

    def log_ball(self, run, time, ball):
        try:
            self.__cursor.execute("INSERT INTO positions (rid, run, game_time, type, x, y) VALUES (?,?,?,?,?,?)", [self.__rid, run, time, 'ball', ball['x'], ball['y']])
        except Exception as e:
            logging.warning('Exception while logging ball position: %s\n%s', e, str([self.__rid, run, time, 'ball', ball['x'], ball['y']]))

    def log_player(self, run, time, players):
        for p in players:
            try:
                self.__cursor.execute("INSERT INTO positions (rid, run, game_time, type, team, player, x, y, r) VALUES (?,?,?,?,?,?,?,?,?)", [self.__rid, run, time, 'player', p['team'], p['number'], p['x'], p['y'], p['r']])
            except Exception as e:
                logging.warning('Exception while logging player position: %s\n%s', e, str([self.__rid, run, time, 'player', p['team'], p['number'], p['x'], p['y'], p['r']]))

    def half_result(self, run, left: Config.Team, right: Config.Team, half, score_left, score_right):
        try:
            self.__cursor.execute("""
                INSERT INTO games 
                    (rid, run, team_left, team_right, players_left, players_right, half, score_left, score_right) 
                    VALUES (?,?,?,?,?,?,?,?,?)""",
                [self.__rid, run, left.name, right.name, left.players, right.players, half, score_left, score_right])
        except Exception as e:
            logging.warning('Exception while logging half result: %s\n%s', e, str([self.__rid, run, left, right, half, score_left, score_right]))
        self.__db.commit()

    def finish(self):
        self.__cursor.close()
        self.__db.commit()
        self.__db.close()

        self.__cursor = None
        self.__rid = None
        self.__db = None


class LogStd(Log):
    def new_run(self, config: Config):
        print('Start a new run')
        print(config)

    def log_ball(self, run, time, ball):
        print("Run: {}\tTime: {}\tBall: {}, {}".format(run, time, ball['x'], ball['y']))

    def log_messages(self, run, time, message_cnt):
        print("Run: {}\tTime: {}\tLeft: {}, Right: {}".format(run, time, message_cnt[0], message_cnt[1]))

    def log_player(self, run, time, players):
        players_left = []
        players_right = []

        for p in players:
            if p['team'] == 'Left': players_left.insert(p['number'], "({}) {:.2f},{:.2f}".format(p['number'], p['x'], p['y']))
            elif p['team'] == 'Right': players_right.insert(p['number'], "({}) {:.2f},{:.2f}".format(p['number'], p['x'], p['y']))
            else: print('Unknown Team?!')
        print("Run: {}\tTime: {}\n\tLeft:\t{}\n\tRight:\t{}".format(run, time, "\t".join(players_left), "\t".join(players_right)))

    def half_result(self, run, left: Config.Team, right: Config.Team, half, score_left, score_right):
        print('=' * 100)
        print("Run: {}\tHalf: {}\tScore: {} {} - {} {}".format(run, half, left.name, score_left, right.name, score_right))
        print('=' * 100)

    def finish(self):
        print('FINISH!')


def createLog(log: str):
    if log.endswith('db'):
        return LogDb(log)
    return LogStd()


def prepare_game(s):
    # Left team
    s.cmd_agentMove(1, -3.5, 3.2, r=-180, t='Left')
    s.cmd_agentMove(2, -2.5, 3.2, r=-180, t='Left')
    s.cmd_agentMove(3, -1.5, 3.2, r=-180, t='Left')
    s.cmd_agentMove(4, -3.0, -3.2, r=0, t='Left')
    s.cmd_agentMove(5, -2.0, -3.2, r=0, t='Left')
    # Right team
    s.cmd_agentMove(1, 3.5, -3.2, r=0, t='Right')
    s.cmd_agentMove(2, 2.5, -3.2, r=0, t='Right')
    s.cmd_agentMove(3, 1.5, -3.2, r=0, t='Right')
    s.cmd_agentMove(4, 3.0, 3.2, r=-180, t='Right')
    s.cmd_agentMove(5, 2.0, 3.2, r=-180, t='Right')


def wait_half(r, s, half_time, log:Log=None, i:multiprocessing.Event=None):
    t = s.get_time()
    while (s.is_alive() and s.is_connected()) \
            and (t is None or t < half_time) \
            and ((i is not None and not i.is_set()) or i is None):
        # wait for the half to end
        time.sleep(1)
        _ = s.get_time()
        # make sure we got a new time frame to log
        if log and t != _:
            log.log_ball(r, t, s.get_ball())
            log.log_messages(r, t, s.get_messageCount())
            log.log_player(r, t, s.get_robots())
        t = _

def wait_pause(pause: int):
    if pause == 0:
        input('Press enter/return to continue!')
    else:
        time.sleep(pause)

def notify(signum, frame):
    if s.is_connected():
        print('Currently running game simulation #', r, '@', s.get_time(), ',', s.get_team('Left'), s.get_score('Left'), ':', s.get_score('Right'), s.get_team('Right'))
    else:
        print('WARNING: not connected to simulation!')


def interrupt(signum, frame):
    logging.info("Interrupted at {}".format(time.asctime()))
    if interrupt:
        interrupt.set()


def stop(s, agents):
    # stop agents first
    for t, a in agents: a.stop()
    # stop simulation
    s.stop()


if __name__ == "__main__":
    args = parseArguments()
    config = Config(args.config) if 'config' in args and args.config else Config(args)

    if 'write_config' in args and args.write_config:
        config.write(args.write_config)
        exit(0)

    if args.verbose: logging.basicConfig(level=logging.DEBUG)

    # TODO: configures simspark

    logging.info("Started at {}".format(time.asctime()))

    log = createLog(config.log)
    log.new_run(config)

    signal.signal(signal.SIGHUP, notify)
    signal.signal(signal.SIGINT, interrupt)

    # flag, if the process got interrupted and everything should be shutdown
    interrupt = multiprocessing.Event()

    left = config.team_left
    right = config.team_right

    for r in range(1, config.runs+1):
        s = SimsparkController(config.simspark_exe, config.simspark_server_port, config.simspark_agent_port)
        s.start()
        s.wait_connected()  # wait for the monitor to be connected

        # switch team sides
        if config.alternate:
            left, right = right, left

        agents = []

        for n in range(1, left.players+1):
            agents.append(('Left', AgentController(left.executable, left.config_dir, left.name, n, config.sync, True, False)))

        for n in range(1, right.players+1):
            agents.append(('Right', AgentController(right.executable, right.config_dir, right.name, n, config.sync, True, False)))

        # start all agents and wait for the agent to be fully started (prevent simspark error)
        for t, a in agents:
            if config.simspark_agent_port is not None: a.ss_port = config.simspark_agent_port
            a.start()
            a.wait_connected()

        # it takes sometimes a while until simspark got the correct player number
        for t, a in agents: wait_for(lambda: s.get_robot(a.number, t) is not None, 0.3)

        # wait until the player is on the field
        for t, a in agents: wait_for(lambda: s.get_robot(a.number, t) is not None and s.get_robot(a.number, t)['z'] <= 0.4, 0.3)

        wait_pause(config.pauseBeforeGame) # wait, before starting game

        prepare_game(s)     # move agents to ready location
        s.cmd_kickoff()     # start first half
        wait_half(r, s, 600, log, interrupt)   # wait for half complete

        if interrupt.is_set() or not s.is_alive():
            stop(s, agents)
            break

        log.half_result(r, left, right, 1, s.get_score('Left'), s.get_score('Right'))

        wait_pause(config.pauseHalfTime) # wait, before starting next half

        prepare_game(s)     # move agents to ready location
        s.cmd_kickoff()     # start first half
        wait_half(r, s, 1200, log, interrupt)  # wait for half complete

        log.half_result(r, left, right, 2, s.get_score('Left'), s.get_score('Right'))

        wait_pause(config.pauseAfterGame) # wait, before quitting game

        stop(s, agents)

        if interrupt.is_set():
            break

    log.finish()
    logging.info("Ended at {}".format(time.asctime()))
