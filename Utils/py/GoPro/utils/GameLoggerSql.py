import queue
import sqlite3
import threading

import time

from utils import Logger, blackboard

# setup logger for network related logs
logger = Logger.getLogger("GoPro")

class GameLoggerSql(threading.Thread):

    def __init__(self, db, teams=None):
        super().__init__()

        self.__cancel = threading.Event()

        self.db = db
        self.teams = teams if teams is not None else {}
        self.maxTimeGameValid = 60*30

        self.state = { 'v': None }
        self.last_id = 0

        self.init_database()

    def run(self):
        con = sqlite3.connect(self.db)
        cur = con.cursor()
        while not self.__cancel.is_set():

            gopro = blackboard['gopro']
            if gopro is not None and gopro['state'] == 2 and self.state['v'] != gopro['lastVideo']:
                # got a videofile, add it to the game entry
                cur.execute("UPDATE game SET video = video || ? WHERE id = ?", [ str(gopro['lastVideo'])+', ', self.last_id ])
                con.commit()

            gc_data = blackboard['gamecontroller']
            if gc_data is not None:
                ts = time.gmtime()
                # check&get game id
                db_id = cur.execute("SELECT id FROM game WHERE timestmap >= ? and phase = ? and type = ? and half = ? and team_1 = ? and team_2 = ?", [ts-self.maxTimeGameValid, gc_data.competitionPhase, gc_data.competitionType, gc_data.firstHalf, gc_data.team[0].teamNumber, gc_data.team[1].teamNumber]).fetchone()
                if not db_id:
                    # new game
                    cur.execute("INSERT INTO game (timestmap, phase, type, half, team_1, team_1_name, team_2, team_2_name) VALUES (?,?,?,?,?,?,?,?)",
                                [ts, gc_data.competitionPhase, gc_data.competitionType, gc_data.firstHalf, gc_data.team[0].teamNumber, self.teams[gc_data.team[0].teamNumber] if gc_data.team[0].teamNumber in self.teams else None, gc_data.team[1].teamNumber, self.teams[gc_data.team[1].teamNumber] if gc_data.team[1].teamNumber in self.teams else None])
                    con.commit()
                    db_id = cur.lastrowid
                else:
                    db_id = db_id[0]

                if db_id is not None:
                    self.last_id = db_id
                    if db_id not in self.state:
                        # didn't have something about the current game - init
                        self.state[db_id] = { 'phase': gc_data.gamePhase, 'state': gc_data.gameState, 'set': gc_data.setPlay, 'half': gc_data.firstHalf, 'score': (gc_data.team[0].score, gc_data.team[1].score) }
                    else:
                        # game phase change
                        if self.state[db_id]['phase'] != gc_data.gamePhase:
                            cur.execute("INSERT INTO event (timestmap, game_id, type, time, extra) VALUES (?,?,?,?,?)",
                                        [ ts, db_id, 'phase_change', gc_data.secsRemaining, '[{}, {}]'.format(self.state[db_id]['phase'], gc_data.gamePhase)])
                            self.state[db_id]['phase'] = gc_data.gamePhase
                        # game state change
                        if self.state[db_id]['state'] != gc_data.gameState:
                            cur.execute("INSERT INTO event (timestmap, game_id, type, time, extra) VALUES (?,?,?,?,?)",
                                        [ts, db_id, 'state_change', gc_data.secsRemaining, '[{}, {}]'.format(self.state[db_id]['state'], gc_data.gameState)])
                            self.state[db_id]['state'] = gc_data.gameState
                        # free kick
                        if self.state[db_id]['set'] != gc_data.setPlay:
                            # only log the beginning of the free kick
                            if self.state[db_id]['set'] == 0:
                                cur.execute("INSERT INTO event (timestmap, game_id, type, time, extra) VALUES (?,?,?,?,?)",
                                            [ts, db_id, 'free_kick', gc_data.secsRemaining, str(gc_data.kickingTeam)])
                            self.state[db_id]['set'] = gc_data.setPlay
                        # goal scored
                        if self.state[db_id]['score'][0] != gc_data.team[0].score or self.state[db_id]['score'][1] != gc_data.team[1].score:
                            cur.execute("INSERT INTO event (timestmap, game_id, type, time, extra) VALUES (?,?,?,?,?)",
                                        [ts, db_id, 'goal', msg.secsRemaining, '[{}, {}]'.format(gc_data.team[0].score, gc_data.team[1].score)])
                            self.state[db_id]['score'] = (gc_data.team[0].score, gc_data.team[1].score)

        con.commit()
        con.close()

    def cancel(self):
        self.__cancel.set()

    def init_database(self):
        con = sqlite3.connect(self.db)
        con.execute("CREATE TABLE IF NOT EXISTS game (id INTEGER PRIMARY KEY AUTOINCREMENT, timestmap INTEGER, phase INTEGER, type INTEGER, half INTEGER, team_1 INTEGER, team_1_name VARCHAR(50), team_2 INTEGER, team_2_name VARCHAR(50), video TEXT DEFAULT '');")
        con.execute("CREATE TABLE IF NOT EXISTS event (timestmap INTEGER, game_id INTEGER, type VARCHAR(20), time INTEGER, extra TEXT, PRIMARY KEY (timestmap, game_id, type), FOREIGN KEY (game_id) REFERENCES game (id) ON DELETE CASCADE ON UPDATE NO ACTION);")
        con.commit()
        con.close()
