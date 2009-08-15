// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "Message.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>

namespace protorpc {

void protobuf_ShutdownFile_Message_2eproto() {
  delete Message::default_instance_;
  delete DescriptorResponse::default_instance_;
}

void protobuf_AddDesc_Message_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

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

Message::Message() {
  SharedCtor();
}

void Message::InitAsDefaultInstance() {
}

Message::Message(const Message& from) {
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
}

bool Message::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional .protorpc.Type type = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
        int value;
        DO_(::google::protobuf::internal::WireFormatLite::ReadEnum(input, &value));
        if (protorpc::Type_IsValid(value)) {
          set_type(static_cast< protorpc::Type >(value));
        }
        if (input->ExpectTag(16)) goto parse_id;
        break;
      }
      
      // optional uint32 id = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_id:
        DO_(::google::protobuf::internal::WireFormatLite::ReadUInt32(
              input, &id_));
        _set_bit(1);
        if (input->ExpectTag(26)) goto parse_name;
        break;
      }
      
      // optional string name = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_name:
        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
              input, this->mutable_name()));
        if (input->ExpectTag(34)) goto parse_buffer;
        break;
      }
      
      // optional bytes buffer = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_buffer:
        DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
              input, this->mutable_buffer()));
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Message::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional .protorpc.Type type = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->type(), output);
  }
  
  // optional uint32 id = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(2, this->id(), output);
  }
  
  // optional string name = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormatLite::WriteString(
      3, this->name(), output);
  }
  
  // optional bytes buffer = 4;
  if (_has_bit(3)) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      4, this->buffer(), output);
  }
  
}

int Message::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional .protorpc.Type type = 1;
    if (has_type()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->type());
    }
    
    // optional uint32 id = 2;
    if (has_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->id());
    }
    
    // optional string name = 3;
    if (has_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->name());
    }
    
    // optional bytes buffer = 4;
    if (has_buffer()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->buffer());
    }
    
  }
  _cached_size_ = total_size;
  return total_size;
}

void Message::CheckTypeAndMergeFrom(
    const ::google::protobuf::MessageLite& from) {
  MergeFrom(*::google::protobuf::down_cast<const Message*>(&from));
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
}

void Message::CopyFrom(const Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Message::IsInitialized() const {
  
  return true;
}

void Message::Swap(Message* other) {
  if (other != this) {
    std::swap(type_, other->type_);
    std::swap(id_, other->id_);
    std::swap(name_, other->name_);
    std::swap(buffer_, other->buffer_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::std::string Message::GetTypeName() const {
  return "protorpc.Message";
}


// ===================================================================

const ::std::string DescriptorResponse::_default_desc_;
const ::std::string DescriptorResponse::_default_servicename_;
#ifndef _MSC_VER
const int DescriptorResponse::kDescFieldNumber;
const int DescriptorResponse::kDepsFieldNumber;
const int DescriptorResponse::kServiceNameFieldNumber;
#endif  // !_MSC_VER

DescriptorResponse::DescriptorResponse() {
  SharedCtor();
}

void DescriptorResponse::InitAsDefaultInstance() {
}

DescriptorResponse::DescriptorResponse(const DescriptorResponse& from) {
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
}

bool DescriptorResponse::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required bytes desc = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
        DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
              input, this->mutable_desc()));
        if (input->ExpectTag(18)) goto parse_deps;
        break;
      }
      
      // repeated .protorpc.DescriptorResponse deps = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_deps:
        DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
              input, add_deps()));
        if (input->ExpectTag(18)) goto parse_deps;
        if (input->ExpectTag(26)) goto parse_serviceName;
        break;
      }
      
      // optional string serviceName = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_serviceName:
        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
              input, this->mutable_servicename()));
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void DescriptorResponse::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required bytes desc = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      1, this->desc(), output);
  }
  
  // repeated .protorpc.DescriptorResponse deps = 2;
  for (int i = 0; i < this->deps_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageNoVirtual(
      2, this->deps(i), output);
  }
  
  // optional string serviceName = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormatLite::WriteString(
      3, this->servicename(), output);
  }
  
}

int DescriptorResponse::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required bytes desc = 1;
    if (has_desc()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->desc());
    }
    
    // optional string serviceName = 3;
    if (has_servicename()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->servicename());
    }
    
  }
  // repeated .protorpc.DescriptorResponse deps = 2;
  total_size += 1 * this->deps_size();
  for (int i = 0; i < this->deps_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->deps(i));
  }
  
  _cached_size_ = total_size;
  return total_size;
}

void DescriptorResponse::CheckTypeAndMergeFrom(
    const ::google::protobuf::MessageLite& from) {
  MergeFrom(*::google::protobuf::down_cast<const DescriptorResponse*>(&from));
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
}

void DescriptorResponse::CopyFrom(const DescriptorResponse& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool DescriptorResponse::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  for (int i = 0; i < deps_size(); i++) {
    if (!this->deps(i).IsInitialized()) return false;
  }
  return true;
}

void DescriptorResponse::Swap(DescriptorResponse* other) {
  if (other != this) {
    std::swap(desc_, other->desc_);
    deps_.Swap(&other->deps_);
    std::swap(servicename_, other->servicename_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::std::string DescriptorResponse::GetTypeName() const {
  return "protorpc.DescriptorResponse";
}


}  // namespace protorpc
