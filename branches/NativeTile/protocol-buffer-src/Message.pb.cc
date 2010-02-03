// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "Message.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
// @@protoc_insertion_point(includes)

namespace protorpc {

void protobuf_ShutdownFile_Message_2eproto() {
  delete Message::default_instance_;
}

void protobuf_AddDesc_Message_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  Message::default_instance_ = new Message();
  Message::default_instance_->InitAsDefaultInstance();
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
    case 9:
      return true;
    default:
      return false;
  }
}

bool FunctionID_IsValid(int value) {
  switch(value) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
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
const int Message::kMethodIdFieldNumber;
const int Message::kIdFieldNumber;
const int Message::kNameFieldNumber;
const int Message::kBufferFieldNumber;
#endif  // !_MSC_VER

Message::Message()
  : ::google::protobuf::MessageLite() {
  SharedCtor();
}

void Message::InitAsDefaultInstance() {
}

Message::Message(const Message& from)
  : ::google::protobuf::MessageLite() {
  SharedCtor();
  MergeFrom(from);
}

void Message::SharedCtor() {
  _cached_size_ = 0;
  type_ = 1;
  method_id_ = 1;
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

void Message::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
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
    method_id_ = 1;
    id_ = 0u;
    if (_has_bit(3)) {
      if (name_ != &_default_name_) {
        name_->clear();
      }
    }
    if (_has_bit(4)) {
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
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (protorpc::Type_IsValid(value)) {
            set_type(static_cast< protorpc::Type >(value));
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_method_id;
        break;
      }
      
      // optional .protorpc.FunctionID method_id = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_method_id:
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (protorpc::FunctionID_IsValid(value)) {
            set_method_id(static_cast< protorpc::FunctionID >(value));
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_id;
        break;
      }
      
      // optional uint32 id = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_id:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &id_)));
          _set_bit(2);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_name;
        break;
      }
      
      // optional string name = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_name:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_name()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(42)) goto parse_buffer;
        break;
      }
      
      // optional bytes buffer = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_buffer:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_buffer()));
        } else {
          goto handle_uninterpreted;
        }
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
  
  // optional .protorpc.FunctionID method_id = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      2, this->method_id(), output);
  }
  
  // optional uint32 id = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(3, this->id(), output);
  }
  
  // optional string name = 4;
  if (_has_bit(3)) {
    ::google::protobuf::internal::WireFormatLite::WriteString(
      4, this->name(), output);
  }
  
  // optional bytes buffer = 5;
  if (_has_bit(4)) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      5, this->buffer(), output);
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
    
    // optional .protorpc.FunctionID method_id = 2;
    if (has_method_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->method_id());
    }
    
    // optional uint32 id = 3;
    if (has_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->id());
    }
    
    // optional string name = 4;
    if (has_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->name());
    }
    
    // optional bytes buffer = 5;
    if (has_buffer()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->buffer());
    }
    
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
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
      set_method_id(from.method_id());
    }
    if (from._has_bit(2)) {
      set_id(from.id());
    }
    if (from._has_bit(3)) {
      set_name(from.name());
    }
    if (from._has_bit(4)) {
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
    std::swap(method_id_, other->method_id_);
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


// @@protoc_insertion_point(namespace_scope)

}  // namespace protorpc

// @@protoc_insertion_point(global_scope)
