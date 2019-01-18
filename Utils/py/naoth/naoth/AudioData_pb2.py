# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: AudioData.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='AudioData.proto',
  package='naothmessages',
  syntax='proto2',
  serialized_pb=_b('\n\x0f\x41udioData.proto\x12\rnaothmessages\"l\n\tAudioData\x12\x0f\n\x07samples\x18\x01 \x02(\x0c\x12\x12\n\nsampleSize\x18\x02 \x02(\r\x12\x12\n\nsampleRate\x18\x03 \x02(\r\x12\x13\n\x0bnumChannels\x18\x04 \x02(\r\x12\x11\n\ttimestamp\x18\x05 \x02(\rB\x16\n\x14\x64\x65.naoth.rc.messages')
)




_AUDIODATA = _descriptor.Descriptor(
  name='AudioData',
  full_name='naothmessages.AudioData',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='samples', full_name='naothmessages.AudioData.samples', index=0,
      number=1, type=12, cpp_type=9, label=2,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='sampleSize', full_name='naothmessages.AudioData.sampleSize', index=1,
      number=2, type=13, cpp_type=3, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='sampleRate', full_name='naothmessages.AudioData.sampleRate', index=2,
      number=3, type=13, cpp_type=3, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='numChannels', full_name='naothmessages.AudioData.numChannels', index=3,
      number=4, type=13, cpp_type=3, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='timestamp', full_name='naothmessages.AudioData.timestamp', index=4,
      number=5, type=13, cpp_type=3, label=2,
      has_default_value=False, default_value=0,
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
  serialized_start=34,
  serialized_end=142,
)

DESCRIPTOR.message_types_by_name['AudioData'] = _AUDIODATA
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

AudioData = _reflection.GeneratedProtocolMessageType('AudioData', (_message.Message,), dict(
  DESCRIPTOR = _AUDIODATA,
  __module__ = 'AudioData_pb2'
  # @@protoc_insertion_point(class_scope:naothmessages.AudioData)
  ))
_sym_db.RegisterMessage(AudioData)


DESCRIPTOR.has_options = True
DESCRIPTOR._options = _descriptor._ParseOptions(descriptor_pb2.FileOptions(), _b('\n\024de.naoth.rc.messages'))
# @@protoc_insertion_point(module_scope)
