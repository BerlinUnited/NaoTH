"""Representation of the SPL standard message."""
from struct import Struct as _Struct

from .pb import TeamMessage_pb2 as _TM
from .math import Vector2 as _v2, Pose2D as _p2


class MixedTeamMessage(_Struct):

    def __init__(self, data=None):
        """Constructor."""
        super(MixedTeamMessage, self).__init__('Q4B')
        # based on the given data the message is initialized with default values or parsed from the data string
        if data is not None:
            self.unpack(data)
        else:
            self.setDefaults()

    def setDefaults(self):
        self.timestamp = 0
        self.teamID = 4
        self.isPenalized = 0
        self.whistleDetected = 0
        self.dummy = 42

    def unpack(self, data):
        # check 'data' length
        if len(data) < self.size:
            raise Exception("Invalid data size for MixedTeamMessage")

        msg = _Struct.unpack(self, data[:self.size])
        it = iter(msg)
        self.timestamp = next(it)
        self.teamID = next(it)
        self.isPenalized = next(it)
        self.whistleDetected = next(it)
        self.dummy = next(it)

    def pack(self):
        return _Struct.pack(self,
                           self.timestamp,
                           self.teamID,
                           self.isPenalized,
                           self.whistleDetected,
                           self.dummy
        )

    def __str__(self):
        return str(self.__dict__)


class SPLMessage(_Struct):
    """Representation of the standard SPLMessage."""

    SPL_STANDARD_MESSAGE_STRUCT_HEADER = b'SPL '
    SPL_STANDARD_MESSAGE_STRUCT_VERSION = 7
    SPL_STANDARD_MESSAGE_DATA_SIZE = 474
    SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS = 6

    def __init__(self, teamnumber:int=0, playernumber:int=0, data=None):
        """Constructor."""
        # 4s    header +
        # 3b    version + playernumber + teamnumber
        # ?     fallen
        # 6f    pose_x, pose_y, pose_r, ballage, ball_x, ball_y
        # h     size
        super(SPLMessage, self).__init__('4s3b?6fh')
        # based on the given data the message is initialized with default values or parsed from the data string
        if data is not None:
            self.unpack(data)
        else:
            self.setDefaults(teamnumber, playernumber)

    def setDefaults(self, teamnumber=0, playernumber=0):

        self.playerNumber = playernumber
        self.teamNumber = teamnumber
        self.fallen = False
        self.pose = _p2(_v2(0.0, 0.0), 0.0)  # x, y, r | +/-4500, +/-3000
        #self.walkingTo = _v2(0.0, 0.0)
        #self.shootingTo = _v2(0.0, 0.0)
        self.ballAge = -1
        self.ballPosition = _v2(0.0, 0.0)
        #self.ballVelocity = _v2(0.0, 0.0)
        #self.suggestion = [0 for x in range(self.SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS)]
        #self.intention = 0
        #self.averageWalkSpeed = 200  # see TeamCommSender
        #self.maxKickDistance = 3000  # see TeamCommSender
        #self.currentPositionConfidence = 100
        #self.currentSideConfidence = 100

        self._mixed = MixedTeamMessage()

        self.data = _TM.BUUserTeamMessage()

        # set known default values of custom message part
        for field in self.data.DESCRIPTOR.fields:
            if field.has_default_value:
                setattr(self.data, field.name, field.default_value)

        self.numOfDataBytes = self.data.ByteSize() + self._mixed.size

    def pack(self):

        return _Struct.pack(self,
                           self.SPL_STANDARD_MESSAGE_STRUCT_HEADER,
                           self.SPL_STANDARD_MESSAGE_STRUCT_VERSION,
                           self.playerNumber,
                           self.teamNumber,
                           self.fallen,
                           self.pose.translation.x, self.pose.translation.y, self.pose.rotation,
                           #*self.walkingTo.__dict__.values(),
                           #*self.shootingTo.__dict__.values(),
                           self.ballAge,
                           #*self.ballPosition.__dict__.values(),
                           self.ballPosition.x,
                           self.ballPosition.y,
                           #*self.ballVelocity.__dict__.values(),
                           #*self.suggestion,
                           #self.intention,
                           #self.averageWalkSpeed,
                           #self.maxKickDistance,
                           #self.currentPositionConfidence,
                           #self.currentSideConfidence,
                           (self.data.ByteSize() + self._mixed.size)
                           ) + self._mixed.pack()\
                             + self.data.SerializeToString()

    def unpack(self, data):
        # check 'data' length
        if len(data) < self.size:
            raise ErrorNotAnSplMessage("Not an SPL Message.")

        msg = _Struct.unpack(self, data[:self.size])

        # check header
        if msg[0] != self.SPL_STANDARD_MESSAGE_STRUCT_HEADER:
            raise ErrorInvalidSplHeader("Invalid header! (" + msg[0].decode("utf-8") + ")")

        # check spl message version
        if msg[1] != self.SPL_STANDARD_MESSAGE_STRUCT_VERSION:
            raise ErrorWrongSplMessageVersion("Wrong version: received " + str(msg[1]) + ", but expected " + str(self.SPL_STANDARD_MESSAGE_STRUCT_VERSION))

        # assign data
        it = iter(msg[2:])
        self.playerNumber = next(it)
        self.teamNumber = next(it)
        self.fallen = next(it)
        self.pose = _p2( _v2(next(it), next(it)), next(it) )
        #self.walkingTo = _v2( next(it), next(it) )
        #self.shootingTo = _v2( next(it), next(it) )
        self.ballAge = next(it)
        self.ballPosition = _v2( next(it), next(it) )
        #self.ballVelocity = _v2( next(it), next(it) )
        #self.suggestion = [ next(it) for i in range(self.SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS) ]
        #self.intention = next(it)
        #self.averageWalkSpeed = next(it)
        #self.maxKickDistance = next(it)
        #self.currentPositionConfidence = next(it)
        #self.currentSideConfidence = next(it)
        self.numOfDataBytes = next(it)
        self.data = data[self.size:self.size+self.numOfDataBytes]

        # unpack mixed team part
        self._mixed = MixedTeamMessage()
        self._mixed.unpack(self.data[:self._mixed.size])

        # check if we got a 'valid' mixed team message and adjust data offset
        offset = self._mixed.size if self._mixed.teamID == 4 else 0

        custom = None
        try:
            custom = _TM.BUUserTeamMessage()
            custom.ParseFromString(data[self.size+offset:self.size+offset+self.numOfDataBytes])
        except:
            # if we can't parse custom data - it is not our message
            pass
        finally:
            # if its our custom data ...
            if custom is not None and custom.ByteSize() > 0 and (custom.HasField('key') and custom.key == "naoth"):
                self.data = custom


    def parseFromLogJson(self, data):
        self.__map_json_data('playerNum', 'playerNumber', data)
        self.__map_json_data('teamNum', 'teamNumber', data)
        self.__map_json_data('fallen', 'fallen', data)
        self.__map_json_data((self.pose,'x'), 'pose_x', data)
        self.__map_json_data((self.pose,'y'), 'pose_y', data)
        self.__map_json_data((self.pose,'r'), 'pose_a', data)
        self.__map_json_data((self.walkingTo,'x'), 'walkingTo_x', data)
        self.__map_json_data((self.walkingTo,'y'), 'walkingTo_y', data)
        self.__map_json_data((self.shootingTo,'x'), 'shootingTo_x', data)
        self.__map_json_data((self.shootingTo,'y'), 'shootingTo_y', data)
        self.__map_json_data('ballAge', 'ballAge', data)
        self.__map_json_data((self.ball,'x'), 'ball_x', data)
        self.__map_json_data((self.ball,'y'), 'ball_y', data)
        self.__map_json_data((self.ballVel,'x'), 'ballVel_x', data)
        self.__map_json_data((self.ballVel,'y'), 'ballVel_y', data)
        self.__map_json_data('suggestion', 'suggestion', data)
        self.__map_json_data('intention', 'intention', data)
        self.__map_json_data('averageWalkSpeed', 'averageWalkSpeed', data)
        self.__map_json_data('maxKickDistance', 'maxKickDistance', data)
        self.__map_json_data('currentPositionConfidence', 'currentPositionConfidence', data)
        self.__map_json_data('currentSideConfidence', 'currentSideConfidence', data)
        self.__map_json_data('numOfDataBytes', 'numOfDataBytes', data)
        self.__map_json_data('data', 'data', data, lambda x: bytes(list(map(lambda i: -128*(i//128)+(i%128),x))))

    def __map_json_data(self, var, key, data, func=lambda x: x):
        if key in data:
            if type(var) == tuple:
                var[0].__dict__[var[1]] = func(data[key])
            else:
                self.__dict__[var] = func(data[key])

    def __repr__(self):
        """Returns all 'active' message fields as string."""
        result = ""
        for attr in self.__dict__:
            #print(attr, isinstance(self.__dict__[attr], bytes), self.__dict__[attr])
            if attr == "data" and not isinstance(self.__dict__[attr], bytes):
                fields = self.__dict__[attr].DESCRIPTOR.fields_by_name
                for custom_attr in fields:
                    # do not show deprecated custom fields!
                    if fields[custom_attr].GetOptions().deprecated:
                        continue
                    result += "\t" + custom_attr + " = " + str(getattr(self.__dict__[attr], custom_attr)) + "\n"
            elif attr.startswith("_"):
                # skip attributes starting with '_' (private)
                continue
            else:
                result += "\t" + attr + " = " + str(self.__dict__[attr]) + "\n"
        return result


class ErrorNotAnSplMessage(Exception):
    """Exception, if given data is not an SPL message."""
    pass


class ErrorWrongSplMessageVersion(Exception):
    """Wrong spl message version exception."""
    pass


class ErrorInvalidSplHeader(Exception):
    """Wrong spl message header exception."""
    pass
