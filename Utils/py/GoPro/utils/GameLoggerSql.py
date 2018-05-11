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
        self.messages = queue.Queue()
        self.state = {}

        self.init_database()

    def run(self):
        con = sqlite3.connect(self.db)
        cur = con.cursor()
        while not self.__cancel.is_set():
            ts, msg = self.messages.get()
            db_id = cur.execute("SELECT id FROM game WHERE phase = ? and type = ? and half = ? and team_1 = ? and team_2 = ?", [msg.competitionPhase, msg.competitionType, msg.firstHalf, msg.team[0].teamNumber, msg.team[1].teamNumber]).fetchone()
            if not db_id:
                # new game
                cur.execute("INSERT INTO game (timestmap, phase, type, half, team_1, team_1_name, team_2, team_2_name) VALUES (?,?,?,?,?,?,?,?)",
                            [ts, msg.competitionPhase, msg.competitionType, msg.firstHalf, msg.team[0].teamNumber, self.teams[msg.team[0].teamNumber] if msg.team[0].teamNumber in self.teams else None, msg.team[1].teamNumber, self.teams[msg.team[1].teamNumber] if msg.team[1].teamNumber in self.teams else None])
                con.commit()
                db_id = cur.lastrowid
            else:
                db_id = db_id[0]

            if db_id is not None:
                if db_id not in self.state:
                    # didn't have something about the current game - init
                    self.state[db_id] = { 'phase': msg.gamePhase, 'state': msg.gameState, 'set': msg.setPlay, 'half': msg.firstHalf, 'score': (msg.team[0].score, msg.team[1].score) }
                else:
                    pass
                    # TODO: log the changes
                    '''
                    event types:
                        - game_start
                        - game_end
                        - goal
                        - phase_change
                        - state_change
                        - free_kick
                    '''

            print(db_id)
        #gamePhase
        #gameState
        #setPlay
        #firstHalf
        #secsRemaining
        #team[0].teamNumber
        # team[0].score
        con.commit()
        con.close()

    def cancel(self):
        self.__cancel.set()
        self.messages.task_done()
        self.messages.join()
        print("logger finished")

    def init_database(self):
        con = sqlite3.connect(self.db)
        con.execute("CREATE TABLE IF NOT EXISTS game (id INTEGER PRIMARY KEY AUTOINCREMENT, timestmap INTEGER, phase INTEGER, type INTEGER, half INTEGER, team_1 INTEGER, team_1_name VARCHAR(50), team_2 INTEGER, team_2_name VARCHAR(50), video TEXT);")
        con.execute("CREATE TABLE IF NOT EXISTS event (timestmap INTEGER, game_id INTEGER, type VARCHAR(20), time INTEGER, extra TEXT, PRIMARY KEY (timestmap, game_id, type), FOREIGN KEY (game_id) REFERENCES game (id) ON DELETE CASCADE ON UPDATE NO ACTION);")
        con.commit()
        con.close()


    def receivedGC(self, evt:Event.GameControllerMessage):
        """ Is called, when a new GameController message was received. """
        self.messages.put_nowait((time.time(), evt.message))

    def timeoutGC(self, evt:Event.GameControllerTimedout):
        """ Is called, when a new GameController times out. """
        pass

    # TODO: track recording state of the camera in order to update the video files
