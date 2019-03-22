package protos;// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: distributedmapmessage.proto

public final class MessageObjectProtos {
    private static com.google.protobuf.Descriptors.Descriptor
            internal_static_MessageObject_descriptor;
    private static
    com.google.protobuf.GeneratedMessage.FieldAccessorTable
            internal_static_MessageObject_fieldAccessorTable;
    private static com.google.protobuf.Descriptors.FileDescriptor
            descriptor;

    static {
        java.lang.String[] descriptorData = {
                "\n\033distributedmapmessage.proto\"\243\001\n\rMessag" +
                        "eObject\022.\n\004type\030\001 \002(\0162 .MessageObject.Me" +
                        "ssageObjectType\022\013\n\003key\030\002 \002(\t\022\r\n\005value\030\003 " +
                        "\001(\005\"F\n\021MessageObjectType\022\017\n\013NEW_ELEMENT\020" +
                        "\000\022\017\n\013GET_ELEMENT\020\001\022\017\n\013RET_ELEMENT\020\002B\035\n\006p" +
                        "rotosB\023MessageObjectProtos"
        };
        com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner assigner =
                new com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner() {
                    public com.google.protobuf.ExtensionRegistry assignDescriptors(
                            com.google.protobuf.Descriptors.FileDescriptor root) {
                        descriptor = root;
                        internal_static_MessageObject_descriptor =
                                getDescriptor().getMessageTypes().get(0);
                        internal_static_MessageObject_fieldAccessorTable = new
                                com.google.protobuf.GeneratedMessage.FieldAccessorTable(
                                internal_static_MessageObject_descriptor,
                                new java.lang.String[]{"Type", "Key", "Value",},
                                protos.MessageObjectProtos.MessageObject.class,
                                protos.MessageObjectProtos.MessageObject.Builder.class);
                        return null;
                    }
                };
        com.google.protobuf.Descriptors.FileDescriptor
                .internalBuildGeneratedFileFrom(descriptorData,
                        new com.google.protobuf.Descriptors.FileDescriptor[]{
                        }, assigner);
    }

    private MessageObjectProtos() {
    }

    public static void registerAllExtensions(
            com.google.protobuf.ExtensionRegistry registry) {
    }

    public static com.google.protobuf.Descriptors.FileDescriptor
    getDescriptor() {
        return descriptor;
    }

    public interface MessageObjectOrBuilder
            extends com.google.protobuf.MessageOrBuilder {

        // required .MessageObject.MessageObjectType type = 1;
        boolean hasType();

        protos.MessageObjectProtos.MessageObject.MessageObjectType getType();

        // required string key = 2;
        boolean hasKey();

        String getKey();

        // optional int32 value = 3;
        boolean hasValue();

        int getValue();
    }

    public static final class MessageObject extends
            com.google.protobuf.GeneratedMessage
            implements MessageObjectOrBuilder {
        // required .MessageObject.MessageObjectType type = 1;
        public static final int TYPE_FIELD_NUMBER = 1;
        // required string key = 2;
        public static final int KEY_FIELD_NUMBER = 2;
        // optional int32 value = 3;
        public static final int VALUE_FIELD_NUMBER = 3;
        private static final MessageObject defaultInstance;
        private static final long serialVersionUID = 0L;

        static {
            defaultInstance = new MessageObject(true);
            defaultInstance.initFields();
        }

        private int bitField0_;
        private protos.MessageObjectProtos.MessageObject.MessageObjectType type_;
        private java.lang.Object key_;
        private int value_;
        private byte memoizedIsInitialized = -1;
        private int memoizedSerializedSize = -1;

        // Use MessageObject.newBuilder() to construct.
        private MessageObject(Builder builder) {
            super(builder);
        }

        private MessageObject(boolean noInit) {
        }

        public static MessageObject getDefaultInstance() {
            return defaultInstance;
        }

        public static final com.google.protobuf.Descriptors.Descriptor
        getDescriptor() {
            return internal_static_MessageObject_descriptor;
        }

        public static protos.MessageObjectProtos.MessageObject parseFrom(
                com.google.protobuf.ByteString data)
                throws com.google.protobuf.InvalidProtocolBufferException {
            return newBuilder().mergeFrom(data).buildParsed();
        }

        public static protos.MessageObjectProtos.MessageObject parseFrom(
                com.google.protobuf.ByteString data,
                com.google.protobuf.ExtensionRegistryLite extensionRegistry)
                throws com.google.protobuf.InvalidProtocolBufferException {
            return newBuilder().mergeFrom(data, extensionRegistry)
                    .buildParsed();
        }

        public static protos.MessageObjectProtos.MessageObject parseFrom(byte[] data)
                throws com.google.protobuf.InvalidProtocolBufferException {
            return newBuilder().mergeFrom(data).buildParsed();
        }

        public static protos.MessageObjectProtos.MessageObject parseFrom(
                byte[] data,
                com.google.protobuf.ExtensionRegistryLite extensionRegistry)
                throws com.google.protobuf.InvalidProtocolBufferException {
            return newBuilder().mergeFrom(data, extensionRegistry)
                    .buildParsed();
        }

        public static protos.MessageObjectProtos.MessageObject parseFrom(java.io.InputStream input)
                throws java.io.IOException {
            return newBuilder().mergeFrom(input).buildParsed();
        }

        public static protos.MessageObjectProtos.MessageObject parseFrom(
                java.io.InputStream input,
                com.google.protobuf.ExtensionRegistryLite extensionRegistry)
                throws java.io.IOException {
            return newBuilder().mergeFrom(input, extensionRegistry)
                    .buildParsed();
        }

        public static protos.MessageObjectProtos.MessageObject parseDelimitedFrom(java.io.InputStream input)
                throws java.io.IOException {
            Builder builder = newBuilder();
            if (builder.mergeDelimitedFrom(input)) {
                return builder.buildParsed();
            } else {
                return null;
            }
        }

        public static protos.MessageObjectProtos.MessageObject parseDelimitedFrom(
                java.io.InputStream input,
                com.google.protobuf.ExtensionRegistryLite extensionRegistry)
                throws java.io.IOException {
            Builder builder = newBuilder();
            if (builder.mergeDelimitedFrom(input, extensionRegistry)) {
                return builder.buildParsed();
            } else {
                return null;
            }
        }

        public static protos.MessageObjectProtos.MessageObject parseFrom(
                com.google.protobuf.CodedInputStream input)
                throws java.io.IOException {
            return newBuilder().mergeFrom(input).buildParsed();
        }

        public static protos.MessageObjectProtos.MessageObject parseFrom(
                com.google.protobuf.CodedInputStream input,
                com.google.protobuf.ExtensionRegistryLite extensionRegistry)
                throws java.io.IOException {
            return newBuilder().mergeFrom(input, extensionRegistry)
                    .buildParsed();
        }

        public static Builder newBuilder() {
            return Builder.create();
        }

        public static Builder newBuilder(protos.MessageObjectProtos.MessageObject prototype) {
            return newBuilder().mergeFrom(prototype);
        }

        public MessageObject getDefaultInstanceForType() {
            return defaultInstance;
        }

        protected com.google.protobuf.GeneratedMessage.FieldAccessorTable
        internalGetFieldAccessorTable() {
            return internal_static_MessageObject_fieldAccessorTable;
        }

        public boolean hasType() {
            return ((bitField0_ & 0x00000001) == 0x00000001);
        }

        public protos.MessageObjectProtos.MessageObject.MessageObjectType getType() {
            return type_;
        }

        public boolean hasKey() {
            return ((bitField0_ & 0x00000002) == 0x00000002);
        }

        public String getKey() {
            java.lang.Object ref = key_;
            if (ref instanceof String) {
                return (String) ref;
            } else {
                com.google.protobuf.ByteString bs =
                        (com.google.protobuf.ByteString) ref;
                String s = bs.toStringUtf8();
                if (com.google.protobuf.Internal.isValidUtf8(bs)) {
                    key_ = s;
                }
                return s;
            }
        }

        private com.google.protobuf.ByteString getKeyBytes() {
            java.lang.Object ref = key_;
            if (ref instanceof String) {
                com.google.protobuf.ByteString b =
                        com.google.protobuf.ByteString.copyFromUtf8((String) ref);
                key_ = b;
                return b;
            } else {
                return (com.google.protobuf.ByteString) ref;
            }
        }

        public boolean hasValue() {
            return ((bitField0_ & 0x00000004) == 0x00000004);
        }

        public int getValue() {
            return value_;
        }

        private void initFields() {
            type_ = protos.MessageObjectProtos.MessageObject.MessageObjectType.NEW_ELEMENT;
            key_ = "";
            value_ = 0;
        }

        public final boolean isInitialized() {
            byte isInitialized = memoizedIsInitialized;
            if (isInitialized != -1) return isInitialized == 1;

            if (!hasType()) {
                memoizedIsInitialized = 0;
                return false;
            }
            if (!hasKey()) {
                memoizedIsInitialized = 0;
                return false;
            }
            memoizedIsInitialized = 1;
            return true;
        }

        public void writeTo(com.google.protobuf.CodedOutputStream output)
                throws java.io.IOException {
            getSerializedSize();
            if (((bitField0_ & 0x00000001) == 0x00000001)) {
                output.writeEnum(1, type_.getNumber());
            }
            if (((bitField0_ & 0x00000002) == 0x00000002)) {
                output.writeBytes(2, getKeyBytes());
            }
            if (((bitField0_ & 0x00000004) == 0x00000004)) {
                output.writeInt32(3, value_);
            }
            getUnknownFields().writeTo(output);
        }

        public int getSerializedSize() {
            int size = memoizedSerializedSize;
            if (size != -1) return size;

            size = 0;
            if (((bitField0_ & 0x00000001) == 0x00000001)) {
                size += com.google.protobuf.CodedOutputStream
                        .computeEnumSize(1, type_.getNumber());
            }
            if (((bitField0_ & 0x00000002) == 0x00000002)) {
                size += com.google.protobuf.CodedOutputStream
                        .computeBytesSize(2, getKeyBytes());
            }
            if (((bitField0_ & 0x00000004) == 0x00000004)) {
                size += com.google.protobuf.CodedOutputStream
                        .computeInt32Size(3, value_);
            }
            size += getUnknownFields().getSerializedSize();
            memoizedSerializedSize = size;
            return size;
        }

        @java.lang.Override
        protected java.lang.Object writeReplace()
                throws java.io.ObjectStreamException {
            return super.writeReplace();
        }

        public Builder newBuilderForType() {
            return newBuilder();
        }

        public Builder toBuilder() {
            return newBuilder(this);
        }

        @java.lang.Override
        protected Builder newBuilderForType(
                com.google.protobuf.GeneratedMessage.BuilderParent parent) {
            Builder builder = new Builder(parent);
            return builder;
        }

        public enum MessageObjectType
                implements com.google.protobuf.ProtocolMessageEnum {
            NEW_ELEMENT(0, 0),
            GET_ELEMENT(1, 1),
            RET_ELEMENT(2, 2),
            ;

            public static final int NEW_ELEMENT_VALUE = 0;
            public static final int GET_ELEMENT_VALUE = 1;
            public static final int RET_ELEMENT_VALUE = 2;
            private static final MessageObjectType[] VALUES = {
                    NEW_ELEMENT, GET_ELEMENT, RET_ELEMENT,
            };
            private static com.google.protobuf.Internal.EnumLiteMap<MessageObjectType>
                    internalValueMap =
                    new com.google.protobuf.Internal.EnumLiteMap<MessageObjectType>() {
                        public MessageObjectType findValueByNumber(int number) {
                            return MessageObjectType.valueOf(number);
                        }
                    };
            private final int index;
            private final int value;

            private MessageObjectType(int index, int value) {
                this.index = index;
                this.value = value;
            }

            public static MessageObjectType valueOf(int value) {
                switch (value) {
                    case 0:
                        return NEW_ELEMENT;
                    case 1:
                        return GET_ELEMENT;
                    case 2:
                        return RET_ELEMENT;
                    default:
                        return null;
                }
            }

            public static com.google.protobuf.Internal.EnumLiteMap<MessageObjectType>
            internalGetValueMap() {
                return internalValueMap;
            }

            public static final com.google.protobuf.Descriptors.EnumDescriptor
            getDescriptor() {
                return protos.MessageObjectProtos.MessageObject.getDescriptor().getEnumTypes().get(0);
            }

            public static MessageObjectType valueOf(
                    com.google.protobuf.Descriptors.EnumValueDescriptor desc) {
                if (desc.getType() != getDescriptor()) {
                    throw new java.lang.IllegalArgumentException(
                            "EnumValueDescriptor is not for this type.");
                }
                return VALUES[desc.getIndex()];
            }

            public final int getNumber() {
                return value;
            }

            public final com.google.protobuf.Descriptors.EnumValueDescriptor
            getValueDescriptor() {
                return getDescriptor().getValues().get(index);
            }

            public final com.google.protobuf.Descriptors.EnumDescriptor
            getDescriptorForType() {
                return getDescriptor();
            }

            // @@protoc_insertion_point(enum_scope:MessageObject.MessageObjectType)
        }

        public static final class Builder extends
                com.google.protobuf.GeneratedMessage.Builder<Builder>
                implements protos.MessageObjectProtos.MessageObjectOrBuilder {
            private int bitField0_;
            // required .MessageObject.MessageObjectType type = 1;
            private protos.MessageObjectProtos.MessageObject.MessageObjectType type_ = protos.MessageObjectProtos.MessageObject.MessageObjectType.NEW_ELEMENT;
            // required string key = 2;
            private java.lang.Object key_ = "";
            // optional int32 value = 3;
            private int value_;

            // Construct using protos.protos.MessageObjectProtos.MessageObject.newBuilder()
            private Builder() {
                maybeForceBuilderInitialization();
            }

            private Builder(BuilderParent parent) {
                super(parent);
                maybeForceBuilderInitialization();
            }

            public static final com.google.protobuf.Descriptors.Descriptor
            getDescriptor() {
                return internal_static_MessageObject_descriptor;
            }

            private static Builder create() {
                return new Builder();
            }

            protected com.google.protobuf.GeneratedMessage.FieldAccessorTable
            internalGetFieldAccessorTable() {
                return internal_static_MessageObject_fieldAccessorTable;
            }

            private void maybeForceBuilderInitialization() {
                if (com.google.protobuf.GeneratedMessage.alwaysUseFieldBuilders) {
                }
            }

            public Builder clear() {
                super.clear();
                type_ = protos.MessageObjectProtos.MessageObject.MessageObjectType.NEW_ELEMENT;
                bitField0_ = (bitField0_ & ~0x00000001);
                key_ = "";
                bitField0_ = (bitField0_ & ~0x00000002);
                value_ = 0;
                bitField0_ = (bitField0_ & ~0x00000004);
                return this;
            }

            public Builder clone() {
                return create().mergeFrom(buildPartial());
            }

            public com.google.protobuf.Descriptors.Descriptor
            getDescriptorForType() {
                return protos.MessageObjectProtos.MessageObject.getDescriptor();
            }

            public protos.MessageObjectProtos.MessageObject getDefaultInstanceForType() {
                return getDefaultInstance();
            }

            public protos.MessageObjectProtos.MessageObject build() {
                protos.MessageObjectProtos.MessageObject result = buildPartial();
                if (!result.isInitialized()) {
                    throw newUninitializedMessageException(result);
                }
                return result;
            }

            private protos.MessageObjectProtos.MessageObject buildParsed()
                    throws com.google.protobuf.InvalidProtocolBufferException {
                protos.MessageObjectProtos.MessageObject result = buildPartial();
                if (!result.isInitialized()) {
                    throw newUninitializedMessageException(
                            result).asInvalidProtocolBufferException();
                }
                return result;
            }

            public protos.MessageObjectProtos.MessageObject buildPartial() {
                protos.MessageObjectProtos.MessageObject result = new protos.MessageObjectProtos.MessageObject(this);
                int from_bitField0_ = bitField0_;
                int to_bitField0_ = 0;
                if (((from_bitField0_ & 0x00000001) == 0x00000001)) {
                    to_bitField0_ |= 0x00000001;
                }
                result.type_ = type_;
                if (((from_bitField0_ & 0x00000002) == 0x00000002)) {
                    to_bitField0_ |= 0x00000002;
                }
                result.key_ = key_;
                if (((from_bitField0_ & 0x00000004) == 0x00000004)) {
                    to_bitField0_ |= 0x00000004;
                }
                result.value_ = value_;
                result.bitField0_ = to_bitField0_;
                onBuilt();
                return result;
            }

            public Builder mergeFrom(com.google.protobuf.Message other) {
                if (other instanceof protos.MessageObjectProtos.MessageObject) {
                    return mergeFrom((protos.MessageObjectProtos.MessageObject) other);
                } else {
                    super.mergeFrom(other);
                    return this;
                }
            }

            public Builder mergeFrom(protos.MessageObjectProtos.MessageObject other) {
                if (other == getDefaultInstance()) return this;
                if (other.hasType()) {
                    setType(other.getType());
                }
                if (other.hasKey()) {
                    setKey(other.getKey());
                }
                if (other.hasValue()) {
                    setValue(other.getValue());
                }
                this.mergeUnknownFields(other.getUnknownFields());
                return this;
            }

            public final boolean isInitialized() {
                if (!hasType()) {

                    return false;
                }
                if (!hasKey()) {

                    return false;
                }
                return true;
            }

            public Builder mergeFrom(
                    com.google.protobuf.CodedInputStream input,
                    com.google.protobuf.ExtensionRegistryLite extensionRegistry)
                    throws java.io.IOException {
                com.google.protobuf.UnknownFieldSet.Builder unknownFields =
                        com.google.protobuf.UnknownFieldSet.newBuilder(
                                this.getUnknownFields());
                while (true) {
                    int tag = input.readTag();
                    switch (tag) {
                        case 0:
                            this.setUnknownFields(unknownFields.build());
                            onChanged();
                            return this;
                        default: {
                            if (!parseUnknownField(input, unknownFields,
                                    extensionRegistry, tag)) {
                                this.setUnknownFields(unknownFields.build());
                                onChanged();
                                return this;
                            }
                            break;
                        }
                        case 8: {
                            int rawValue = input.readEnum();
                            protos.MessageObjectProtos.MessageObject.MessageObjectType value = protos.MessageObjectProtos.MessageObject.MessageObjectType.valueOf(rawValue);
                            if (value == null) {
                                unknownFields.mergeVarintField(1, rawValue);
                            } else {
                                bitField0_ |= 0x00000001;
                                type_ = value;
                            }
                            break;
                        }
                        case 18: {
                            bitField0_ |= 0x00000002;
                            key_ = input.readBytes();
                            break;
                        }
                        case 24: {
                            bitField0_ |= 0x00000004;
                            value_ = input.readInt32();
                            break;
                        }
                    }
                }
            }

            public boolean hasType() {
                return ((bitField0_ & 0x00000001) == 0x00000001);
            }

            public protos.MessageObjectProtos.MessageObject.MessageObjectType getType() {
                return type_;
            }

            public Builder setType(protos.MessageObjectProtos.MessageObject.MessageObjectType value) {
                if (value == null) {
                    throw new NullPointerException();
                }
                bitField0_ |= 0x00000001;
                type_ = value;
                onChanged();
                return this;
            }

            public Builder clearType() {
                bitField0_ = (bitField0_ & ~0x00000001);
                type_ = protos.MessageObjectProtos.MessageObject.MessageObjectType.NEW_ELEMENT;
                onChanged();
                return this;
            }

            public boolean hasKey() {
                return ((bitField0_ & 0x00000002) == 0x00000002);
            }

            public String getKey() {
                java.lang.Object ref = key_;
                if (!(ref instanceof String)) {
                    String s = ((com.google.protobuf.ByteString) ref).toStringUtf8();
                    key_ = s;
                    return s;
                } else {
                    return (String) ref;
                }
            }

            void setKey(com.google.protobuf.ByteString value) {
                bitField0_ |= 0x00000002;
                key_ = value;
                onChanged();
            }

            public Builder setKey(String value) {
                if (value == null) {
                    throw new NullPointerException();
                }
                bitField0_ |= 0x00000002;
                key_ = value;
                onChanged();
                return this;
            }

            public Builder clearKey() {
                bitField0_ = (bitField0_ & ~0x00000002);
                key_ = getDefaultInstance().getKey();
                onChanged();
                return this;
            }

            public boolean hasValue() {
                return ((bitField0_ & 0x00000004) == 0x00000004);
            }

            public int getValue() {
                return value_;
            }

            public Builder setValue(int value) {
                bitField0_ |= 0x00000004;
                value_ = value;
                onChanged();
                return this;
            }

            public Builder clearValue() {
                bitField0_ = (bitField0_ & ~0x00000004);
                value_ = 0;
                onChanged();
                return this;
            }

            // @@protoc_insertion_point(builder_scope:MessageObject)
        }

        // @@protoc_insertion_point(class_scope:MessageObject)
    }

    // @@protoc_insertion_point(outer_class_scope)
}
