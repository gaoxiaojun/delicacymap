// Generated by the protocol buffer compiler.  DO NOT EDIT!

#include "Message.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format_inl.h>

namespace protorpc {

namespace {

const ::google::protobuf::Descriptor* Message_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Message_reflection_ = NULL;
const ::google::protobuf::Descriptor* DescriptorResponse_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  DescriptorResponse_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* Type_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_Message_2eproto() {
  protobuf_AddDesc_Message_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "Message.proto");
  GOOGLE_CHECK(file != NULL);
  Message_descriptor_ = file->message_type(0);
  static const int Message_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, buffer_),
  };
  Message_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Message_descriptor_,
      Message::default_instance_,
      Message_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Message));
  DescriptorResponse_descriptor_ = file->message_type(1);
  static const int DescriptorResponse_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DescriptorResponse, desc_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DescriptorResponse, deps_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DescriptorResponse, servicename_),
  };
  DescriptorResponse_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      DescriptorResponse_descriptor_,
      DescriptorResponse::default_instance_,
      DescriptorResponse_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DescriptorResponse, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DescriptorResponse, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(DescriptorResponse));
  Type_descriptor_ = file->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_Message_2eproto);
}

void protobuf_RegisterTypes() {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Message_descriptor_, &Message::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    DescriptorResponse_descriptor_, &DescriptorResponse::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_Message_2eproto() {
  delete Message::default_instance_;
  delete Message_reflection_;
  delete DescriptorResponse::default_instance_;
  delete DescriptorResponse_reflection_;
}

void protobuf_AddDesc_Message_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\rMessage.proto\022\010protorpc\"Q\n\007Message\022\034\n\004"
    "type\030\001 \001(\0162\016.protorpc.Type\022\n\n\002id\030\002 \001(\r\022\014"
    "\n\004name\030\003 \001(\t\022\016\n\006buffer\030\004 \001(\014\"c\n\022Descript"
    "orResponse\022\014\n\004desc\030\001 \002(\014\022*\n\004deps\030\002 \003(\0132\034"
    ".protorpc.DescriptorResponse\022\023\n\013serviceN"
    "ame\030\003 \001(\t*\252\001\n\004Type\022\013\n\007REQUEST\020\001\022\014\n\010RESPO"
    "NSE\020\002\022\023\n\017RESPONSE_CANCEL\020\003\022\023\n\017RESPONSE_F"
    "AILED\020\004\022\034\n\030RESPONSE_NOT_IMPLEMENTED\020\005\022\016\n"
    "\nDISCONNECT\020\006\022\026\n\022DESCRIPTOR_REQUEST\020\007\022\027\n"
    "\023DESCRIPTOR_RESPONSE\020\010B%\n\025com.likbilen.p"
    "rotorpcB\014MessageProto", 421);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "Message.proto", &protobuf_RegisterTypes);
  Message::default_instance_ = new Message();
  DescriptorResponse::default_instance_ = new DescriptorResponse();
  Message::default_instance_->InitAsDefaultInstance();
  DescriptorResponse::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_Message_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_Message_2eproto {
  StaticDescriptorInitializer_Message_2eproto() {
    protobuf_AddDesc_Message_2eproto();
  }
} static_descriptor_initializer_Message_2eproto_;

const ::google::protobuf::EnumDescriptor* Type_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Type_descriptor_;
}
bool Type_IsValid(int value) {
  switch(value) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
      return true;
    default:
      return false;
  }
}


// ===================================================================

const ::std::string Message::_default_name_;
const ::std::string Message::_default_buffer_;
#ifndef _MSC_VER
const int Message::kTypeFieldNumber;
const int Message::kIdFieldNumber;
const int Message::kNameFieldNumber;
const int Message::kBufferFieldNumber;
#endif  // !_MSC_VER

Message::Message()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Message::InitAsDefaultInstance() {}

Message::Message(const Message& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Message::SharedCtor() {
  _cached_size_ = 0;
  type_ = 1;
  id_ = 0u;
  name_ = const_cast< ::std::string*>(&_default_name_);
  buffer_ = const_cast< ::std::string*>(&_default_buffer_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Message::~Message() {
  SharedDtor();
}

void Message::SharedDtor() {
  if (name_ != &_default_name_) {
    delete name_;
  }
  if (buffer_ != &_default_buffer_) {
    delete buffer_;
  }
  if (this != default_instance_) {
  }
}

const ::google::protobuf::Descriptor* Message::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Message_descriptor_;
}

const Message& Message::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_Message_2eproto();  return *default_instance_;
}

Message* Message::default_instance_ = NULL;

Message* Message::New() const {
  return new Message;
}

void Message::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    type_ = 1;
    id_ = 0u;
    if (_has_bit(2)) {
      if (name_ != &_default_name_) {
        name_->clear();
      }
    }
    if (_has_bit(3)) {
      if (buffer_ != &_default_buffer_) {
        buffer_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Message::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormat::GetTagFieldNumber(tag)) {
      // optional .protorpc.Type type = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormat::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormat::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
        int value;
        DO_(::google::protobuf::internal::WireFormat::ReadEnum(input, &value));
        if (protorpc::Type_IsValid(value)) {
          set_type(static_cast< protorpc::Type >(value));
        } else {
          mutable_unknown_fields()->AddVarint(1, value);
        }
        if (input->ExpectTag(16)) goto parse_id;
        break;
      }
      
      // optional uint32 id = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormat::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormat::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_id:
        DO_(::google::protobuf::internal::WireFormat::ReadUInt32(
              input, &id_));
        _set_bit(1);
        if (input->ExpectTag(26)) goto parse_name;
        break;
      }
      
      // optional string name = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormat::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormat::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_name:
        DO_(::google::protobuf::internal::WireFormat::ReadString(input, mutable_name()));
        if (input->ExpectTag(34)) goto parse_buffer;
        break;
      }
      
      // optional bytes buffer = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormat::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormat::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_buffer:
        DO_(::google::protobuf::internal::WireFormat::ReadBytes(input, mutable_buffer()));
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormat::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormat::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Message::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  ::google::protobuf::uint8* raw_buffer = output->GetDirectBufferForNBytesAndAdvance(_cached_size_);
  if (raw_buffer != NULL) {
    Message::SerializeWithCachedSizesToArray(raw_buffer);
    return;
  }
  
  // optional .protorpc.Type type = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::WriteEnum(1, this->type(), output);
  }
  
  // optional uint32 id = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormat::WriteUInt32(2, this->id(), output);
  }
  
  // optional string name = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormat::WriteString(3, this->name(), output);
  }
  
  // optional bytes buffer = 4;
  if (_has_bit(3)) {
    ::google::protobuf::internal::WireFormat::WriteBytes(4, this->buffer(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Message::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional .protorpc.Type type = 1;
  if (_has_bit(0)) {
    target = ::google::protobuf::internal::WireFormat::WriteEnumToArray(1, this->type(), target);
  }
  
  // optional uint32 id = 2;
  if (_has_bit(1)) {
    target = ::google::protobuf::internal::WireFormat::WriteUInt32ToArray(2, this->id(), target);
  }
  
  // optional string name = 3;
  if (_has_bit(2)) {
    target = ::google::protobuf::internal::WireFormat::WriteStringToArray(3, this->name(), target);
  }
  
  // optional bytes buffer = 4;
  if (_has_bit(3)) {
    target = ::google::protobuf::internal::WireFormat::WriteBytesToArray(4, this->buffer(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Message::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional .protorpc.Type type = 1;
    if (has_type()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormat::EnumSize(this->type());
    }
    
    // optional uint32 id = 2;
    if (has_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormat::UInt32Size(
          this->id());
    }
    
    // optional string name = 3;
    if (has_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormat::StringSize(this->name());
    }
    
    // optional bytes buffer = 4;
    if (has_buffer()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormat::BytesSize(this->buffer());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  _cached_size_ = total_size;
  return total_size;
}

void Message::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Message* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Message*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Message::MergeFrom(const Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_type(from.type());
    }
    if (from._has_bit(1)) {
      set_id(from.id());
    }
    if (from._has_bit(2)) {
      set_name(from.name());
    }
    if (from._has_bit(3)) {
      set_buffer(from.buffer());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Message::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Message::CopyFrom(const Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Message::Swap(Message* other) {
  if (other != this) {
    std::swap(type_, other->type_);
    std::swap(id_, other->id_);
    std::swap(name_, other->name_);
    std::swap(buffer_, other->buffer_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

bool Message::IsInitialized() const {
  
  return true;
}

const ::google::protobuf::Descriptor* Message::GetDescriptor() const {
  return descriptor();
}

const ::google::protobuf::Reflection* Message::GetReflection() const {
  protobuf_AssignDescriptorsOnce();
  return Message_reflection_;
}

// ===================================================================

const ::std::string DescriptorResponse::_default_desc_;
const ::std::string DescriptorResponse::_default_servicename_;
#ifndef _MSC_VER
const int DescriptorResponse::kDescFieldNumber;
const int DescriptorResponse::kDepsFieldNumber;
const int DescriptorResponse::kServiceNameFieldNumber;
#endif  // !_MSC_VER

DescriptorResponse::DescriptorResponse()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void DescriptorResponse::InitAsDefaultInstance() {}

DescriptorResponse::DescriptorResponse(const DescriptorResponse& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void DescriptorResponse::SharedCtor() {
  _cached_size_ = 0;
  desc_ = const_cast< ::std::string*>(&_default_desc_);
  servicename_ = const_cast< ::std::string*>(&_default_servicename_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

DescriptorResponse::~DescriptorResponse() {
  SharedDtor();
}

void DescriptorResponse::SharedDtor() {
  if (desc_ != &_default_desc_) {
    delete desc_;
  }
  if (servicename_ != &_default_servicename_) {
    delete servicename_;
  }
  if (this != default_instance_) {
  }
}

const ::google::protobuf::Descriptor* DescriptorResponse::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return DescriptorResponse_descriptor_;
}

const DescriptorResponse& DescriptorResponse::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_Message_2eproto();  return *default_instance_;
}

DescriptorResponse* DescriptorResponse::default_instance_ = NULL;

DescriptorResponse* DescriptorResponse::New() const {
  return new DescriptorResponse;
}

void DescriptorResponse::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (_has_bit(0)) {
      if (desc_ != &_default_desc_) {
        desc_->clear();
      }
    }
    if (_has_bit(2)) {
      if (servicename_ != &_default_servicename_) {
        servicename_->clear();
      }
    }
  }
  deps_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool DescriptorResponse::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormat::GetTagFieldNumber(tag)) {
      // required bytes desc = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormat::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormat::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
        DO_(::google::protobuf::internal::WireFormat::ReadBytes(input, mutable_desc()));
        if (input->ExpectTag(18)) goto parse_deps;
        break;
      }
      
      // repeated .protorpc.DescriptorResponse deps = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormat::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormat::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_deps:
        DO_(::google::protobuf::internal::WireFormat::ReadMessageNoVirtual(
             input, add_deps()));
        if (input->ExpectTag(18)) goto parse_deps;
        if (input->ExpectTag(26)) goto parse_serviceName;
        break;
      }
      
      // optional string serviceName = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormat::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormat::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_serviceName:
        DO_(::google::protobuf::internal::WireFormat::ReadString(input, mutable_servicename()));
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormat::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormat::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void DescriptorResponse::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  ::google::protobuf::uint8* raw_buffer = output->GetDirectBufferForNBytesAndAdvance(_cached_size_);
  if (raw_buffer != NULL) {
    DescriptorResponse::SerializeWithCachedSizesToArray(raw_buffer);
    return;
  }
  
  // required bytes desc = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::WriteBytes(1, this->desc(), output);
  }
  
  // repeated .protorpc.DescriptorResponse deps = 2;
  for (int i = 0; i < this->deps_size(); i++) {
    ::google::protobuf::internal::WireFormat::WriteMessageNoVirtual(2, this->deps(i), output);
  }
  
  // optional string serviceName = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormat::WriteString(3, this->servicename(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* DescriptorResponse::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required bytes desc = 1;
  if (_has_bit(0)) {
    target = ::google::protobuf::internal::WireFormat::WriteBytesToArray(1, this->desc(), target);
  }
  
  // repeated .protorpc.DescriptorResponse deps = 2;
  for (int i = 0; i < this->deps_size(); i++) {
    target = ::google::protobuf::internal::WireFormat::WriteMessageNoVirtualToArray(2, this->deps(i), target);
  }
  
  // optional string serviceName = 3;
  if (_has_bit(2)) {
    target = ::google::protobuf::internal::WireFormat::WriteStringToArray(3, this->servicename(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int DescriptorResponse::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required bytes desc = 1;
    if (has_desc()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormat::BytesSize(this->desc());
    }
    
    // optional string serviceName = 3;
    if (has_servicename()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormat::StringSize(this->servicename());
    }
    
  }
  // repeated .protorpc.DescriptorResponse deps = 2;
  total_size += 1 * this->deps_size();
  for (int i = 0; i < this->deps_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormat::MessageSizeNoVirtual(
        this->deps(i));
  }
  
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  _cached_size_ = total_size;
  return total_size;
}

void DescriptorResponse::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const DescriptorResponse* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const DescriptorResponse*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void DescriptorResponse::MergeFrom(const DescriptorResponse& from) {
  GOOGLE_CHECK_NE(&from, this);
  deps_.MergeFrom(from.deps_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_desc(from.desc());
    }
    if (from._has_bit(2)) {
      set_servicename(from.servicename());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void DescriptorResponse::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void DescriptorResponse::CopyFrom(const DescriptorResponse& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void DescriptorResponse::Swap(DescriptorResponse* other) {
  if (other != this) {
    std::swap(desc_, other->desc_);
    deps_.Swap(&other->deps_);
    std::swap(servicename_, other->servicename_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

bool DescriptorResponse::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  for (int i = 0; i < deps_size(); i++) {
    if (!this->deps(i).IsInitialized()) return false;
  }
  return true;
}

const ::google::protobuf::Descriptor* DescriptorResponse::GetDescriptor() const {
  return descriptor();
}

const ::google::protobuf::Reflection* DescriptorResponse::GetReflection() const {
  protobuf_AssignDescriptorsOnce();
  return DescriptorResponse_reflection_;
}

}  // namespace protorpc
