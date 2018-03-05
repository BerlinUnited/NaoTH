'''
    Copied structure from the GameController "data.GameControlData.java" file.
'''

import logging
from struct import Struct


class GameControlData2018(Struct):
    """Representation of the SPL message format."""

    GAMECONTROLLER_STRUCT_HEADER = b'RGme'
    GAMECONTROLLER_STRUCT_VERSION = 11

    COMPETITION_PHASE_ROUNDROBIN  = 0
    COMPETITION_PHASE_PLAYOFF     = 1
    
    COMPETITION_TYPE_NORMAL               = 0
    COMPETITION_TYPE_MIXEDTEAM            = 1
    COMPETITION_TYPE_GENERAL_PENALTY_KICK = 2
    
    GAME_PHASE_NORMAL                   = 0
    GAME_PHASE_PENALTYSHOOT             = 1
    GAME_PHASE_OVERTIME                 = 2
    GAME_PHASE_TIMEOUT                  = 3
    
    STATE_INITIAL                       = 0
    STATE_READY                         = 1
    STATE_SET                           = 2
    STATE_PLAYING                       = 3
    STATE_FINISHED                      = 4

    SET_PLAY_NONE                       = 0
    SET_PLAY_GOAL_FREE_KICK             = 1
    SET_PLAY_PUSHING_FREE_KICK          = 2

    
    def __init__(self, data=None):
        """Constructor."""
        # initialize with the struct format characters as described here
        # https://docs.python.org/2/library/struct.html
        super(GameControlData2018, self).__init__('4sH10B3H')
        self.logger = logging.getLogger("GameControlData2018")
        # based on the given data the message is initialized with default values or parsed from the data string
        if data is None or self.unpack(data)[0]:
            self.setDefaults()

    def setDefaults(self):
        self.packetNumber = 0
        self.playersPerTeam = 6
        
        self.competitionPhase = self.COMPETITION_PHASE_ROUNDROBIN
        self.competitionType  = self.COMPETITION_TYPE_NORMAL
        self.gamePhase        = self.GAME_PHASE_NORMAL
        self.gameState        = self.STATE_INITIAL
        self.setPlay          = self.SET_PLAY_NONE
        
        self.firstHalf = 1
        self.kickingTeam = 0
        
        self.dropInTeam = 0
        self.dropInTime = -1
        self.secsRemaining = 600
        self.secondaryTime = 0
        # INFO: ignoring data, currently not need: TeamInfo

    def unpack(self, data):
        # check 'data' length
        if len(data) < self.size:
            return (False, "Not well formed GameControlData!")

        msg = Struct.unpack(self, data[:self.size])

        # check header
        if msg[0] != self.GAMECONTROLLER_STRUCT_HEADER:
            raise Exception("Invalid message type!")

        # check spl message version
        if msg[1] != self.GAMECONTROLLER_STRUCT_VERSION:
            raise Exception("Wrong version!")

        # assign data
        it = iter(msg[2:])
        self.packetNumber     = next(it)
        self.playersPerTeam   = next(it)
        
        self.competitionPhase = next(it)
        self.competitionType  = next(it)
        self.gamePhase        = next(it)
        self.gameState        = next(it)
        self.setPlay          = next(it)
        
        self.firstHalf = next(it)
        self.kickingTeam = next(it)
        self.dropInTeam = next(it)
        self.dropInTime = next(it)
        self.secsRemaining = next(it)
        self.secondaryTime = next(it)

        return (True, None)


    def pack(self):
        return Struct.pack(self,
                           self.SPL_STANDARD_MESSAGE_STRUCT_HEADER,
                           self.SPL_STANDARD_MESSAGE_STRUCT_VERSION,
                           self.packetNumber,
                           self.playersPerTeam,
                           self.competitionPhase,
                           self.competitionType,
                           self.gamePhase,
                           self.gameState,
                           self.setPlay,
                           self.firstHalf,
                           self.kickingTeam,
                           self.dropInTeam,
                           self.dropInTime,
                           self.secsRemaining,
                           self.secondaryTime
                           )

    def __str__(self):
        out = "              Header: " + str(self.GAMECONTROLLER_STRUCT_HEADER, 'utf-8') + "\n"
        out += "            Version: " + str(self.GAMECONTROLLER_STRUCT_VERSION) + "\n"
        out += "      Packet Number: " + str(self.packetNumber & 0xFF) + "\n"
        out += "   Players per Team: " + str(self.playersPerTeam) + "\n"
        
        out += "   competitionPhase: " + self.getCompetitionPhase() + "\n"
        out += "    competitionType: " + self.getCompetitionType() + "\n"
        out += "          gamePhase: " + self.getGamePhase() + "\n"
        out += "          gameState: " + self.getGameState() + "\n"
        out += "            setPlay: " + self.getSetPlay() + "\n"

        out += "          firstHalf: "
        if self.firstHalf == 1:
            out += "true"
        elif self.firstHalf == 0:
            out += "false"
        else:
            out += "undefinied(" + str(self.firstHalf) + ")"
            
        out += "\n"
        out += "        kickingTeam: " + str(self.kickingTeam) + "\n"
        out += "         dropInTeam: " + str(self.dropInTeam) + "\n"
        out += "         dropInTime: " + str(self.dropInTime) + "\n"
        out += "      secsRemaining: " + str(self.secsRemaining) + "\n"
        out += "      secondaryTime: " + str(self.secondaryTime) + "\n"

        return out

        
    def getName(self, names, value):
      if value in names:
        return names[value]
      else:
        return "undefinied({})".format(value)
        
        
    def getCompetitionPhase(self):
      return self.getName({
        self.COMPETITION_PHASE_ROUNDROBIN: "round robin",
        self.COMPETITION_PHASE_PLAYOFF   : "playoff"
      }, self.competitionPhase)
    
    def getCompetitionType(self):
      return self.getName({
        self.COMPETITION_TYPE_NORMAL: "normal",
        self.COMPETITION_TYPE_MIXEDTEAM: "mixed team",
        self.COMPETITION_TYPE_GENERAL_PENALTY_KICK: "penalty kick"
      }, self.competitionType)
            
    def getGamePhase(self):
      return self.getName({
        self.GAME_PHASE_NORMAL: "normal",
        self.GAME_PHASE_PENALTYSHOOT: "penalty shoot",
        self.GAME_PHASE_OVERTIME: "over time",
        self.GAME_PHASE_TIMEOUT: "timeout"
      }, self.gamePhase)
            
    def getGameState(self):
      return self.getName({
        self.STATE_INITIAL: "initial",
        self.STATE_READY: "ready",
        self.STATE_SET: "set",
        self.STATE_PLAYING: "playing",
        self.STATE_FINISHED: "finished"
      }, self.gameState)
    
    def getSetPlay(self):
      return self.getName({
        self.SET_PLAY_NONE: "none",
        self.SET_PLAY_GOAL_FREE_KICK: "goal free kick",
        self.SET_PLAY_PUSHING_FREE_KICK: "pushing free kick"
      }, self.setPlay)

