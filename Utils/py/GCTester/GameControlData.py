'''
    Python representation of the RoboCupGameControlData.h packet layout.

    Updated from RoboCupGameControlData.h / GAMECONTROLLER_STRUCT_VERSION 20.
'''

import logging
from struct import Struct


GAMECONTROLLER_DATA_PORT = 3838
GAMECONTROLLER_RETURN_PORT = 3939
MAX_NUM_PLAYERS = 20


class GameControlData(Struct):
    """Representation of the RoboCup game controller message format."""

    TRUE_HEADER = b'RGTD'
    GAMECONTROLLER_STRUCT_HEADER = b'RGme'
    GAMECONTROLLER_STRUCT_VERSION = 20

    COMPETITION_TYPE_SMALL = 0
    COMPETITION_TYPE_MIDDLE = 1
    COMPETITION_TYPE_LARGE = 2

    GAME_PHASE_NORMAL = 0
    GAME_PHASE_PENALTY_SHOOT_OUT = 1
    GAME_PHASE_EXTRA_TIME = 2
    GAME_PHASE_TIMEOUT = 3

    # Backwards-compatible spellings used by older code.
    GAME_PHASE_PENALTYSHOOT = GAME_PHASE_PENALTY_SHOOT_OUT
    GAME_PHASE_EXTRATIME = GAME_PHASE_EXTRA_TIME
    GAME_PHASE_OVERTIME = GAME_PHASE_EXTRA_TIME

    STATE_INITIAL = 0
    STATE_READY = 1
    STATE_SET = 2
    STATE_PLAYING = 3
    STATE_FINISHED = 4

    SET_PLAY_NONE = 0
    SET_PLAY_DIRECT_FREE_KICK = 1
    SET_PLAY_INDIRECT_FREE_KICK = 2
    SET_PLAY_PENALTY_KICK = 3
    SET_PLAY_THROW_IN = 4
    SET_PLAY_GOAL_KICK = 5
    SET_PLAY_CORNER_KICK = 6

    KICKING_TEAM_NONE = 255

    def __init__(self, data=None):
        """Constructor."""
        # Explicit little-endian, standard sizes, and no implicit alignment.
        super().__init__('<'
                         '4s'  # header
                         'B'   # version
                         'B'   # packetNumber
                         'B'   # playersPerTeam
                         'B'   # competitionType
                         'B'   # stopped
                         'B'   # gamePhase
                         'B'   # state
                         'B'   # setPlay
                         'B'   # firstHalf
                         'B'   # kickingTeam
                         'h'   # secsRemaining
                         'h'   # secondaryTime
                         )
        self.logger = logging.getLogger('GameControlData')
        self.setDefaults()

        if data is not None:
            self.unpack(data)

    @property
    def header_size(self):
        """Size of the fixed RoboCupGameControlData header before team data."""
        return super().size

    @property
    def size(self):
        """Total packet size including both teams."""
        return self.header_size + sum(t.size for t in self.teams)

    def setDefaults(self):
        self.isTrueData = False
        self.packetNumber = 0
        self.playersPerTeam = 7
        self.competitionType = self.COMPETITION_TYPE_SMALL
        self.stopped = 0
        self.gamePhase = self.GAME_PHASE_NORMAL
        self.gameState = self.STATE_INITIAL
        self.setPlay = self.SET_PLAY_NONE
        self.firstHalf = 1
        self.kickingTeam = self.KICKING_TEAM_NONE
        self.secsRemaining = 600
        self.secondaryTime = 0
        self.teams = [TeamInfo(), TeamInfo()]

    @property
    def state(self):
        """Alias for the header's `state` field."""
        return self.gameState

    @state.setter
    def state(self, value):
        self.gameState = value

    @property
    def team(self):
        """Backwards-compatible alias for `teams`."""
        return self.teams

    @team.setter
    def team(self, value):
        self.teams = value

    def unpack(self, data):
        if len(data) < self.size:
            raise Exception(f"Unexpected size: {len(data)} expected {self.size}")

        msg = Struct.unpack(self, data[:self.header_size])

        self.isTrueData = False
        if msg[0] == self.TRUE_HEADER:
            self.isTrueData = True
        elif msg[0] != self.GAMECONTROLLER_STRUCT_HEADER:
            raise Exception('Invalid message type!')

        if msg[1] != self.GAMECONTROLLER_STRUCT_VERSION:
            raise Exception('Wrong version!')

        it = iter(msg[2:])
        self.packetNumber = next(it)
        self.playersPerTeam = next(it)
        self.competitionType = next(it)
        self.stopped = next(it)
        self.gamePhase = next(it)
        self.gameState = next(it)
        self.setPlay = next(it)
        self.firstHalf = next(it)
        self.kickingTeam = next(it)
        self.secsRemaining = next(it)
        self.secondaryTime = next(it)

        offset = self.header_size
        for team in self.teams:
            team.unpack(data[offset:offset + team.size])
            offset += team.size

        return True, None

    def pack(self):
        payload = Struct.pack(self,
                              self.GAMECONTROLLER_STRUCT_HEADER,
                              self.GAMECONTROLLER_STRUCT_VERSION,
                              self.packetNumber,
                              self.playersPerTeam,
                              self.competitionType,
                              self.stopped,
                              self.gamePhase,
                              self.gameState,
                              self.setPlay,
                              self.firstHalf,
                              self.kickingTeam,
                              self.secsRemaining,
                              self.secondaryTime)
        payload += b''.join(team.pack() for team in self.teams)
        return payload

    def __str__(self):
        out =  '             Header: ' + str(self.GAMECONTROLLER_STRUCT_HEADER, 'utf-8') + '\n'
        out += '            Version: ' + str(self.GAMECONTROLLER_STRUCT_VERSION) + '\n'
        out += '      Packet Number: ' + str(self.packetNumber & 0xFF) + '\n'
        out += '   Players per Team: ' + str(self.playersPerTeam) + '\n'
        out += '    competitionType: ' + self.getCompetitionType() + '\n'
        out += '            stopped: ' + str(self.stopped) + '\n'
        out += '          gamePhase: ' + self.getGamePhase() + '\n'
        out += '              state: ' + self.getGameState() + '\n'
        out += '            setPlay: ' + self.getSetPlay() + '\n'

        out += '          firstHalf: '
        if self.firstHalf == 1:
            out += 'true'
        elif self.firstHalf == 0:
            out += 'false'
        else:
            out += 'undefined(' + str(self.firstHalf) + ')'

        out += '\n'
        out += '        kickingTeam: ' + self.getKickingTeam() + '\n'
        out += '      secsRemaining: ' + str(self.secsRemaining) + '\n'
        out += '      secondaryTime: ' + str(self.secondaryTime) + '\n'

        for team in self.teams:
            out += str(team)

        return out

    def getName(self, names, value):
        return names.get(value, 'undefined({})'.format(value))

    def getCompetitionType(self):
        return self.getName({
            self.COMPETITION_TYPE_SMALL: 'small',
            self.COMPETITION_TYPE_MIDDLE: 'middle',
            self.COMPETITION_TYPE_LARGE: 'large',
        }, self.competitionType)

    def getGamePhase(self):
        return self.getName({
            self.GAME_PHASE_NORMAL: 'normal',
            self.GAME_PHASE_PENALTY_SHOOT_OUT: 'penalty shoot out',
            self.GAME_PHASE_EXTRA_TIME: 'extra time',
            self.GAME_PHASE_TIMEOUT: 'timeout',
        }, self.gamePhase)

    def getGameState(self):
        return self.getName({
            self.STATE_INITIAL: 'initial',
            self.STATE_READY: 'ready',
            self.STATE_SET: 'set',
            self.STATE_PLAYING: 'playing',
            self.STATE_FINISHED: 'finished',
        }, self.gameState)

    def getSetPlay(self):
        return self.getName({
            self.SET_PLAY_NONE: 'none',
            self.SET_PLAY_DIRECT_FREE_KICK: 'direct free kick',
            self.SET_PLAY_INDIRECT_FREE_KICK: 'indirect free kick',
            self.SET_PLAY_PENALTY_KICK: 'penalty kick',
            self.SET_PLAY_THROW_IN: 'throw in',
            self.SET_PLAY_GOAL_KICK: 'goal kick',
            self.SET_PLAY_CORNER_KICK: 'corner kick',
        }, self.setPlay)

    def getKickingTeam(self):
        if self.kickingTeam == self.KICKING_TEAM_NONE:
            return 'none'
        return str(self.kickingTeam)


class TeamInfo(Struct):
    """Representation of TeamInfo."""

    MAX_NUM_PLAYERS = MAX_NUM_PLAYERS

    TEAM_BLUE = 0    # blue, cyan
    TEAM_RED = 1     # red, magenta, pink
    TEAM_YELLOW = 2  # yellow
    TEAM_BLACK = 3   # black, dark gray
    TEAM_WHITE = 4   # white
    TEAM_GREEN = 5   # green
    TEAM_ORANGE = 6  # orange
    TEAM_PURPLE = 7  # purple, violet
    TEAM_BROWN = 8   # brown
    TEAM_GRAY = 9    # lighter gray

    def __init__(self, data=None):
        super().__init__('<'
                         'B'  # teamNumber
                         'B'  # fieldPlayerColour
                         'B'  # goalkeeperColour
                         'B'  # goalkeeper
                         'B'  # score
                         'B'  # penaltyShot
                         'H'  # singleShots
                         'H'  # messageBudget
                         )
        self.setDefaults()

        if data is not None:
            self.unpack(data)

    @property
    def header_size(self):
        return super().size

    @property
    def size(self):
        return self.header_size + sum(player.size for player in self.players)

    def setDefaults(self):
        self.teamNumber = 0
        self.fieldPlayerColour = 0
        self.goalkeeperColour = 0
        self.goalkeeper = 0
        self.score = 0
        self.penaltyShot = 0
        self.singleShots = 0
        self.messageBudget = 0
        self.players = [RobotInfo() for _ in range(self.MAX_NUM_PLAYERS)]

    @property
    def fieldPlayerColor(self):
        """Backwards-compatible American-spelling alias."""
        return self.fieldPlayerColour

    @fieldPlayerColor.setter
    def fieldPlayerColor(self, value):
        self.fieldPlayerColour = value

    @property
    def goalkeeperColor(self):
        """Backwards-compatible American-spelling alias."""
        return self.goalkeeperColour

    @goalkeeperColor.setter
    def goalkeeperColor(self, value):
        self.goalkeeperColour = value

    @property
    def player(self):
        """Backwards-compatible alias for `players`."""
        return self.players

    @player.setter
    def player(self, value):
        self.players = value

    def unpack(self, data):
        if len(data) != self.size:
            raise Exception(f"Unexpected size: {len(data)} expected {self.size}")

        msg = Struct.unpack(self, data[:self.header_size])

        it = iter(msg)
        self.teamNumber = next(it)
        self.fieldPlayerColour = next(it)
        self.goalkeeperColour = next(it)
        self.goalkeeper = next(it)
        self.score = next(it)
        self.penaltyShot = next(it)
        self.singleShots = next(it)
        self.messageBudget = next(it)

        offset = self.header_size
        for player in self.players:
            player.unpack(data[offset:offset + player.size])
            offset += player.size

        return True, None

    def pack(self):
        payload = Struct.pack(self,
                              self.teamNumber,
                              self.fieldPlayerColour,
                              self.goalkeeperColour,
                              self.goalkeeper,
                              self.score,
                              self.penaltyShot,
                              self.singleShots,
                              self.messageBudget)
        payload += b''.join(player.pack() for player in self.players)
        return payload

    def getName(self, names, value):
        return names.get(value, 'undefined({})'.format(value))

    def getColorName(self, value):
        return self.getName({
            self.TEAM_BLUE: 'blue',
            self.TEAM_RED: 'red',
            self.TEAM_YELLOW: 'yellow',
            self.TEAM_BLACK: 'black',
            self.TEAM_WHITE: 'white',
            self.TEAM_GREEN: 'green',
            self.TEAM_ORANGE: 'orange',
            self.TEAM_PURPLE: 'purple',
            self.TEAM_BROWN: 'brown',
            self.TEAM_GRAY: 'gray',
        }, value)

    def getFieldPlayerColor(self):
        return self.getColorName(self.fieldPlayerColour)

    def getGoalkeeperColor(self):
        return self.getColorName(self.goalkeeperColour)

    def getColor(self):
        """Backwards-compatible helper for field player colour."""
        return self.getFieldPlayerColor()

    def __str__(self):
        out = '--------------------------------------\n'
        out += '         teamNumber: ' + str(self.teamNumber) + '\n'
        out += '  fieldPlayerColour: ' + self.getFieldPlayerColor() + '\n'
        out += '   goalkeeperColour: ' + self.getGoalkeeperColor() + '\n'
        out += '         goalkeeper: ' + str(self.goalkeeper) + '\n'
        out += '              score: ' + str(self.score) + '\n'
        out += '        penaltyShot: ' + str(self.penaltyShot) + '\n'
        out += '        singleShots: ' + str(self.singleShots) + '\n'
        out += '      messageBudget: ' + str(self.messageBudget) + '\n'
        for i, player in enumerate(self.players):
            out += '          Player #' + str(i + 1) + ': ' + str(player) + '\n'
        return out


class RobotInfo(Struct):
    """Representation of RobotInfo."""

    PENALTY_NONE = 0
    PENALTY_ILLEGAL_POSITIONING = 1
    PENALTY_MOTION_IN_SET = 2
    PENALTY_MOTION_IN_STOP = 3
    PENALTY_LOCAL_GAME_STUCK = 4
    PENALTY_INCAPABLE_ROBOT = 5
    PENALTY_PICK_UP = 6
    PENALTY_BALL_HOLDING = 7
    PENALTY_LEAVING_THE_FIELD = 8
    PENALTY_PLAYING_WITH_ARMS_HANDS = 9
    PENALTY_PUSHING = 10
    PENALTY_CAUTIONED = 11
    PENALTY_SENT_OFF = 12
    PENALTY_SUBSTITUTE = 13

    def __init__(self, data=None):
        super().__init__('<'
                         'B'  # penalty
                         'B'  # secsTillUnpenalised
                         'B'  # cautions
                         )
        self.setDefaults()

        if data is not None:
            self.unpack(data)

    def setDefaults(self):
        self.penalty = self.PENALTY_NONE
        self.secsTillUnpenalised = 0
        self.cautions = 0

    def unpack(self, data):
        if len(data) != self.size:
            raise Exception(f"Unexpected size: {len(data)} expected {self.size}")

        msg = Struct.unpack(self, data)

        it = iter(msg)
        self.penalty = next(it)
        self.secsTillUnpenalised = next(it)
        self.cautions = next(it)

        return True, None

    def pack(self):
        return Struct.pack(self,
                           self.penalty,
                           self.secsTillUnpenalised,
                           self.cautions)

    def getName(self, names, value):
        return names.get(value, 'undefined({})'.format(value))

    def getPenalty(self):
        return self.getName({
            self.PENALTY_NONE: 'none',
            self.PENALTY_ILLEGAL_POSITIONING: 'illegal positioning',
            self.PENALTY_MOTION_IN_SET: 'motion in set',
            self.PENALTY_MOTION_IN_STOP: 'motion in stop',
            self.PENALTY_LOCAL_GAME_STUCK: 'local game stuck',
            self.PENALTY_INCAPABLE_ROBOT: 'incapable robot',
            self.PENALTY_PICK_UP: 'pick up',
            self.PENALTY_BALL_HOLDING: 'ball holding',
            self.PENALTY_LEAVING_THE_FIELD: 'leaving the field',
            self.PENALTY_PLAYING_WITH_ARMS_HANDS: 'playing with arms/hands',
            self.PENALTY_PUSHING: 'pushing',
            self.PENALTY_CAUTIONED: 'cautioned',
            self.PENALTY_SENT_OFF: 'sent off',
            self.PENALTY_SUBSTITUTE: 'substitute',
        }, self.penalty)

    def __str__(self):
        out = 'penalty: ' + self.getPenalty()
        out += ', secsTillUnpenalised: ' + str(self.secsTillUnpenalised)
        out += ', cautions: ' + str(self.cautions)
        return out


# Backwards-compatible class name used by the older Python file.
PlayerInfo = RobotInfo


class GameControlReturnData(Struct):
    """Representation of RoboCupGameControlReturnData."""

    GAMECONTROLLER_RETURN_STRUCT_HEADER = b'RGrt'
    GAMECONTROLLER_RETURN_STRUCT_VERSION = 4

    def __init__(self, data=None):
        super().__init__('<'
                         '4s'  # header
                         'B'   # version
                         'B'   # playerNum
                         'B'   # teamNum
                         'B'   # fallen
                         '3f'  # pose: x, y, theta
                         'f'   # ballAge
                         '2f'  # ball: x, y
                         )
        self.setDefaults()

        if data is not None:
            self.unpack(data)

    def setDefaults(self):
        self.playerNum = 0
        self.teamNum = 0
        self.fallen = 255
        self.pose = [0.0, 0.0, 0.0]
        self.ballAge = -1.0
        self.ball = [0.0, 0.0]

    def unpack(self, data):
        if len(data) != self.size:
            raise Exception(f"Unexpected size: {len(data)} expected {self.size}")

        msg = Struct.unpack(self, data)

        if msg[0] != self.GAMECONTROLLER_RETURN_STRUCT_HEADER:
            raise Exception('Invalid return message type!')

        if msg[1] != self.GAMECONTROLLER_RETURN_STRUCT_VERSION:
            raise Exception('Wrong return version!')

        self.playerNum = msg[2]
        self.teamNum = msg[3]
        self.fallen = msg[4]
        self.pose = [msg[5], msg[6], msg[7]]
        self.ballAge = msg[8]
        self.ball = [msg[9], msg[10]]

        return True, None

    def pack(self):
        return Struct.pack(self,
                           self.GAMECONTROLLER_RETURN_STRUCT_HEADER,
                           self.GAMECONTROLLER_RETURN_STRUCT_VERSION,
                           self.playerNum,
                           self.teamNum,
                           self.fallen,
                           self.pose[0],
                           self.pose[1],
                           self.pose[2],
                           self.ballAge,
                           self.ball[0],
                           self.ball[1])
