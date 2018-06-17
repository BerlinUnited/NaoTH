# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: TeamMessage.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf.internal import enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import CommonTypes_pb2 as CommonTypes__pb2
import Framework_Representations_pb2 as Framework__Representations__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='TeamMessage.proto',
  package='naothmessages',
  syntax='proto2',
  serialized_pb=_b('\n\x11TeamMessage.proto\x12\rnaothmessages\x1a\x11\x43ommonTypes.proto\x1a\x1f\x46ramework-Representations.proto\"L\n\x08Opponent\x12\x14\n\tplayerNum\x18\x01 \x02(\x05:\x01\x30\x12*\n\x0bposeOnField\x18\x02 \x01(\x0b\x32\x15.naothmessages.Pose2D\"A\n\x03Ntp\x12\x14\n\tplayerNum\x18\x01 \x02(\x05:\x01\x30\x12\x0f\n\x04sent\x18\x02 \x01(\x04:\x01\x30\x12\x13\n\x08received\x18\x03 \x01(\x04:\x01\x30\"\x8e\x01\n\x05\x44rops\x12\x1b\n\x10\x64ropNoSplMessage\x18\x01 \x01(\r:\x01\x30\x12\x19\n\x0e\x64ropNotOurTeam\x18\x02 \x01(\r:\x01\x30\x12\x1b\n\x10\x64ropNotParseable\x18\x03 \x01(\r:\x01\x30\x12\x16\n\x0b\x64ropKeyFail\x18\x04 \x01(\r:\x01\x30\x12\x18\n\rdropMonotonic\x18\x05 \x01(\r:\x01\x30\"\x9a\x04\n\x11\x42UUserTeamMessage\x12\x17\n\x06\x62odyID\x18\x01 \x01(\t:\x07unknown\x12\x12\n\ntimeToBall\x18\x02 \x01(\r\x12\x19\n\nwasStriker\x18\x03 \x01(\x08:\x05\x66\x61lse\x12\x1a\n\x0bisPenalized\x18\x04 \x01(\x08:\x05\x66\x61lse\x12*\n\topponents\x18\x05 \x03(\x0b\x32\x17.naothmessages.Opponent\x12\x19\n\nteamNumber\x18\x06 \x01(\r:\x01\x30\x42\x02\x18\x01\x12\x18\n\rbatteryCharge\x18\x07 \x01(\x02:\x01\x31\x12\x16\n\x0btemperature\x18\x08 \x01(\x02:\x01\x30\x12\x14\n\ttimestamp\x18\t \x01(\x04:\x01\x30\x12\x1f\n\x10wantsToBeStriker\x18\n \x01(\x08:\x05\x66\x61lse\x12\x19\n\x0e\x63puTemperature\x18\x0b \x01(\x02:\x01\x30\x12\x1e\n\x0fwhistleDetected\x18\x0c \x01(\x08:\x05\x66\x61lse\x12\x17\n\x0cwhistleCount\x18\r \x01(\x05:\x01\x30\x12.\n\x08teamBall\x18\x0e \x01(\x0b\x32\x1c.naothmessages.DoubleVector2\x12&\n\nntpRequest\x18\x0f \x03(\x0b\x32\x12.naothmessages.Ntp\x12\x32\n\x0c\x62\x61llVelocity\x18\x10 \x01(\x0b\x32\x1c.naothmessages.DoubleVector2\x12\x11\n\x03key\x18\x64 \x01(\t:\x04none\"\xee\x03\n\x0bTeamMessage\x12-\n\x04\x64\x61ta\x18\x01 \x03(\x0b\x32\x1f.naothmessages.TeamMessage.Data\x12)\n\x0bmessageDrop\x18\x02 \x01(\x0b\x32\x14.naothmessages.Drops\x1a\x84\x03\n\x04\x44\x61ta\x12\x14\n\tplayerNum\x18\x01 \x01(\r:\x01\x30\x12\x39\n\tteamColor\x18\x0b \x01(\x0e\x32\x18.naothmessages.TeamColor:\x08\x62lueTeamB\x02\x18\x01\x12#\n\x04pose\x18\x03 \x01(\x0b\x32\x15.naothmessages.Pose2D\x12\x13\n\x07\x62\x61llAge\x18\x04 \x01(\x05:\x02-1\x12\x32\n\x0c\x62\x61llPosition\x18\x05 \x01(\x0b\x32\x1c.naothmessages.DoubleVector2\x12\x32\n\x0c\x62\x61llVelocity\x18\x06 \x01(\x0b\x32\x1c.naothmessages.DoubleVector2\x12\x15\n\x06\x66\x61llen\x18\n \x01(\x08:\x05\x66\x61lse\x12.\n\x04user\x18\x08 \x01(\x0b\x32 .naothmessages.BUUserTeamMessage\x12+\n\tframeInfo\x18\t \x01(\x0b\x32\x18.naothmessages.FrameInfo\x12\x15\n\nteamNumber\x18\x0c \x01(\r:\x01\x30*\xb2\x01\n\tTeamColor\x12\x0c\n\x08\x62lueTeam\x10\x00\x12\x0b\n\x07redTeam\x10\x01\x12\x0e\n\nyellowTeam\x10\x02\x12\r\n\tblackTeam\x10\x03\x12\r\n\twhiteTeam\x10\x04\x12\r\n\tgreenTeam\x10\x05\x12\x0e\n\norangeTeam\x10\x06\x12\x0e\n\npurpleTeam\x10\x07\x12\r\n\tbrownTeam\x10\x08\x12\x0c\n\x08grayTeam\x10\t\x12\x10\n\x0binvalidTeam\x10\xff\x01\x42\x16\n\x14\x64\x65.naoth.rc.messages')
  ,
  dependencies=[CommonTypes__pb2.DESCRIPTOR,Framework__Representations__pb2.DESCRIPTOR,])

_TEAMCOLOR = _descriptor.EnumDescriptor(
  name='TeamColor',
  full_name='naothmessages.TeamColor',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='blueTeam', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='redTeam', index=1, number=1,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='yellowTeam', index=2, number=2,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='blackTeam', index=3, number=3,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='whiteTeam', index=4, number=4,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='greenTeam', index=5, number=5,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='orangeTeam', index=6, number=6,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='purpleTeam', index=7, number=7,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='brownTeam', index=8, number=8,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='grayTeam', index=9, number=9,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='invalidTeam', index=10, number=255,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=1417,
  serialized_end=1595,
)
_sym_db.RegisterEnumDescriptor(_TEAMCOLOR)

TeamColor = enum_type_wrapper.EnumTypeWrapper(_TEAMCOLOR)
blueTeam = 0
redTeam = 1
yellowTeam = 2
blackTeam = 3
whiteTeam = 4
greenTeam = 5
orangeTeam = 6
purpleTeam = 7
brownTeam = 8
grayTeam = 9
invalidTeam = 255



_OPPONENT = _descriptor.Descriptor(
  name='Opponent',
  full_name='naothmessages.Opponent',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='playerNum', full_name='naothmessages.Opponent.playerNum', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='poseOnField', full_name='naothmessages.Opponent.poseOnField', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=88,
  serialized_end=164,
)


_NTP = _descriptor.Descriptor(
  name='Ntp',
  full_name='naothmessages.Ntp',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='playerNum', full_name='naothmessages.Ntp.playerNum', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='sent', full_name='naothmessages.Ntp.sent', index=1,
      number=2, type=4, cpp_type=4, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='received', full_name='naothmessages.Ntp.received', index=2,
      number=3, type=4, cpp_type=4, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=166,
  serialized_end=231,
)


_DROPS = _descriptor.Descriptor(
  name='Drops',
  full_name='naothmessages.Drops',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='dropNoSplMessage', full_name='naothmessages.Drops.dropNoSplMessage', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='dropNotOurTeam', full_name='naothmessages.Drops.dropNotOurTeam', index=1,
      number=2, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='dropNotParseable', full_name='naothmessages.Drops.dropNotParseable', index=2,
      number=3, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='dropKeyFail', full_name='naothmessages.Drops.dropKeyFail', index=3,
      number=4, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='dropMonotonic', full_name='naothmessages.Drops.dropMonotonic', index=4,
      number=5, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=234,
  serialized_end=376,
)


_BUUSERTEAMMESSAGE = _descriptor.Descriptor(
  name='BUUserTeamMessage',
  full_name='naothmessages.BUUserTeamMessage',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='bodyID', full_name='naothmessages.BUUserTeamMessage.bodyID', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=True, default_value=_b("unknown").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='timeToBall', full_name='naothmessages.BUUserTeamMessage.timeToBall', index=1,
      number=2, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='wasStriker', full_name='naothmessages.BUUserTeamMessage.wasStriker', index=2,
      number=3, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='isPenalized', full_name='naothmessages.BUUserTeamMessage.isPenalized', index=3,
      number=4, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='opponents', full_name='naothmessages.BUUserTeamMessage.opponents', index=4,
      number=5, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='teamNumber', full_name='naothmessages.BUUserTeamMessage.teamNumber', index=5,
      number=6, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=_descriptor._ParseOptions(descriptor_pb2.FieldOptions(), _b('\030\001'))),
    _descriptor.FieldDescriptor(
      name='batteryCharge', full_name='naothmessages.BUUserTeamMessage.batteryCharge', index=6,
      number=7, type=2, cpp_type=6, label=1,
      has_default_value=True, default_value=float(1),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='temperature', full_name='naothmessages.BUUserTeamMessage.temperature', index=7,
      number=8, type=2, cpp_type=6, label=1,
      has_default_value=True, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='timestamp', full_name='naothmessages.BUUserTeamMessage.timestamp', index=8,
      number=9, type=4, cpp_type=4, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='wantsToBeStriker', full_name='naothmessages.BUUserTeamMessage.wantsToBeStriker', index=9,
      number=10, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='cpuTemperature', full_name='naothmessages.BUUserTeamMessage.cpuTemperature', index=10,
      number=11, type=2, cpp_type=6, label=1,
      has_default_value=True, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='whistleDetected', full_name='naothmessages.BUUserTeamMessage.whistleDetected', index=11,
      number=12, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='whistleCount', full_name='naothmessages.BUUserTeamMessage.whistleCount', index=12,
      number=13, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='teamBall', full_name='naothmessages.BUUserTeamMessage.teamBall', index=13,
      number=14, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='ntpRequest', full_name='naothmessages.BUUserTeamMessage.ntpRequest', index=14,
      number=15, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='ballVelocity', full_name='naothmessages.BUUserTeamMessage.ballVelocity', index=15,
      number=16, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='key', full_name='naothmessages.BUUserTeamMessage.key', index=16,
      number=100, type=9, cpp_type=9, label=1,
      has_default_value=True, default_value=_b("none").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=379,
  serialized_end=917,
)


_TEAMMESSAGE_DATA = _descriptor.Descriptor(
  name='Data',
  full_name='naothmessages.TeamMessage.Data',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='playerNum', full_name='naothmessages.TeamMessage.Data.playerNum', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='teamColor', full_name='naothmessages.TeamMessage.Data.teamColor', index=1,
      number=11, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=_descriptor._ParseOptions(descriptor_pb2.FieldOptions(), _b('\030\001'))),
    _descriptor.FieldDescriptor(
      name='pose', full_name='naothmessages.TeamMessage.Data.pose', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='ballAge', full_name='naothmessages.TeamMessage.Data.ballAge', index=3,
      number=4, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=-1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='ballPosition', full_name='naothmessages.TeamMessage.Data.ballPosition', index=4,
      number=5, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='ballVelocity', full_name='naothmessages.TeamMessage.Data.ballVelocity', index=5,
      number=6, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='fallen', full_name='naothmessages.TeamMessage.Data.fallen', index=6,
      number=10, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='user', full_name='naothmessages.TeamMessage.Data.user', index=7,
      number=8, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='frameInfo', full_name='naothmessages.TeamMessage.Data.frameInfo', index=8,
      number=9, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='teamNumber', full_name='naothmessages.TeamMessage.Data.teamNumber', index=9,
      number=12, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=1026,
  serialized_end=1414,
)

_TEAMMESSAGE = _descriptor.Descriptor(
  name='TeamMessage',
  full_name='naothmessages.TeamMessage',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='data', full_name='naothmessages.TeamMessage.data', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='messageDrop', full_name='naothmessages.TeamMessage.messageDrop', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[_TEAMMESSAGE_DATA, ],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=920,
  serialized_end=1414,
)

_OPPONENT.fields_by_name['poseOnField'].message_type = CommonTypes__pb2._POSE2D
_BUUSERTEAMMESSAGE.fields_by_name['opponents'].message_type = _OPPONENT
_BUUSERTEAMMESSAGE.fields_by_name['teamBall'].message_type = CommonTypes__pb2._DOUBLEVECTOR2
_BUUSERTEAMMESSAGE.fields_by_name['ntpRequest'].message_type = _NTP
_BUUSERTEAMMESSAGE.fields_by_name['ballVelocity'].message_type = CommonTypes__pb2._DOUBLEVECTOR2
_TEAMMESSAGE_DATA.fields_by_name['teamColor'].enum_type = _TEAMCOLOR
_TEAMMESSAGE_DATA.fields_by_name['pose'].message_type = CommonTypes__pb2._POSE2D
_TEAMMESSAGE_DATA.fields_by_name['ballPosition'].message_type = CommonTypes__pb2._DOUBLEVECTOR2
_TEAMMESSAGE_DATA.fields_by_name['ballVelocity'].message_type = CommonTypes__pb2._DOUBLEVECTOR2
_TEAMMESSAGE_DATA.fields_by_name['user'].message_type = _BUUSERTEAMMESSAGE
_TEAMMESSAGE_DATA.fields_by_name['frameInfo'].message_type = Framework__Representations__pb2._FRAMEINFO
_TEAMMESSAGE_DATA.containing_type = _TEAMMESSAGE
_TEAMMESSAGE.fields_by_name['data'].message_type = _TEAMMESSAGE_DATA
_TEAMMESSAGE.fields_by_name['messageDrop'].message_type = _DROPS
DESCRIPTOR.message_types_by_name['Opponent'] = _OPPONENT
DESCRIPTOR.message_types_by_name['Ntp'] = _NTP
DESCRIPTOR.message_types_by_name['Drops'] = _DROPS
DESCRIPTOR.message_types_by_name['BUUserTeamMessage'] = _BUUSERTEAMMESSAGE
DESCRIPTOR.message_types_by_name['TeamMessage'] = _TEAMMESSAGE
DESCRIPTOR.enum_types_by_name['TeamColor'] = _TEAMCOLOR
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

Opponent = _reflection.GeneratedProtocolMessageType('Opponent', (_message.Message,), dict(
  DESCRIPTOR = _OPPONENT,
  __module__ = 'TeamMessage_pb2'
  # @@protoc_insertion_point(class_scope:naothmessages.Opponent)
  ))
_sym_db.RegisterMessage(Opponent)

Ntp = _reflection.GeneratedProtocolMessageType('Ntp', (_message.Message,), dict(
  DESCRIPTOR = _NTP,
  __module__ = 'TeamMessage_pb2'
  # @@protoc_insertion_point(class_scope:naothmessages.Ntp)
  ))
_sym_db.RegisterMessage(Ntp)

Drops = _reflection.GeneratedProtocolMessageType('Drops', (_message.Message,), dict(
  DESCRIPTOR = _DROPS,
  __module__ = 'TeamMessage_pb2'
  # @@protoc_insertion_point(class_scope:naothmessages.Drops)
  ))
_sym_db.RegisterMessage(Drops)

BUUserTeamMessage = _reflection.GeneratedProtocolMessageType('BUUserTeamMessage', (_message.Message,), dict(
  DESCRIPTOR = _BUUSERTEAMMESSAGE,
  __module__ = 'TeamMessage_pb2'
  # @@protoc_insertion_point(class_scope:naothmessages.BUUserTeamMessage)
  ))
_sym_db.RegisterMessage(BUUserTeamMessage)

TeamMessage = _reflection.GeneratedProtocolMessageType('TeamMessage', (_message.Message,), dict(

  Data = _reflection.GeneratedProtocolMessageType('Data', (_message.Message,), dict(
    DESCRIPTOR = _TEAMMESSAGE_DATA,
    __module__ = 'TeamMessage_pb2'
    # @@protoc_insertion_point(class_scope:naothmessages.TeamMessage.Data)
    ))
  ,
  DESCRIPTOR = _TEAMMESSAGE,
  __module__ = 'TeamMessage_pb2'
  # @@protoc_insertion_point(class_scope:naothmessages.TeamMessage)
  ))
_sym_db.RegisterMessage(TeamMessage)
_sym_db.RegisterMessage(TeamMessage.Data)


DESCRIPTOR.has_options = True
DESCRIPTOR._options = _descriptor._ParseOptions(descriptor_pb2.FileOptions(), _b('\n\024de.naoth.rc.messages'))
_BUUSERTEAMMESSAGE.fields_by_name['teamNumber'].has_options = True
_BUUSERTEAMMESSAGE.fields_by_name['teamNumber']._options = _descriptor._ParseOptions(descriptor_pb2.FieldOptions(), _b('\030\001'))
_TEAMMESSAGE_DATA.fields_by_name['teamColor'].has_options = True
_TEAMMESSAGE_DATA.fields_by_name['teamColor']._options = _descriptor._ParseOptions(descriptor_pb2.FieldOptions(), _b('\030\001'))
# @@protoc_insertion_point(module_scope)
