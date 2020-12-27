// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: RobotPose.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "RobotPose.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace naothmessages {
class RobotPoseDefaultTypeInternal {
public:
 ::google::protobuf::internal::ExplicitlyConstructed<RobotPose>
     _instance;
} _RobotPose_default_instance_;

namespace protobuf_RobotPose_2eproto {


namespace {

::google::protobuf::Metadata file_level_metadata[1];

}  // namespace

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTableField
    const TableStruct::entries[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  {0, 0, 0, ::google::protobuf::internal::kInvalidMask, 0, 0},
};

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::AuxillaryParseTableField
    const TableStruct::aux[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ::google::protobuf::internal::AuxillaryParseTableField(),
};
PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTable const
    TableStruct::schema[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { NULL, NULL, 0, -1, -1, -1, -1, NULL, false },
};

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RobotPose, _has_bits_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RobotPose, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RobotPose, pose_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RobotPose, isvalid_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RobotPose, principleaxismajor_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RobotPose, principleaxisminor_),
  0,
  3,
  1,
  2,
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 9, sizeof(RobotPose)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&_RobotPose_default_instance_),
};

namespace {

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "RobotPose.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

}  // namespace
void TableStruct::InitDefaultsImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::internal::InitProtobufDefaults();
  ::naothmessages::protobuf_CommonTypes_2eproto::InitDefaults();
  _RobotPose_default_instance_._instance.DefaultConstruct();
  ::google::protobuf::internal::OnShutdownDestroyMessage(
      &_RobotPose_default_instance_);_RobotPose_default_instance_._instance.get_mutable()->pose_ = const_cast< ::naothmessages::Pose2D*>(
      ::naothmessages::Pose2D::internal_default_instance());
  _RobotPose_default_instance_._instance.get_mutable()->principleaxismajor_ = const_cast< ::naothmessages::DoubleVector2*>(
      ::naothmessages::DoubleVector2::internal_default_instance());
  _RobotPose_default_instance_._instance.get_mutable()->principleaxisminor_ = const_cast< ::naothmessages::DoubleVector2*>(
      ::naothmessages::DoubleVector2::internal_default_instance());
}

void InitDefaults() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &TableStruct::InitDefaultsImpl);
}
namespace {
void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\017RobotPose.proto\022\rnaothmessages\032\021Common"
      "Types.proto\"\265\001\n\tRobotPose\022#\n\004pose\030\001 \002(\0132"
      "\025.naothmessages.Pose2D\022\017\n\007isValid\030\002 \001(\010\022"
      "8\n\022principleAxisMajor\030\003 \001(\0132\034.naothmessa"
      "ges.DoubleVector2\0228\n\022principleAxisMinor\030"
      "\004 \001(\0132\034.naothmessages.DoubleVector2B\033\n\031d"
      "e.naoth.rc.core.messages"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 264);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "RobotPose.proto", &protobuf_RegisterTypes);
  ::naothmessages::protobuf_CommonTypes_2eproto::AddDescriptors();
}
} // anonymous namespace

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;

}  // namespace protobuf_RobotPose_2eproto


// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int RobotPose::kPoseFieldNumber;
const int RobotPose::kIsValidFieldNumber;
const int RobotPose::kPrincipleAxisMajorFieldNumber;
const int RobotPose::kPrincipleAxisMinorFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

RobotPose::RobotPose()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    protobuf_RobotPose_2eproto::InitDefaults();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:naothmessages.RobotPose)
}
RobotPose::RobotPose(const RobotPose& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _has_bits_(from._has_bits_),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  if (from.has_pose()) {
    pose_ = new ::naothmessages::Pose2D(*from.pose_);
  } else {
    pose_ = NULL;
  }
  if (from.has_principleaxismajor()) {
    principleaxismajor_ = new ::naothmessages::DoubleVector2(*from.principleaxismajor_);
  } else {
    principleaxismajor_ = NULL;
  }
  if (from.has_principleaxisminor()) {
    principleaxisminor_ = new ::naothmessages::DoubleVector2(*from.principleaxisminor_);
  } else {
    principleaxisminor_ = NULL;
  }
  isvalid_ = from.isvalid_;
  // @@protoc_insertion_point(copy_constructor:naothmessages.RobotPose)
}

void RobotPose::SharedCtor() {
  _cached_size_ = 0;
  ::memset(&pose_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&isvalid_) -
      reinterpret_cast<char*>(&pose_)) + sizeof(isvalid_));
}

RobotPose::~RobotPose() {
  // @@protoc_insertion_point(destructor:naothmessages.RobotPose)
  SharedDtor();
}

void RobotPose::SharedDtor() {
  if (this != internal_default_instance()) delete pose_;
  if (this != internal_default_instance()) delete principleaxismajor_;
  if (this != internal_default_instance()) delete principleaxisminor_;
}

void RobotPose::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* RobotPose::descriptor() {
  protobuf_RobotPose_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_RobotPose_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const RobotPose& RobotPose::default_instance() {
  protobuf_RobotPose_2eproto::InitDefaults();
  return *internal_default_instance();
}

RobotPose* RobotPose::New(::google::protobuf::Arena* arena) const {
  RobotPose* n = new RobotPose;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void RobotPose::Clear() {
// @@protoc_insertion_point(message_clear_start:naothmessages.RobotPose)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 7u) {
    if (cached_has_bits & 0x00000001u) {
      GOOGLE_DCHECK(pose_ != NULL);
      pose_->::naothmessages::Pose2D::Clear();
    }
    if (cached_has_bits & 0x00000002u) {
      GOOGLE_DCHECK(principleaxismajor_ != NULL);
      principleaxismajor_->::naothmessages::DoubleVector2::Clear();
    }
    if (cached_has_bits & 0x00000004u) {
      GOOGLE_DCHECK(principleaxisminor_ != NULL);
      principleaxisminor_->::naothmessages::DoubleVector2::Clear();
    }
  }
  isvalid_ = false;
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

bool RobotPose::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:naothmessages.RobotPose)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .naothmessages.Pose2D pose = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u /* 10 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_pose()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // optional bool isValid = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(16u /* 16 & 0xFF */)) {
          set_has_isvalid();
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &isvalid_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // optional .naothmessages.DoubleVector2 principleAxisMajor = 3;
      case 3: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(26u /* 26 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_principleaxismajor()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // optional .naothmessages.DoubleVector2 principleAxisMinor = 4;
      case 4: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(34u /* 34 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_principleaxisminor()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:naothmessages.RobotPose)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:naothmessages.RobotPose)
  return false;
#undef DO_
}

void RobotPose::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:naothmessages.RobotPose)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required .naothmessages.Pose2D pose = 1;
  if (cached_has_bits & 0x00000001u) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, *this->pose_, output);
  }

  // optional bool isValid = 2;
  if (cached_has_bits & 0x00000008u) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(2, this->isvalid(), output);
  }

  // optional .naothmessages.DoubleVector2 principleAxisMajor = 3;
  if (cached_has_bits & 0x00000002u) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      3, *this->principleaxismajor_, output);
  }

  // optional .naothmessages.DoubleVector2 principleAxisMinor = 4;
  if (cached_has_bits & 0x00000004u) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      4, *this->principleaxisminor_, output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:naothmessages.RobotPose)
}

::google::protobuf::uint8* RobotPose::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:naothmessages.RobotPose)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required .naothmessages.Pose2D pose = 1;
  if (cached_has_bits & 0x00000001u) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageNoVirtualToArray(
        1, *this->pose_, deterministic, target);
  }

  // optional bool isValid = 2;
  if (cached_has_bits & 0x00000008u) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(2, this->isvalid(), target);
  }

  // optional .naothmessages.DoubleVector2 principleAxisMajor = 3;
  if (cached_has_bits & 0x00000002u) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageNoVirtualToArray(
        3, *this->principleaxismajor_, deterministic, target);
  }

  // optional .naothmessages.DoubleVector2 principleAxisMinor = 4;
  if (cached_has_bits & 0x00000004u) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageNoVirtualToArray(
        4, *this->principleaxisminor_, deterministic, target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:naothmessages.RobotPose)
  return target;
}

size_t RobotPose::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:naothmessages.RobotPose)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  // required .naothmessages.Pose2D pose = 1;
  if (has_pose()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->pose_);
  }
  if (_has_bits_[0 / 32] & 14u) {
    // optional .naothmessages.DoubleVector2 principleAxisMajor = 3;
    if (has_principleaxismajor()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          *this->principleaxismajor_);
    }

    // optional .naothmessages.DoubleVector2 principleAxisMinor = 4;
    if (has_principleaxisminor()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          *this->principleaxisminor_);
    }

    // optional bool isValid = 2;
    if (has_isvalid()) {
      total_size += 1 + 1;
    }

  }
  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void RobotPose::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:naothmessages.RobotPose)
  GOOGLE_DCHECK_NE(&from, this);
  const RobotPose* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const RobotPose>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:naothmessages.RobotPose)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:naothmessages.RobotPose)
    MergeFrom(*source);
  }
}

void RobotPose::MergeFrom(const RobotPose& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:naothmessages.RobotPose)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 15u) {
    if (cached_has_bits & 0x00000001u) {
      mutable_pose()->::naothmessages::Pose2D::MergeFrom(from.pose());
    }
    if (cached_has_bits & 0x00000002u) {
      mutable_principleaxismajor()->::naothmessages::DoubleVector2::MergeFrom(from.principleaxismajor());
    }
    if (cached_has_bits & 0x00000004u) {
      mutable_principleaxisminor()->::naothmessages::DoubleVector2::MergeFrom(from.principleaxisminor());
    }
    if (cached_has_bits & 0x00000008u) {
      isvalid_ = from.isvalid_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void RobotPose::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:naothmessages.RobotPose)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RobotPose::CopyFrom(const RobotPose& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:naothmessages.RobotPose)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RobotPose::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  if (has_pose()) {
    if (!this->pose_->IsInitialized()) return false;
  }
  if (has_principleaxismajor()) {
    if (!this->principleaxismajor_->IsInitialized()) return false;
  }
  if (has_principleaxisminor()) {
    if (!this->principleaxisminor_->IsInitialized()) return false;
  }
  return true;
}

void RobotPose::Swap(RobotPose* other) {
  if (other == this) return;
  InternalSwap(other);
}
void RobotPose::InternalSwap(RobotPose* other) {
  using std::swap;
  swap(pose_, other->pose_);
  swap(principleaxismajor_, other->principleaxismajor_);
  swap(principleaxisminor_, other->principleaxisminor_);
  swap(isvalid_, other->isvalid_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata RobotPose::GetMetadata() const {
  protobuf_RobotPose_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_RobotPose_2eproto::file_level_metadata[kIndexInFileMessages];
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// RobotPose

// required .naothmessages.Pose2D pose = 1;
bool RobotPose::has_pose() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
void RobotPose::set_has_pose() {
  _has_bits_[0] |= 0x00000001u;
}
void RobotPose::clear_has_pose() {
  _has_bits_[0] &= ~0x00000001u;
}
void RobotPose::clear_pose() {
  if (pose_ != NULL) pose_->::naothmessages::Pose2D::Clear();
  clear_has_pose();
}
const ::naothmessages::Pose2D& RobotPose::pose() const {
  const ::naothmessages::Pose2D* p = pose_;
  // @@protoc_insertion_point(field_get:naothmessages.RobotPose.pose)
  return p != NULL ? *p : *reinterpret_cast<const ::naothmessages::Pose2D*>(
      &::naothmessages::_Pose2D_default_instance_);
}
::naothmessages::Pose2D* RobotPose::mutable_pose() {
  set_has_pose();
  if (pose_ == NULL) {
    pose_ = new ::naothmessages::Pose2D;
  }
  // @@protoc_insertion_point(field_mutable:naothmessages.RobotPose.pose)
  return pose_;
}
::naothmessages::Pose2D* RobotPose::release_pose() {
  // @@protoc_insertion_point(field_release:naothmessages.RobotPose.pose)
  clear_has_pose();
  ::naothmessages::Pose2D* temp = pose_;
  pose_ = NULL;
  return temp;
}
void RobotPose::set_allocated_pose(::naothmessages::Pose2D* pose) {
  delete pose_;
  pose_ = pose;
  if (pose) {
    set_has_pose();
  } else {
    clear_has_pose();
  }
  // @@protoc_insertion_point(field_set_allocated:naothmessages.RobotPose.pose)
}

// optional bool isValid = 2;
bool RobotPose::has_isvalid() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
void RobotPose::set_has_isvalid() {
  _has_bits_[0] |= 0x00000008u;
}
void RobotPose::clear_has_isvalid() {
  _has_bits_[0] &= ~0x00000008u;
}
void RobotPose::clear_isvalid() {
  isvalid_ = false;
  clear_has_isvalid();
}
bool RobotPose::isvalid() const {
  // @@protoc_insertion_point(field_get:naothmessages.RobotPose.isValid)
  return isvalid_;
}
void RobotPose::set_isvalid(bool value) {
  set_has_isvalid();
  isvalid_ = value;
  // @@protoc_insertion_point(field_set:naothmessages.RobotPose.isValid)
}

// optional .naothmessages.DoubleVector2 principleAxisMajor = 3;
bool RobotPose::has_principleaxismajor() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
void RobotPose::set_has_principleaxismajor() {
  _has_bits_[0] |= 0x00000002u;
}
void RobotPose::clear_has_principleaxismajor() {
  _has_bits_[0] &= ~0x00000002u;
}
void RobotPose::clear_principleaxismajor() {
  if (principleaxismajor_ != NULL) principleaxismajor_->::naothmessages::DoubleVector2::Clear();
  clear_has_principleaxismajor();
}
const ::naothmessages::DoubleVector2& RobotPose::principleaxismajor() const {
  const ::naothmessages::DoubleVector2* p = principleaxismajor_;
  // @@protoc_insertion_point(field_get:naothmessages.RobotPose.principleAxisMajor)
  return p != NULL ? *p : *reinterpret_cast<const ::naothmessages::DoubleVector2*>(
      &::naothmessages::_DoubleVector2_default_instance_);
}
::naothmessages::DoubleVector2* RobotPose::mutable_principleaxismajor() {
  set_has_principleaxismajor();
  if (principleaxismajor_ == NULL) {
    principleaxismajor_ = new ::naothmessages::DoubleVector2;
  }
  // @@protoc_insertion_point(field_mutable:naothmessages.RobotPose.principleAxisMajor)
  return principleaxismajor_;
}
::naothmessages::DoubleVector2* RobotPose::release_principleaxismajor() {
  // @@protoc_insertion_point(field_release:naothmessages.RobotPose.principleAxisMajor)
  clear_has_principleaxismajor();
  ::naothmessages::DoubleVector2* temp = principleaxismajor_;
  principleaxismajor_ = NULL;
  return temp;
}
void RobotPose::set_allocated_principleaxismajor(::naothmessages::DoubleVector2* principleaxismajor) {
  delete principleaxismajor_;
  principleaxismajor_ = principleaxismajor;
  if (principleaxismajor) {
    set_has_principleaxismajor();
  } else {
    clear_has_principleaxismajor();
  }
  // @@protoc_insertion_point(field_set_allocated:naothmessages.RobotPose.principleAxisMajor)
}

// optional .naothmessages.DoubleVector2 principleAxisMinor = 4;
bool RobotPose::has_principleaxisminor() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
void RobotPose::set_has_principleaxisminor() {
  _has_bits_[0] |= 0x00000004u;
}
void RobotPose::clear_has_principleaxisminor() {
  _has_bits_[0] &= ~0x00000004u;
}
void RobotPose::clear_principleaxisminor() {
  if (principleaxisminor_ != NULL) principleaxisminor_->::naothmessages::DoubleVector2::Clear();
  clear_has_principleaxisminor();
}
const ::naothmessages::DoubleVector2& RobotPose::principleaxisminor() const {
  const ::naothmessages::DoubleVector2* p = principleaxisminor_;
  // @@protoc_insertion_point(field_get:naothmessages.RobotPose.principleAxisMinor)
  return p != NULL ? *p : *reinterpret_cast<const ::naothmessages::DoubleVector2*>(
      &::naothmessages::_DoubleVector2_default_instance_);
}
::naothmessages::DoubleVector2* RobotPose::mutable_principleaxisminor() {
  set_has_principleaxisminor();
  if (principleaxisminor_ == NULL) {
    principleaxisminor_ = new ::naothmessages::DoubleVector2;
  }
  // @@protoc_insertion_point(field_mutable:naothmessages.RobotPose.principleAxisMinor)
  return principleaxisminor_;
}
::naothmessages::DoubleVector2* RobotPose::release_principleaxisminor() {
  // @@protoc_insertion_point(field_release:naothmessages.RobotPose.principleAxisMinor)
  clear_has_principleaxisminor();
  ::naothmessages::DoubleVector2* temp = principleaxisminor_;
  principleaxisminor_ = NULL;
  return temp;
}
void RobotPose::set_allocated_principleaxisminor(::naothmessages::DoubleVector2* principleaxisminor) {
  delete principleaxisminor_;
  principleaxisminor_ = principleaxisminor;
  if (principleaxisminor) {
    set_has_principleaxisminor();
  } else {
    clear_has_principleaxisminor();
  }
  // @@protoc_insertion_point(field_set_allocated:naothmessages.RobotPose.principleAxisMinor)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace naothmessages

// @@protoc_insertion_point(global_scope)