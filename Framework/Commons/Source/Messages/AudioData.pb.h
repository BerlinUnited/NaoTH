// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: AudioData.proto

#ifndef PROTOBUF_AudioData_2eproto__INCLUDED
#define PROTOBUF_AudioData_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3004000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
namespace naothmessages {
class AudioData;
class AudioDataDefaultTypeInternal;
extern AudioDataDefaultTypeInternal _AudioData_default_instance_;
}  // namespace naothmessages

namespace naothmessages {

namespace protobuf_AudioData_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[];
  static const ::google::protobuf::uint32 offsets[];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static void InitDefaultsImpl();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_AudioData_2eproto

// ===================================================================

class AudioData : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:naothmessages.AudioData) */ {
 public:
  AudioData();
  virtual ~AudioData();

  AudioData(const AudioData& from);

  inline AudioData& operator=(const AudioData& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  AudioData(AudioData&& from) noexcept
    : AudioData() {
    *this = ::std::move(from);
  }

  inline AudioData& operator=(AudioData&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const AudioData& default_instance();

  static inline const AudioData* internal_default_instance() {
    return reinterpret_cast<const AudioData*>(
               &_AudioData_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(AudioData* other);
  friend void swap(AudioData& a, AudioData& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline AudioData* New() const PROTOBUF_FINAL { return New(NULL); }

  AudioData* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const AudioData& from);
  void MergeFrom(const AudioData& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(AudioData* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required bytes samples = 1;
  bool has_samples() const;
  void clear_samples();
  static const int kSamplesFieldNumber = 1;
  const ::std::string& samples() const;
  void set_samples(const ::std::string& value);
  #if LANG_CXX11
  void set_samples(::std::string&& value);
  #endif
  void set_samples(const char* value);
  void set_samples(const void* value, size_t size);
  ::std::string* mutable_samples();
  ::std::string* release_samples();
  void set_allocated_samples(::std::string* samples);

  // required uint32 sampleSize = 2;
  bool has_samplesize() const;
  void clear_samplesize();
  static const int kSampleSizeFieldNumber = 2;
  ::google::protobuf::uint32 samplesize() const;
  void set_samplesize(::google::protobuf::uint32 value);

  // required uint32 sampleRate = 3;
  bool has_samplerate() const;
  void clear_samplerate();
  static const int kSampleRateFieldNumber = 3;
  ::google::protobuf::uint32 samplerate() const;
  void set_samplerate(::google::protobuf::uint32 value);

  // required uint32 numChannels = 4;
  bool has_numchannels() const;
  void clear_numchannels();
  static const int kNumChannelsFieldNumber = 4;
  ::google::protobuf::uint32 numchannels() const;
  void set_numchannels(::google::protobuf::uint32 value);

  // required uint32 timestamp = 5;
  bool has_timestamp() const;
  void clear_timestamp();
  static const int kTimestampFieldNumber = 5;
  ::google::protobuf::uint32 timestamp() const;
  void set_timestamp(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:naothmessages.AudioData)
 private:
  void set_has_samples();
  void clear_has_samples();
  void set_has_samplesize();
  void clear_has_samplesize();
  void set_has_samplerate();
  void clear_has_samplerate();
  void set_has_numchannels();
  void clear_has_numchannels();
  void set_has_timestamp();
  void clear_has_timestamp();

  // helper for ByteSizeLong()
  size_t RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::internal::ArenaStringPtr samples_;
  ::google::protobuf::uint32 samplesize_;
  ::google::protobuf::uint32 samplerate_;
  ::google::protobuf::uint32 numchannels_;
  ::google::protobuf::uint32 timestamp_;
  friend struct protobuf_AudioData_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// AudioData

// required bytes samples = 1;
inline bool AudioData::has_samples() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void AudioData::set_has_samples() {
  _has_bits_[0] |= 0x00000001u;
}
inline void AudioData::clear_has_samples() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void AudioData::clear_samples() {
  samples_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_samples();
}
inline const ::std::string& AudioData::samples() const {
  // @@protoc_insertion_point(field_get:naothmessages.AudioData.samples)
  return samples_.GetNoArena();
}
inline void AudioData::set_samples(const ::std::string& value) {
  set_has_samples();
  samples_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:naothmessages.AudioData.samples)
}
#if LANG_CXX11
inline void AudioData::set_samples(::std::string&& value) {
  set_has_samples();
  samples_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:naothmessages.AudioData.samples)
}
#endif
inline void AudioData::set_samples(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_samples();
  samples_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:naothmessages.AudioData.samples)
}
inline void AudioData::set_samples(const void* value, size_t size) {
  set_has_samples();
  samples_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:naothmessages.AudioData.samples)
}
inline ::std::string* AudioData::mutable_samples() {
  set_has_samples();
  // @@protoc_insertion_point(field_mutable:naothmessages.AudioData.samples)
  return samples_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* AudioData::release_samples() {
  // @@protoc_insertion_point(field_release:naothmessages.AudioData.samples)
  clear_has_samples();
  return samples_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void AudioData::set_allocated_samples(::std::string* samples) {
  if (samples != NULL) {
    set_has_samples();
  } else {
    clear_has_samples();
  }
  samples_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), samples);
  // @@protoc_insertion_point(field_set_allocated:naothmessages.AudioData.samples)
}

// required uint32 sampleSize = 2;
inline bool AudioData::has_samplesize() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void AudioData::set_has_samplesize() {
  _has_bits_[0] |= 0x00000002u;
}
inline void AudioData::clear_has_samplesize() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void AudioData::clear_samplesize() {
  samplesize_ = 0u;
  clear_has_samplesize();
}
inline ::google::protobuf::uint32 AudioData::samplesize() const {
  // @@protoc_insertion_point(field_get:naothmessages.AudioData.sampleSize)
  return samplesize_;
}
inline void AudioData::set_samplesize(::google::protobuf::uint32 value) {
  set_has_samplesize();
  samplesize_ = value;
  // @@protoc_insertion_point(field_set:naothmessages.AudioData.sampleSize)
}

// required uint32 sampleRate = 3;
inline bool AudioData::has_samplerate() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void AudioData::set_has_samplerate() {
  _has_bits_[0] |= 0x00000004u;
}
inline void AudioData::clear_has_samplerate() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void AudioData::clear_samplerate() {
  samplerate_ = 0u;
  clear_has_samplerate();
}
inline ::google::protobuf::uint32 AudioData::samplerate() const {
  // @@protoc_insertion_point(field_get:naothmessages.AudioData.sampleRate)
  return samplerate_;
}
inline void AudioData::set_samplerate(::google::protobuf::uint32 value) {
  set_has_samplerate();
  samplerate_ = value;
  // @@protoc_insertion_point(field_set:naothmessages.AudioData.sampleRate)
}

// required uint32 numChannels = 4;
inline bool AudioData::has_numchannels() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void AudioData::set_has_numchannels() {
  _has_bits_[0] |= 0x00000008u;
}
inline void AudioData::clear_has_numchannels() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void AudioData::clear_numchannels() {
  numchannels_ = 0u;
  clear_has_numchannels();
}
inline ::google::protobuf::uint32 AudioData::numchannels() const {
  // @@protoc_insertion_point(field_get:naothmessages.AudioData.numChannels)
  return numchannels_;
}
inline void AudioData::set_numchannels(::google::protobuf::uint32 value) {
  set_has_numchannels();
  numchannels_ = value;
  // @@protoc_insertion_point(field_set:naothmessages.AudioData.numChannels)
}

// required uint32 timestamp = 5;
inline bool AudioData::has_timestamp() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void AudioData::set_has_timestamp() {
  _has_bits_[0] |= 0x00000010u;
}
inline void AudioData::clear_has_timestamp() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void AudioData::clear_timestamp() {
  timestamp_ = 0u;
  clear_has_timestamp();
}
inline ::google::protobuf::uint32 AudioData::timestamp() const {
  // @@protoc_insertion_point(field_get:naothmessages.AudioData.timestamp)
  return timestamp_;
}
inline void AudioData::set_timestamp(::google::protobuf::uint32 value) {
  set_has_timestamp();
  timestamp_ = value;
  // @@protoc_insertion_point(field_set:naothmessages.AudioData.timestamp)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)


}  // namespace naothmessages

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_AudioData_2eproto__INCLUDED
