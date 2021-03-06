// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: AudioData.proto

package de.naoth.rc.core.messages;

public final class AudioDataOuterClass {
  private AudioDataOuterClass() {}
  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistryLite registry) {
  }

  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistry registry) {
    registerAllExtensions(
        (com.google.protobuf.ExtensionRegistryLite) registry);
  }
  public interface AudioDataOrBuilder extends
      // @@protoc_insertion_point(interface_extends:naothmessages.AudioData)
      com.google.protobuf.MessageOrBuilder {

    /**
     * <code>required bytes samples = 1;</code>
     */
    boolean hasSamples();
    /**
     * <code>required bytes samples = 1;</code>
     */
    com.google.protobuf.ByteString getSamples();

    /**
     * <code>required uint32 sampleSize = 2;</code>
     */
    boolean hasSampleSize();
    /**
     * <code>required uint32 sampleSize = 2;</code>
     */
    int getSampleSize();

    /**
     * <code>required uint32 sampleRate = 3;</code>
     */
    boolean hasSampleRate();
    /**
     * <code>required uint32 sampleRate = 3;</code>
     */
    int getSampleRate();

    /**
     * <code>required uint32 numChannels = 4;</code>
     */
    boolean hasNumChannels();
    /**
     * <code>required uint32 numChannels = 4;</code>
     */
    int getNumChannels();

    /**
     * <code>required uint32 timestamp = 5;</code>
     */
    boolean hasTimestamp();
    /**
     * <code>required uint32 timestamp = 5;</code>
     */
    int getTimestamp();
  }
  /**
   * Protobuf type {@code naothmessages.AudioData}
   */
  public  static final class AudioData extends
      com.google.protobuf.GeneratedMessageV3 implements
      // @@protoc_insertion_point(message_implements:naothmessages.AudioData)
      AudioDataOrBuilder {
  private static final long serialVersionUID = 0L;
    // Use AudioData.newBuilder() to construct.
    private AudioData(com.google.protobuf.GeneratedMessageV3.Builder<?> builder) {
      super(builder);
    }
    private AudioData() {
      samples_ = com.google.protobuf.ByteString.EMPTY;
      sampleSize_ = 0;
      sampleRate_ = 0;
      numChannels_ = 0;
      timestamp_ = 0;
    }

    @java.lang.Override
    public final com.google.protobuf.UnknownFieldSet
    getUnknownFields() {
      return this.unknownFields;
    }
    private AudioData(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      this();
      int mutable_bitField0_ = 0;
      com.google.protobuf.UnknownFieldSet.Builder unknownFields =
          com.google.protobuf.UnknownFieldSet.newBuilder();
      try {
        boolean done = false;
        while (!done) {
          int tag = input.readTag();
          switch (tag) {
            case 0:
              done = true;
              break;
            default: {
              if (!parseUnknownField(
                  input, unknownFields, extensionRegistry, tag)) {
                done = true;
              }
              break;
            }
            case 10: {
              bitField0_ |= 0x00000001;
              samples_ = input.readBytes();
              break;
            }
            case 16: {
              bitField0_ |= 0x00000002;
              sampleSize_ = input.readUInt32();
              break;
            }
            case 24: {
              bitField0_ |= 0x00000004;
              sampleRate_ = input.readUInt32();
              break;
            }
            case 32: {
              bitField0_ |= 0x00000008;
              numChannels_ = input.readUInt32();
              break;
            }
            case 40: {
              bitField0_ |= 0x00000010;
              timestamp_ = input.readUInt32();
              break;
            }
          }
        }
      } catch (com.google.protobuf.InvalidProtocolBufferException e) {
        throw e.setUnfinishedMessage(this);
      } catch (java.io.IOException e) {
        throw new com.google.protobuf.InvalidProtocolBufferException(
            e).setUnfinishedMessage(this);
      } finally {
        this.unknownFields = unknownFields.build();
        makeExtensionsImmutable();
      }
    }
    public static final com.google.protobuf.Descriptors.Descriptor
        getDescriptor() {
      return de.naoth.rc.core.messages.AudioDataOuterClass.internal_static_naothmessages_AudioData_descriptor;
    }

    protected com.google.protobuf.GeneratedMessageV3.FieldAccessorTable
        internalGetFieldAccessorTable() {
      return de.naoth.rc.core.messages.AudioDataOuterClass.internal_static_naothmessages_AudioData_fieldAccessorTable
          .ensureFieldAccessorsInitialized(
              de.naoth.rc.core.messages.AudioDataOuterClass.AudioData.class, de.naoth.rc.core.messages.AudioDataOuterClass.AudioData.Builder.class);
    }

    private int bitField0_;
    public static final int SAMPLES_FIELD_NUMBER = 1;
    private com.google.protobuf.ByteString samples_;
    /**
     * <code>required bytes samples = 1;</code>
     */
    public boolean hasSamples() {
      return ((bitField0_ & 0x00000001) == 0x00000001);
    }
    /**
     * <code>required bytes samples = 1;</code>
     */
    public com.google.protobuf.ByteString getSamples() {
      return samples_;
    }

    public static final int SAMPLESIZE_FIELD_NUMBER = 2;
    private int sampleSize_;
    /**
     * <code>required uint32 sampleSize = 2;</code>
     */
    public boolean hasSampleSize() {
      return ((bitField0_ & 0x00000002) == 0x00000002);
    }
    /**
     * <code>required uint32 sampleSize = 2;</code>
     */
    public int getSampleSize() {
      return sampleSize_;
    }

    public static final int SAMPLERATE_FIELD_NUMBER = 3;
    private int sampleRate_;
    /**
     * <code>required uint32 sampleRate = 3;</code>
     */
    public boolean hasSampleRate() {
      return ((bitField0_ & 0x00000004) == 0x00000004);
    }
    /**
     * <code>required uint32 sampleRate = 3;</code>
     */
    public int getSampleRate() {
      return sampleRate_;
    }

    public static final int NUMCHANNELS_FIELD_NUMBER = 4;
    private int numChannels_;
    /**
     * <code>required uint32 numChannels = 4;</code>
     */
    public boolean hasNumChannels() {
      return ((bitField0_ & 0x00000008) == 0x00000008);
    }
    /**
     * <code>required uint32 numChannels = 4;</code>
     */
    public int getNumChannels() {
      return numChannels_;
    }

    public static final int TIMESTAMP_FIELD_NUMBER = 5;
    private int timestamp_;
    /**
     * <code>required uint32 timestamp = 5;</code>
     */
    public boolean hasTimestamp() {
      return ((bitField0_ & 0x00000010) == 0x00000010);
    }
    /**
     * <code>required uint32 timestamp = 5;</code>
     */
    public int getTimestamp() {
      return timestamp_;
    }

    private byte memoizedIsInitialized = -1;
    public final boolean isInitialized() {
      byte isInitialized = memoizedIsInitialized;
      if (isInitialized == 1) return true;
      if (isInitialized == 0) return false;

      if (!hasSamples()) {
        memoizedIsInitialized = 0;
        return false;
      }
      if (!hasSampleSize()) {
        memoizedIsInitialized = 0;
        return false;
      }
      if (!hasSampleRate()) {
        memoizedIsInitialized = 0;
        return false;
      }
      if (!hasNumChannels()) {
        memoizedIsInitialized = 0;
        return false;
      }
      if (!hasTimestamp()) {
        memoizedIsInitialized = 0;
        return false;
      }
      memoizedIsInitialized = 1;
      return true;
    }

    public void writeTo(com.google.protobuf.CodedOutputStream output)
                        throws java.io.IOException {
      if (((bitField0_ & 0x00000001) == 0x00000001)) {
        output.writeBytes(1, samples_);
      }
      if (((bitField0_ & 0x00000002) == 0x00000002)) {
        output.writeUInt32(2, sampleSize_);
      }
      if (((bitField0_ & 0x00000004) == 0x00000004)) {
        output.writeUInt32(3, sampleRate_);
      }
      if (((bitField0_ & 0x00000008) == 0x00000008)) {
        output.writeUInt32(4, numChannels_);
      }
      if (((bitField0_ & 0x00000010) == 0x00000010)) {
        output.writeUInt32(5, timestamp_);
      }
      unknownFields.writeTo(output);
    }

    public int getSerializedSize() {
      int size = memoizedSize;
      if (size != -1) return size;

      size = 0;
      if (((bitField0_ & 0x00000001) == 0x00000001)) {
        size += com.google.protobuf.CodedOutputStream
          .computeBytesSize(1, samples_);
      }
      if (((bitField0_ & 0x00000002) == 0x00000002)) {
        size += com.google.protobuf.CodedOutputStream
          .computeUInt32Size(2, sampleSize_);
      }
      if (((bitField0_ & 0x00000004) == 0x00000004)) {
        size += com.google.protobuf.CodedOutputStream
          .computeUInt32Size(3, sampleRate_);
      }
      if (((bitField0_ & 0x00000008) == 0x00000008)) {
        size += com.google.protobuf.CodedOutputStream
          .computeUInt32Size(4, numChannels_);
      }
      if (((bitField0_ & 0x00000010) == 0x00000010)) {
        size += com.google.protobuf.CodedOutputStream
          .computeUInt32Size(5, timestamp_);
      }
      size += unknownFields.getSerializedSize();
      memoizedSize = size;
      return size;
    }

    @java.lang.Override
    public boolean equals(final java.lang.Object obj) {
      if (obj == this) {
       return true;
      }
      if (!(obj instanceof de.naoth.rc.core.messages.AudioDataOuterClass.AudioData)) {
        return super.equals(obj);
      }
      de.naoth.rc.core.messages.AudioDataOuterClass.AudioData other = (de.naoth.rc.core.messages.AudioDataOuterClass.AudioData) obj;

      boolean result = true;
      result = result && (hasSamples() == other.hasSamples());
      if (hasSamples()) {
        result = result && getSamples()
            .equals(other.getSamples());
      }
      result = result && (hasSampleSize() == other.hasSampleSize());
      if (hasSampleSize()) {
        result = result && (getSampleSize()
            == other.getSampleSize());
      }
      result = result && (hasSampleRate() == other.hasSampleRate());
      if (hasSampleRate()) {
        result = result && (getSampleRate()
            == other.getSampleRate());
      }
      result = result && (hasNumChannels() == other.hasNumChannels());
      if (hasNumChannels()) {
        result = result && (getNumChannels()
            == other.getNumChannels());
      }
      result = result && (hasTimestamp() == other.hasTimestamp());
      if (hasTimestamp()) {
        result = result && (getTimestamp()
            == other.getTimestamp());
      }
      result = result && unknownFields.equals(other.unknownFields);
      return result;
    }

    @java.lang.Override
    public int hashCode() {
      if (memoizedHashCode != 0) {
        return memoizedHashCode;
      }
      int hash = 41;
      hash = (19 * hash) + getDescriptor().hashCode();
      if (hasSamples()) {
        hash = (37 * hash) + SAMPLES_FIELD_NUMBER;
        hash = (53 * hash) + getSamples().hashCode();
      }
      if (hasSampleSize()) {
        hash = (37 * hash) + SAMPLESIZE_FIELD_NUMBER;
        hash = (53 * hash) + getSampleSize();
      }
      if (hasSampleRate()) {
        hash = (37 * hash) + SAMPLERATE_FIELD_NUMBER;
        hash = (53 * hash) + getSampleRate();
      }
      if (hasNumChannels()) {
        hash = (37 * hash) + NUMCHANNELS_FIELD_NUMBER;
        hash = (53 * hash) + getNumChannels();
      }
      if (hasTimestamp()) {
        hash = (37 * hash) + TIMESTAMP_FIELD_NUMBER;
        hash = (53 * hash) + getTimestamp();
      }
      hash = (29 * hash) + unknownFields.hashCode();
      memoizedHashCode = hash;
      return hash;
    }

    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parseFrom(
        java.nio.ByteBuffer data)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return PARSER.parseFrom(data);
    }
    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parseFrom(
        java.nio.ByteBuffer data,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return PARSER.parseFrom(data, extensionRegistry);
    }
    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parseFrom(
        com.google.protobuf.ByteString data)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return PARSER.parseFrom(data);
    }
    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parseFrom(
        com.google.protobuf.ByteString data,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return PARSER.parseFrom(data, extensionRegistry);
    }
    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parseFrom(byte[] data)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return PARSER.parseFrom(data);
    }
    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parseFrom(
        byte[] data,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return PARSER.parseFrom(data, extensionRegistry);
    }
    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parseFrom(java.io.InputStream input)
        throws java.io.IOException {
      return com.google.protobuf.GeneratedMessageV3
          .parseWithIOException(PARSER, input);
    }
    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parseFrom(
        java.io.InputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws java.io.IOException {
      return com.google.protobuf.GeneratedMessageV3
          .parseWithIOException(PARSER, input, extensionRegistry);
    }
    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parseDelimitedFrom(java.io.InputStream input)
        throws java.io.IOException {
      return com.google.protobuf.GeneratedMessageV3
          .parseDelimitedWithIOException(PARSER, input);
    }
    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parseDelimitedFrom(
        java.io.InputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws java.io.IOException {
      return com.google.protobuf.GeneratedMessageV3
          .parseDelimitedWithIOException(PARSER, input, extensionRegistry);
    }
    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parseFrom(
        com.google.protobuf.CodedInputStream input)
        throws java.io.IOException {
      return com.google.protobuf.GeneratedMessageV3
          .parseWithIOException(PARSER, input);
    }
    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parseFrom(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws java.io.IOException {
      return com.google.protobuf.GeneratedMessageV3
          .parseWithIOException(PARSER, input, extensionRegistry);
    }

    public Builder newBuilderForType() { return newBuilder(); }
    public static Builder newBuilder() {
      return DEFAULT_INSTANCE.toBuilder();
    }
    public static Builder newBuilder(de.naoth.rc.core.messages.AudioDataOuterClass.AudioData prototype) {
      return DEFAULT_INSTANCE.toBuilder().mergeFrom(prototype);
    }
    public Builder toBuilder() {
      return this == DEFAULT_INSTANCE
          ? new Builder() : new Builder().mergeFrom(this);
    }

    @java.lang.Override
    protected Builder newBuilderForType(
        com.google.protobuf.GeneratedMessageV3.BuilderParent parent) {
      Builder builder = new Builder(parent);
      return builder;
    }
    /**
     * Protobuf type {@code naothmessages.AudioData}
     */
    public static final class Builder extends
        com.google.protobuf.GeneratedMessageV3.Builder<Builder> implements
        // @@protoc_insertion_point(builder_implements:naothmessages.AudioData)
        de.naoth.rc.core.messages.AudioDataOuterClass.AudioDataOrBuilder {
      public static final com.google.protobuf.Descriptors.Descriptor
          getDescriptor() {
        return de.naoth.rc.core.messages.AudioDataOuterClass.internal_static_naothmessages_AudioData_descriptor;
      }

      protected com.google.protobuf.GeneratedMessageV3.FieldAccessorTable
          internalGetFieldAccessorTable() {
        return de.naoth.rc.core.messages.AudioDataOuterClass.internal_static_naothmessages_AudioData_fieldAccessorTable
            .ensureFieldAccessorsInitialized(
                de.naoth.rc.core.messages.AudioDataOuterClass.AudioData.class, de.naoth.rc.core.messages.AudioDataOuterClass.AudioData.Builder.class);
      }

      // Construct using de.naoth.rc.core.messages.AudioDataOuterClass.AudioData.newBuilder()
      private Builder() {
        maybeForceBuilderInitialization();
      }

      private Builder(
          com.google.protobuf.GeneratedMessageV3.BuilderParent parent) {
        super(parent);
        maybeForceBuilderInitialization();
      }
      private void maybeForceBuilderInitialization() {
        if (com.google.protobuf.GeneratedMessageV3
                .alwaysUseFieldBuilders) {
        }
      }
      public Builder clear() {
        super.clear();
        samples_ = com.google.protobuf.ByteString.EMPTY;
        bitField0_ = (bitField0_ & ~0x00000001);
        sampleSize_ = 0;
        bitField0_ = (bitField0_ & ~0x00000002);
        sampleRate_ = 0;
        bitField0_ = (bitField0_ & ~0x00000004);
        numChannels_ = 0;
        bitField0_ = (bitField0_ & ~0x00000008);
        timestamp_ = 0;
        bitField0_ = (bitField0_ & ~0x00000010);
        return this;
      }

      public com.google.protobuf.Descriptors.Descriptor
          getDescriptorForType() {
        return de.naoth.rc.core.messages.AudioDataOuterClass.internal_static_naothmessages_AudioData_descriptor;
      }

      public de.naoth.rc.core.messages.AudioDataOuterClass.AudioData getDefaultInstanceForType() {
        return de.naoth.rc.core.messages.AudioDataOuterClass.AudioData.getDefaultInstance();
      }

      public de.naoth.rc.core.messages.AudioDataOuterClass.AudioData build() {
        de.naoth.rc.core.messages.AudioDataOuterClass.AudioData result = buildPartial();
        if (!result.isInitialized()) {
          throw newUninitializedMessageException(result);
        }
        return result;
      }

      public de.naoth.rc.core.messages.AudioDataOuterClass.AudioData buildPartial() {
        de.naoth.rc.core.messages.AudioDataOuterClass.AudioData result = new de.naoth.rc.core.messages.AudioDataOuterClass.AudioData(this);
        int from_bitField0_ = bitField0_;
        int to_bitField0_ = 0;
        if (((from_bitField0_ & 0x00000001) == 0x00000001)) {
          to_bitField0_ |= 0x00000001;
        }
        result.samples_ = samples_;
        if (((from_bitField0_ & 0x00000002) == 0x00000002)) {
          to_bitField0_ |= 0x00000002;
        }
        result.sampleSize_ = sampleSize_;
        if (((from_bitField0_ & 0x00000004) == 0x00000004)) {
          to_bitField0_ |= 0x00000004;
        }
        result.sampleRate_ = sampleRate_;
        if (((from_bitField0_ & 0x00000008) == 0x00000008)) {
          to_bitField0_ |= 0x00000008;
        }
        result.numChannels_ = numChannels_;
        if (((from_bitField0_ & 0x00000010) == 0x00000010)) {
          to_bitField0_ |= 0x00000010;
        }
        result.timestamp_ = timestamp_;
        result.bitField0_ = to_bitField0_;
        onBuilt();
        return result;
      }

      public Builder clone() {
        return (Builder) super.clone();
      }
      public Builder setField(
          com.google.protobuf.Descriptors.FieldDescriptor field,
          java.lang.Object value) {
        return (Builder) super.setField(field, value);
      }
      public Builder clearField(
          com.google.protobuf.Descriptors.FieldDescriptor field) {
        return (Builder) super.clearField(field);
      }
      public Builder clearOneof(
          com.google.protobuf.Descriptors.OneofDescriptor oneof) {
        return (Builder) super.clearOneof(oneof);
      }
      public Builder setRepeatedField(
          com.google.protobuf.Descriptors.FieldDescriptor field,
          int index, java.lang.Object value) {
        return (Builder) super.setRepeatedField(field, index, value);
      }
      public Builder addRepeatedField(
          com.google.protobuf.Descriptors.FieldDescriptor field,
          java.lang.Object value) {
        return (Builder) super.addRepeatedField(field, value);
      }
      public Builder mergeFrom(com.google.protobuf.Message other) {
        if (other instanceof de.naoth.rc.core.messages.AudioDataOuterClass.AudioData) {
          return mergeFrom((de.naoth.rc.core.messages.AudioDataOuterClass.AudioData)other);
        } else {
          super.mergeFrom(other);
          return this;
        }
      }

      public Builder mergeFrom(de.naoth.rc.core.messages.AudioDataOuterClass.AudioData other) {
        if (other == de.naoth.rc.core.messages.AudioDataOuterClass.AudioData.getDefaultInstance()) return this;
        if (other.hasSamples()) {
          setSamples(other.getSamples());
        }
        if (other.hasSampleSize()) {
          setSampleSize(other.getSampleSize());
        }
        if (other.hasSampleRate()) {
          setSampleRate(other.getSampleRate());
        }
        if (other.hasNumChannels()) {
          setNumChannels(other.getNumChannels());
        }
        if (other.hasTimestamp()) {
          setTimestamp(other.getTimestamp());
        }
        this.mergeUnknownFields(other.unknownFields);
        onChanged();
        return this;
      }

      public final boolean isInitialized() {
        if (!hasSamples()) {
          return false;
        }
        if (!hasSampleSize()) {
          return false;
        }
        if (!hasSampleRate()) {
          return false;
        }
        if (!hasNumChannels()) {
          return false;
        }
        if (!hasTimestamp()) {
          return false;
        }
        return true;
      }

      public Builder mergeFrom(
          com.google.protobuf.CodedInputStream input,
          com.google.protobuf.ExtensionRegistryLite extensionRegistry)
          throws java.io.IOException {
        de.naoth.rc.core.messages.AudioDataOuterClass.AudioData parsedMessage = null;
        try {
          parsedMessage = PARSER.parsePartialFrom(input, extensionRegistry);
        } catch (com.google.protobuf.InvalidProtocolBufferException e) {
          parsedMessage = (de.naoth.rc.core.messages.AudioDataOuterClass.AudioData) e.getUnfinishedMessage();
          throw e.unwrapIOException();
        } finally {
          if (parsedMessage != null) {
            mergeFrom(parsedMessage);
          }
        }
        return this;
      }
      private int bitField0_;

      private com.google.protobuf.ByteString samples_ = com.google.protobuf.ByteString.EMPTY;
      /**
       * <code>required bytes samples = 1;</code>
       */
      public boolean hasSamples() {
        return ((bitField0_ & 0x00000001) == 0x00000001);
      }
      /**
       * <code>required bytes samples = 1;</code>
       */
      public com.google.protobuf.ByteString getSamples() {
        return samples_;
      }
      /**
       * <code>required bytes samples = 1;</code>
       */
      public Builder setSamples(com.google.protobuf.ByteString value) {
        if (value == null) {
    throw new NullPointerException();
  }
  bitField0_ |= 0x00000001;
        samples_ = value;
        onChanged();
        return this;
      }
      /**
       * <code>required bytes samples = 1;</code>
       */
      public Builder clearSamples() {
        bitField0_ = (bitField0_ & ~0x00000001);
        samples_ = getDefaultInstance().getSamples();
        onChanged();
        return this;
      }

      private int sampleSize_ ;
      /**
       * <code>required uint32 sampleSize = 2;</code>
       */
      public boolean hasSampleSize() {
        return ((bitField0_ & 0x00000002) == 0x00000002);
      }
      /**
       * <code>required uint32 sampleSize = 2;</code>
       */
      public int getSampleSize() {
        return sampleSize_;
      }
      /**
       * <code>required uint32 sampleSize = 2;</code>
       */
      public Builder setSampleSize(int value) {
        bitField0_ |= 0x00000002;
        sampleSize_ = value;
        onChanged();
        return this;
      }
      /**
       * <code>required uint32 sampleSize = 2;</code>
       */
      public Builder clearSampleSize() {
        bitField0_ = (bitField0_ & ~0x00000002);
        sampleSize_ = 0;
        onChanged();
        return this;
      }

      private int sampleRate_ ;
      /**
       * <code>required uint32 sampleRate = 3;</code>
       */
      public boolean hasSampleRate() {
        return ((bitField0_ & 0x00000004) == 0x00000004);
      }
      /**
       * <code>required uint32 sampleRate = 3;</code>
       */
      public int getSampleRate() {
        return sampleRate_;
      }
      /**
       * <code>required uint32 sampleRate = 3;</code>
       */
      public Builder setSampleRate(int value) {
        bitField0_ |= 0x00000004;
        sampleRate_ = value;
        onChanged();
        return this;
      }
      /**
       * <code>required uint32 sampleRate = 3;</code>
       */
      public Builder clearSampleRate() {
        bitField0_ = (bitField0_ & ~0x00000004);
        sampleRate_ = 0;
        onChanged();
        return this;
      }

      private int numChannels_ ;
      /**
       * <code>required uint32 numChannels = 4;</code>
       */
      public boolean hasNumChannels() {
        return ((bitField0_ & 0x00000008) == 0x00000008);
      }
      /**
       * <code>required uint32 numChannels = 4;</code>
       */
      public int getNumChannels() {
        return numChannels_;
      }
      /**
       * <code>required uint32 numChannels = 4;</code>
       */
      public Builder setNumChannels(int value) {
        bitField0_ |= 0x00000008;
        numChannels_ = value;
        onChanged();
        return this;
      }
      /**
       * <code>required uint32 numChannels = 4;</code>
       */
      public Builder clearNumChannels() {
        bitField0_ = (bitField0_ & ~0x00000008);
        numChannels_ = 0;
        onChanged();
        return this;
      }

      private int timestamp_ ;
      /**
       * <code>required uint32 timestamp = 5;</code>
       */
      public boolean hasTimestamp() {
        return ((bitField0_ & 0x00000010) == 0x00000010);
      }
      /**
       * <code>required uint32 timestamp = 5;</code>
       */
      public int getTimestamp() {
        return timestamp_;
      }
      /**
       * <code>required uint32 timestamp = 5;</code>
       */
      public Builder setTimestamp(int value) {
        bitField0_ |= 0x00000010;
        timestamp_ = value;
        onChanged();
        return this;
      }
      /**
       * <code>required uint32 timestamp = 5;</code>
       */
      public Builder clearTimestamp() {
        bitField0_ = (bitField0_ & ~0x00000010);
        timestamp_ = 0;
        onChanged();
        return this;
      }
      public final Builder setUnknownFields(
          final com.google.protobuf.UnknownFieldSet unknownFields) {
        return super.setUnknownFields(unknownFields);
      }

      public final Builder mergeUnknownFields(
          final com.google.protobuf.UnknownFieldSet unknownFields) {
        return super.mergeUnknownFields(unknownFields);
      }


      // @@protoc_insertion_point(builder_scope:naothmessages.AudioData)
    }

    // @@protoc_insertion_point(class_scope:naothmessages.AudioData)
    private static final de.naoth.rc.core.messages.AudioDataOuterClass.AudioData DEFAULT_INSTANCE;
    static {
      DEFAULT_INSTANCE = new de.naoth.rc.core.messages.AudioDataOuterClass.AudioData();
    }

    public static de.naoth.rc.core.messages.AudioDataOuterClass.AudioData getDefaultInstance() {
      return DEFAULT_INSTANCE;
    }

    @java.lang.Deprecated public static final com.google.protobuf.Parser<AudioData>
        PARSER = new com.google.protobuf.AbstractParser<AudioData>() {
      public AudioData parsePartialFrom(
          com.google.protobuf.CodedInputStream input,
          com.google.protobuf.ExtensionRegistryLite extensionRegistry)
          throws com.google.protobuf.InvalidProtocolBufferException {
          return new AudioData(input, extensionRegistry);
      }
    };

    public static com.google.protobuf.Parser<AudioData> parser() {
      return PARSER;
    }

    @java.lang.Override
    public com.google.protobuf.Parser<AudioData> getParserForType() {
      return PARSER;
    }

    public de.naoth.rc.core.messages.AudioDataOuterClass.AudioData getDefaultInstanceForType() {
      return DEFAULT_INSTANCE;
    }

  }

  private static final com.google.protobuf.Descriptors.Descriptor
    internal_static_naothmessages_AudioData_descriptor;
  private static final 
    com.google.protobuf.GeneratedMessageV3.FieldAccessorTable
      internal_static_naothmessages_AudioData_fieldAccessorTable;

  public static com.google.protobuf.Descriptors.FileDescriptor
      getDescriptor() {
    return descriptor;
  }
  private static  com.google.protobuf.Descriptors.FileDescriptor
      descriptor;
  static {
    java.lang.String[] descriptorData = {
      "\n\017AudioData.proto\022\rnaothmessages\"l\n\tAudi" +
      "oData\022\017\n\007samples\030\001 \002(\014\022\022\n\nsampleSize\030\002 \002" +
      "(\r\022\022\n\nsampleRate\030\003 \002(\r\022\023\n\013numChannels\030\004 " +
      "\002(\r\022\021\n\ttimestamp\030\005 \002(\rB\033\n\031de.naoth.rc.co" +
      "re.messages"
    };
    com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner assigner =
        new com.google.protobuf.Descriptors.FileDescriptor.    InternalDescriptorAssigner() {
          public com.google.protobuf.ExtensionRegistry assignDescriptors(
              com.google.protobuf.Descriptors.FileDescriptor root) {
            descriptor = root;
            return null;
          }
        };
    com.google.protobuf.Descriptors.FileDescriptor
      .internalBuildGeneratedFileFrom(descriptorData,
        new com.google.protobuf.Descriptors.FileDescriptor[] {
        }, assigner);
    internal_static_naothmessages_AudioData_descriptor =
      getDescriptor().getMessageTypes().get(0);
    internal_static_naothmessages_AudioData_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessageV3.FieldAccessorTable(
        internal_static_naothmessages_AudioData_descriptor,
        new java.lang.String[] { "Samples", "SampleSize", "SampleRate", "NumChannels", "Timestamp", });
  }

  // @@protoc_insertion_point(outer_class_scope)
}
