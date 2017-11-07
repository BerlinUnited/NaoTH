from struct import Struct


class GameControlData(Struct):
    """Representation of the SPL message format."""

    GAMECONTROLLER_STRUCT_HEADER = b'RGme'
    GAMECONTROLLER_STRUCT_VERSION = 10

    GAME_ROUNDROBIN = 0
    GAME_PLAYOFF = 1
    GAME_MIXEDTEAM_ROUNDROBIN = 2
    GAME_MIXEDTEAM_PLAYOFF = 3

    STATE_INITIAL = 0
    STATE_READY = 1
    STATE_SET = 2
    STATE_PLAYING = 3
    STATE_FINISHED = 4

    STATE2_NORMAL = 0
    STATE2_PENALTYSHOOT = 1
    STATE2_OVERTIME = 2
    STATE2_TIMEOUT = 3

    def __init__(self, data=None):
        """Constructor."""
        super(GameControlData, self).__init__('4sh8b3h')
        # based on the given data the message is initialized with default values or parsed from the data string
        if data is not None:
            self.unpack(data)
        else:
            self.setDefaults()

    def setDefaults(self):
        '''
    // GAMECONTROLLER_STRUCT_HEADER                             // header to identify the structure
    // GAMECONTROLLER_STRUCT_VERSION                            // version of the data structure
    public byte packetNumber = 0;
    public byte playersPerTeam = (byte) Rules.league.teamSize;   // The number of players on a team
    public byte gameType = GAME_ROUNDROBIN;                     // type of the game (GAME_ROUNDROBIN, GAME_PLAYOFF, GAME_DROPIN)
    public byte gameState = STATE_INITIAL;                      // state of the game (STATE_READY, STATE_PLAYING, etc)
    public byte firstHalf = C_TRUE;                             // 1 = game in first half, 0 otherwise
    public byte kickOffTeam;                                    // the next team to kick off
    public byte secGameState = STATE2_NORMAL;                   // Extra state information - (STATE2_NORMAL, STATE2_PENALTYSHOOT, etc)
    public byte dropInTeam;                                     // team that caused last drop in
    protected short dropInTime = -1;                            // number of seconds passed since the last drop in. -1 before first dropin
    public short secsRemaining = (short) Rules.league.halfTime; // estimate of number of seconds remaining in the half
    public short secondaryTime = 0;                             // sub-time (remaining in ready state etc.) in seconds
    public TeamInfo[] team = new TeamInfo[2];
        '''
        self.packetNumber = 0
        self.playersPerTeam = 6
        self.gameType = self.GAME_ROUNDROBIN
        self.gameState = self.STATE_INITIAL
        self.firstHalf = 1
        self.kickOffTeam = 0
        self.secGameState = self.STATE2_NORMAL
        self.dropInTeam = 0
        self.dropInTime = -1
        self.secsRemaining = 600
        self.secondaryTime = 0

    def unpack(self, data):
        # check 'data' length
        if len(data) < self.size:
            print("ERROR: Not an SPL Message.")
            #raise NotAnSplMessage("Not an SPL Message.")

        msg = Struct.unpack(self, data[:self.size])

        # check header
        if msg[0] != self.GAMECONTROLLER_STRUCT_HEADER:
            #raise InvalidSplHeader("Invalid header!")
            print("ERROR: wrong message type!")

        # check spl message version
        if msg[1] != self.GAMECONTROLLER_STRUCT_VERSION:
            #raise WrongSplMessageVersion("Wrong version: received " + msg[1] + ", but expected " + self.SPL_STANDARD_MESSAGE_STRUCT_VERSION)
            print("ERROR: wrong version!")

        # assign data
        it = iter(msg[2:])
        self.packetNumber = next(it)
        self.playersPerTeam = next(it)
        self.gameType = next(it)
        self.gameState = next(it)
        self.firstHalf = next(it)
        self.kickOffTeam = next(it)
        self.secGameState = next(it)
        self.dropInTeam = next(it)
        self.dropInTime = next(it)
        self.secsRemaining = next(it)
        self.secondaryTime = next(it)


    def pack(self):
        return Struct.pack(self,
                           self.SPL_STANDARD_MESSAGE_STRUCT_HEADER,
                           self.SPL_STANDARD_MESSAGE_STRUCT_VERSION,
                           self.packetNumber,
                           self.playersPerTeam,
                           self.gameType,
                           self.gameState,
                           self.firstHalf,
                           self.kickOffTeam,
                           self.secGameState,
                           self.dropInTeam,
                           self.dropInTime,
                           self.secsRemaining,
                           self.secondaryTime
                           )

    def __str__(self):
        out = "             Header: " + str(self.GAMECONTROLLER_STRUCT_HEADER, 'utf-8') + "\n"
        out += "            Version: " + str(self.GAMECONTROLLER_STRUCT_VERSION) + "\n"
        out += "      Packet Number: " + str(self.packetNumber & 0xFF) + "\n"
        out += "   Players per Team: " + str(self.playersPerTeam) + "\n"
        out += "           gameType: " + self.getGameType() + "\n"
        out += "          gameState: " + self.getGameState() + "\n"
        out += "          firstHalf: "
        if self.firstHalf == 1:
            out += "true"
        elif self.firstHalf == 0:
            out += "false"
        else:
            out += "undefinied(" + str(self.firstHalf) + ")"
        out += "\n"
        out += "        kickOffTeam: " + str(self.kickOffTeam) + "\n"
        out += "       secGameState: " + self.getSecGameState() + "\n"
        out += "         dropInTeam: " + str(self.dropInTeam) + "\n"
        out += "         dropInTime: " + str(self.dropInTime) + "\n"
        out += "      secsRemaining: " + str(self.secsRemaining) + "\n"
        out += "      secondaryTime: " + str(self.secondaryTime) + "\n"

        return out

    def getGameType(self):
        if self.gameType == self.GAME_ROUNDROBIN:
            return "round robin"
        elif self.gameType == self.GAME_PLAYOFF:
            return "playoff"
        elif self.gameType == self.GAME_MIXEDTEAM_ROUNDROBIN:
            return "mixed team round robin"
        elif self.gameType == self.GAME_MIXEDTEAM_PLAYOFF:
            return "mixed team playoff"
        return "undefinied(" + str(self.gameType) + ")"

    def getGameState(self):
        if self.gameState == self.STATE_INITIAL:
            return "initial"
        elif self.gameState == self.STATE_READY:
            return "ready"
        elif self.gameState == self.STATE_SET:
            return "set"
        elif self.gameState == self.STATE_PLAYING:
            return "playing"
        elif self.gameState == self.STATE_FINISHED:
            return "finish"
        return "undefinied(" + str(self.gameState) + ")"

    def getSecGameState(self):
        if self.secGameState == self.STATE2_NORMAL:
            return "normal"
        elif self.secGameState == self.STATE2_PENALTYSHOOT:
            return "penaltyshoot"
        elif self.secGameState == self.STATE2_OVERTIME:
            return "overtime"
        elif self.secGameState == self.STATE2_TIMEOUT:
            return "timeout"
        else:
            return "undefinied(" + str(self.secGameState) + ")"
