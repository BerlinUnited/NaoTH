import queue
import sqlite3
import threading

import time

from utils import Event, Logger

# setup logger for network related logs
logger = Logger.getLogger("GoPro")

class GameLoggerSql(threading.Thread):

    def __init__(self, db, teams=None):
        super().__init__()

        Event.registerListener(self)
        self.__cancel = threading.Event()

        self.db = db
        self.teams = teams if teams is not None else {}
        self.maxTimeGameValid = 60*30
        self.messages = queue.Queue()

        self.state = {}
        self.last_id = 0

        self.init_database()

    def run(self):
        con = sqlite3.connect(self.db)
        cur = con.cursor()
        while not self.__cancel.is_set():
            ts, msg = self.messages.get()
            if isinstance(msg, tuple):
                # got a videofile, add it to the game entry
                cur.execute("UPDATE game SET video = video || ? WHERE id = ?", [ str(msg[1])+', ', msg[0] ])
                con.commit()
            else:
                # check&get game id
                db_id = cur.execute("SELECT id FROM game WHERE timestmap >= ? and phase = ? and type = ? and half = ? and team_1 = ? and team_2 = ?", [ts-self.maxTimeGameValid, msg.competitionPhase, msg.competitionType, msg.firstHalf, msg.team[0].teamNumber, msg.team[1].teamNumber]).fetchone()
                if not db_id:
                    # new game
                    cur.execute("INSERT INTO game (timestmap, phase, type, half, team_1, team_1_name, team_2, team_2_name) VALUES (?,?,?,?,?,?,?,?)",
                                [ts, msg.competitionPhase, msg.competitionType, msg.firstHalf, msg.team[0].teamNumber, self.teams[msg.team[0].teamNumber] if msg.team[0].teamNumber in self.teams else None, msg.team[1].teamNumber, self.teams[msg.team[1].teamNumber] if msg.team[1].teamNumber in self.teams else None])
                    con.commit()
                    db_id = cur.lastrowid
                else:
                    db_id = db_id[0]

                if db_id is not None:
                    self.last_id = db_id
                    if db_id not in self.state:
                        # didn't have something about the current game - init
                        self.state[db_id] = { 'phase': msg.gamePhase, 'state': msg.gameState, 'set': msg.setPlay, 'half': msg.firstHalf, 'score': (msg.team[0].score, msg.team[1].score) }
                    else:
                        # game phase change
                        if self.state[db_id]['phase'] != msg.gamePhase:
                            cur.execute("INSERT INTO event (timestmap, game_id, type, time, extra) VALUES (?,?,?,?,?)",
                                        [ ts, db_id, 'phase_change', msg.secsRemaining, '[{}, {}]'.format(self.state[db_id]['phase'], msg.gamePhase)])
                            self.state[db_id]['phase'] = msg.gamePhase
                        # game state change
                        if self.state[db_id]['state'] != msg.gameState:
                            cur.execute("INSERT INTO event (timestmap, game_id, type, time, extra) VALUES (?,?,?,?,?)",
                                        [ts, db_id, 'state_change', msg.secsRemaining, '[{}, {}]'.format(self.state[db_id]['state'], msg.gameState)])
                            self.state[db_id]['state'] = msg.gameState
                        # free kick
                        if self.state[db_id]['set'] != msg.setPlay:
                            # only log the beginning of the free kick
                            if self.state[db_id]['set'] == 0:
                                cur.execute("INSERT INTO event (timestmap, game_id, type, time, extra) VALUES (?,?,?,?,?)",
                                            [ts, db_id, 'free_kick', msg.secsRemaining, str(msg.kickingTeam)])
                            self.state[db_id]['set'] = msg.setPlay
                        # goal scored
                        if self.state[db_id]['score'][0] != msg.team[0].score or self.state[db_id]['score'][1] != msg.team[1].score:
                            cur.execute("INSERT INTO event (timestmap, game_id, type, time, extra) VALUES (?,?,?,?,?)",
                                        [ts, db_id, 'goal', msg.secsRemaining, '[{}, {}]'.format(msg.team[0].score, msg.team[1].score)])
                            self.state[db_id]['score'] = (msg.team[0].score, msg.team[1].score)
            self.messages.task_done()

        con.commit()
        con.close()

    def cancel(self):
        self.__cancel.set()
        self.messages.join()

    def init_database(self):
        con = sqlite3.connect(self.db)
        con.execute("CREATE TABLE IF NOT EXISTS game (id INTEGER PRIMARY KEY AUTOINCREMENT, timestmap INTEGER, phase INTEGER, type INTEGER, half INTEGER, team_1 INTEGER, team_1_name VARCHAR(50), team_2 INTEGER, team_2_name VARCHAR(50), video TEXT DEFAULT '');")
        con.execute("CREATE TABLE IF NOT EXISTS event (timestmap INTEGER, game_id INTEGER, type VARCHAR(20), time INTEGER, extra TEXT, PRIMARY KEY (timestmap, game_id, type), FOREIGN KEY (game_id) REFERENCES game (id) ON DELETE CASCADE ON UPDATE NO ACTION);")
        con.commit()
        con.close()


    def receivedGC(self, evt:Event.GameControllerMessage):
        """ Is called, when a new GameController message was received. """
        self.messages.put_nowait((time.time(), evt.message))

    def timeoutGC(self, evt:Event.GameControllerTimedout):
        """ Is called, when a new GameController times out. """
        pass

    def stoppedGopro(self, evt:Event.GoproStopRecording):
        """ Is called, when the gopro stops recording. """
        if evt.file is not None and len(evt.file) > 0 and self.last_id > 0:
            # only if a 'valid' file is provided
            self.messages.put_nowait((time.time(), (self.last_id, evt.file)))

