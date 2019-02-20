#! /usr/bin/env python3
import sys
import json
import argparse
import shutil
import sqlite3
import configparser
import signal

import naoth
from AgentController import AgentController
from SimsparkController import SimsparkController
from Utils import *

def parseArguments():
    parser = argparse.ArgumentParser(
        description="Sending HUP signal to a running simulation prints the current run and score of the simulation."
    )

    if '--config' in sys.argv:
        parser.add_argument('--config', type=__check_args_config, action='store', help="Use a configuration file instead of the application arguments")
    elif len(sys.argv) <= 2 and '--write-config' in sys.argv:
        parser.add_argument('--write-config', action='store',help="Writes the default config to the given file and exits.")
    else:
        parser.add_argument('-r','--runs', type=int, default=1, action='store', help="The number of runs (games), which should be performed")
        parser.add_argument('-l','--log', type=__check_args_log, action='store', help="The number of runs (games), which should be performed")
        parser.add_argument('-c','--comment', default='', action='store', help="A optional comment to describe this runs.")

        parser.add_argument('-s', '--simspark', type=__check_args_simspark, default='simspark', action='store', help="The simspark executable")
        parser.add_argument('-ap', '--agent-port', type=int, action='store', help="The port for agents to connect to simspark")
        parser.add_argument('-sp', '--server-port', type=int, action='store', help="The port for monitors to connect to simspark")

        parser.add_argument('--sync', action='store_true', help="Whether the simulation should be run synchronized")

        parser.add_argument('-lc', '--left-config', type=__check_args_agentconfig, default='.', action='store', help="The config directory for the left team")
        parser.add_argument('-le', '--left-exe', type=__check_args_exe, default='naoth-simspark', action='store', help="The agent executable for the left team")
        parser.add_argument('-lp', '--left-players', type=__check_args_positive, default=5, action='store', help="The the number of players for each team")
        parser.add_argument('-ln', '--left-name', type=str, default='NaoTH', action='store', help="The name of the left Team")

        parser.add_argument('-rc', '--right-config', type=__check_args_agentconfig, default='.', action='store', help="The config directory for the left team")
        parser.add_argument('-re', '--right-exe', type=__check_args_exe, default='naoth-simspark', action='store', help="The agent executable for the right team")
        parser.add_argument('-rp', '--right-players', type=__check_args_positive, default=5, action='store', help="The the number of players for each team")
        parser.add_argument('-rn', '--right-name', type=str, default='BU', action='store', help="The name of the right Team")

        parser.add_argument('--write-config', action='store', help="Writes the given config to the file and exits.")

    return parser.parse_args()

def __check_args_positive(value):
    ivalue = int(value)
    if ivalue <= 0:
        raise argparse.ArgumentTypeError("%s is an invalid positive int value" % value)
    return ivalue

def __check_args_simspark(value):
    if not shutil.which(value):
        raise argparse.ArgumentTypeError("Can not find simspark application (%s)!" % value)
    return value

def __check_args_exe(value):
    if not shutil.which(value) and not shutil.which(value, path='./'):
        raise argparse.ArgumentTypeError("Can not find simspark agent application (%s)!" % value)
    return value

def __check_args_agentconfig(value):
    if not os.path.isdir(os.path.join(value, 'Config')):
        raise argparse.ArgumentTypeError("Can not find config directory (%s)!" % value)
    return value

def __check_args_log(value:str):
    if not value.endswith('db'):
        raise argparse.ArgumentTypeError("Not a valid log target (%s)!" % value)

    if not os.path.exists(os.path.dirname(value)):
        raise argparse.ArgumentTypeError("Path to log target doesn't exist (%s)!" % value)

    return value

def __check_args_config(value):
    if not os.path.isfile(value):
        raise argparse.ArgumentTypeError("Configuration file doesn't exists (%s)!" % value)
    return value

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

class Log:
    def new_run(self, args):
        pass

    def log_ball(self, run, time, ball):
        pass

    def log_player(self, run, time, players):
        pass

    def half_result(self, run, half, left, right):
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
                                             left VARCHAR(10) NOT NULL, 
                                             right VARCHAR(10) NOT NULL, 
                                             players_left TINYINT NOT NULL, 
                                             players_right TINYINT NOT NULL, 
                                             comment text)
        """)
        self.__db.execute("""
            CREATE TABLE IF NOT EXISTS games (id INTEGER PRIMARY KEY AUTOINCREMENT, 
                                              rid INTEGER NOT NULL, 
                                              timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL, 
                                              run INTEGER NOT NULL, 
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
                                SELECT h1.rid, h1.run, h1.timestamp as half1_time, h1.score_left, h1.score_right,
                                       h2.timestamp as half2_time, h2.score_left, h2.score_right
                                FROM games h1 LEFT OUTER JOIN games h2 
                                ON h1.rid = h2.rid AND h1.run = h2.run AND h1.half = 1 AND h2.half = 2 
                                WHERE h2.id IS NOT NULL
        """)
        self.__cursor = None
        self.__rid = None

    def new_run(self, c):
        self.__cursor = self.__db.cursor()

        self.__cursor.execute("INSERT INTO runs (num_runs, sync, left, right, players_left, players_right, comment) VALUES (?,?,?,?,?,?,?)",
                              [c['runs'], c['sync'], c['left']['name'], c['right']['name'], c['left']['players'], c['right']['players'], c['comment']])
        self.__db.commit()
        self.__rid = self.__cursor.lastrowid

    def log_ball(self, run, time, ball):
        self.__cursor.execute("INSERT INTO positions (rid, run, game_time, type, x, y) VALUES (?,?,?,?,?,?)", [self.__rid, run, time, 'ball', ball['x'], ball['y']])

    def log_player(self, run, time, players):
        for p in players:
            self.__cursor.execute("INSERT INTO positions (rid, run, game_time, type, team, player, x, y, r) VALUES (?,?,?,?,?,?,?,?,?)", [self.__rid, run, time, 'player', p['team'], p['number'], p['x'], p['y'], p['r']])

    def half_result(self, run, half, left, right):
        self.__cursor.execute("INSERT INTO games (rid, run, half, score_left, score_right) VALUES (?,?,?,?,?)", [self.__rid, run, half, left, right])
        self.__db.commit()

    def finish(self):
        self.__cursor.close()
        self.__db.commit()
        self.__db.close()

        self.__cursor = None
        self.__rid = None
        self.__db = None

def createLog(log:str):
    if log.endswith('db'):
        return LogDb(log)
    return Log()

def wait_half(r, s, half_time, log:Log=None):
    t = s.get_time()
    while s.is_connected() and (t is None or t < half_time):
        # wait for the half to end
        time.sleep(1)
        t = s.get_time()
        if log:
            log.log_ball(r, t, s.get_ball())
            log.log_player(r, t, s.get_robots())

def configure(args):
    base = os.path.dirname(__file__)
    retrieve_path = lambda p: p if os.path.isabs(p) else os.path.abspath(os.path.join(base, p))

    if 'config' in args:
        c = configparser.ConfigParser()
        c.read(args.config)
        base = os.path.dirname(args.config)
        c = {
            'runs':     c.getint('general', 'RUNS'),
            'sync':     c.getboolean('general', 'SYNC'),
            'log':      retrieve_path(c.get('general', 'LOG')),
            'comment':  c.get('general', 'COMMENT'),
            'simspark': {
                'exe': c.get('simspark', 'exe'),
                'agent': c.get('simspark', 'agent'),
                'server': c.get('simspark', 'server')
            },
            'left': {
                'config':  retrieve_path(c.get('left', 'CONFIG')),
                'exe':     retrieve_path(c.get('left', 'EXE')),
                'name':    c.get('left', 'NAME'),
                'players': c.getint('left', 'PLAYERS')
            },
            'right': {
                'config':  retrieve_path(c.get('right', 'CONFIG')),
                'exe':     retrieve_path(c.get('right', 'EXE')),
                'name':    c.get('right', 'NAME'),
                'players': c.getint('right', 'PLAYERS')
            }
        }
    else:
        c = {
            'runs':     args.runs if 'runs' in args else 1,
            'sync':     args.sync if 'sync' in args else True,
            'log':      retrieve_path(args.log if 'log' in args else './result.db'),
            'comment':  args.comment if 'comment' in args else 'default',
            'simspark': {
                'exe': args.simspark if 'simspark' in args else 'simspark',
                'agent': args.agent_port if 'agent_port' in args else None,
                'server': args.server_port if 'server_port' in args else None
            },
            'left': {
                'config':   retrieve_path(args.left_config if 'left_config' in args else './left'),
                'exe':      retrieve_path(args.left_exe if 'left_exe' in args else './left/naoth-simspark'),
                'name':     args.left_name if 'left_name' in args else 'NaoTH',
                'players':  args.left_players if 'left_players' in args else 5
            },
            'right': {
                'config':   retrieve_path(args.right_config if 'right_config' in args else './right'),
                'exe':      retrieve_path(args.right_exe if 'right_exe' in args else './right/naoth-simspark'),
                'name':     args.right_name if 'right_name' in args else 'BU',
                'players':  args.right_players if 'right_players' in args else 5
            }
        }

    return c

def write_config(config, file):
    cp = configparser.ConfigParser()
    for c in config:
        if isinstance(config[c], dict):
            cp.add_section(c)
            for sc in config[c]:
                cp.set(c, sc, str(config[c][sc]))
        else:
            if not cp.has_section('general'): cp.add_section('general')
            cp.set('general', c, str(config[c]))
        # Writing our configuration file to 'example.cfg'
        with open(file, 'w') as configfile:
            cp.write(configfile)

def notify(signum, frame):
    print('Currently running game simulation #', r, '@', s.get_time(), ',', s.get_team('Left'), s.get_score('Left'), ':', s.get_score('Right'), s.get_team('Right'))

if __name__ == "__main__":
    args = parseArguments()
    config = configure(args)

    if 'write_config' in args:
        write_config(config, args.write_config)
        exit(0)

    logging.basicConfig(level=logging.DEBUG)

    # TODO: configures simspark

    logging.info("Started at {}".format(time.asctime()))

    log = createLog(config['log'])
    log.new_run(config)

    signal.signal(signal.SIGHUP, notify)

    for r in range(1, config['runs']+1):
        s = SimsparkController(config['simspark']['exe'], True)
        s.set_ports(config['simspark']['server'], config['simspark']['agent'])
        s.start()
        s.connected.wait()  # wait for the monitor to be connected

        agents = []

        for n in range(1, config['left']['players']+1):
            agents.append(AgentController(config['left']['exe'], config['left']['config'], config['left']['name'], n, config['sync'], True, False))

        for n in range(1, config['right']['players']+1):
            agents.append(AgentController(config['right']['exe'], config['right']['config'], config['right']['name'], n, config['sync'], True, False))

        # start all agents and wait for the agent to be fully started (prevent simspark error)
        for a in agents:
            if config['simspark']['agent'] is not None: a.ss_port = config['simspark']['agent']
            a.start()
            a.started.wait()

        # it takes sometimes a while until simspark got the correct player number
        for a in agents: wait_for(lambda: s.get_robot(a.number) is not None, 0.3)
        # wait until the player is on the field
        for a in agents: wait_for(lambda: s.get_robot(a.number)['z'] <= 0.4, 0.3)

        prepare_game(s)     # move agents to ready location
        s.cmd_kickoff()     # start first half
        wait_half(r, s, 600, log)   # wait for half complete

        log.half_result(r, 1, s.get_score('Left'), s.get_score('Right'))

        time.sleep(1)       # wait, before starting next half

        prepare_game(s)     # move agents to ready location
        s.cmd_kickoff()     # start first half
        wait_half(r, s, 1200, log)  # wait for half complete

        log.half_result(r, 2, s.get_score('Left'), s.get_score('Right'))

        s.cancel()          # stop simulation

        # wait for simspark to exit
        while s.is_alive():
            time.sleep(1)

        s.join()

        # stop remaining agents
        for a in agents: a.cancel()
        for a in agents: a.join()

    log.finish()

    logging.info("Ended at {}".format(time.asctime()))
