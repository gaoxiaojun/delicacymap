// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Message.proto

#ifndef PROTOBUF_Message_2eproto__INCLUDED
#define PROTOBUF_Message_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2003000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
// @@protoc_insertion_point(includes)

namespace protorpc {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_Message_2eproto();
void protobuf_AssignDesc_Message_2eproto();
void protobuf_ShutdownFile_Message_2eproto();

class Message;

enum Type {
  REQUEST = 1,
  RESPONSE = 2,
  RESPONSE_CANCEL = 3,
  RESPONSE_FAILED = 4,
  RESPONSE_NOT_IMPLEMENTED = 5,
  DISCONNECT = 6,
  DESCRIPTOR_REQUEST = 7,
  DESCRIPTOR_RESPONSE = 8,
  MESSAGE = 9
};
bool Type_IsValid(int value);
const Type Type_MIN = REQUEST;
const Type Type_MAX = MESSAGE;
const int Type_ARRAYSIZE = Type_MAX + 1;

enum FunctionID {
  GetRestaurants = 1,
  GetLastestCommentsOfRestaurant = 2,
  GetLastestCommentsByUser = 3,
  GetCommentsOfUserSince = 4,
  GetCommentsOfRestaurantSince = 5,
  UserLogin = 6,
  GetUserInfo = 7,
  GetRelatedUsers = 8,
  AddCommentForRestaurant = 9,
  AddRestaurant = 10,
  UpdateUserInfo = 11,
  SetUserRelation = 12,
  Search = 13,
  GetSubscribtionInfo = 14
};
bool FunctionID_IsValid(int value);
const FunctionID FunctionID_MIN = GetRestaurants;
const FunctionID FunctionID_MAX = GetSubscribtionInfo;
const int FunctionID_ARRAYSIZE = FunctionID_MAX + 1;

// ===================================================================

class Message : public ::google::protobuf::MessageLite {
 public:
  Message();
  virtual ~Message();
  
  Message(const Message& from);
  
  inline Message& operator=(const Message& from) {
    CopyFrom(from);
    return *this;
  }
  
  static const Message& default_instance();
  
  void Swap(Message* other);
  
  // implements Message ----------------------------------------------
  
  Message* New() const;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);
  void CopyFrom(const Message& from);
  void MergeFrom(const Message& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::std::string GetTypeName() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // optional .protorpc.Type type = 1;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 1;
  inline protorpc::Type type() const;
  inline void set_type(protorpc::Type value);
  
  // optional .protorpc.FunctionID method_id = 2;
  inline bool has_method_id() const;
  inline void clear_method_id();
  static const int kMethodIdFieldNumber = 2;
  inline protorpc::FunctionID method_id() const;
  inline void set_method_id(protorpc::FunctionID value);
  
  // optional uint32 id = 3;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 3;
  inline ::google::protobuf::uint32 id() const;
  inline void set_id(::google::protobuf::uint32 value);
  
  // optional string name = 4;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 4;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  
  // optional bytes buffer = 5;
  inline bool has_buffer() const;
  inline void clear_buffer();
  static const int kBufferFieldNumber = 5;
  inline const ::std::string& buffer() const;
  inline void set_buffer(const ::std::string& value);
  inline void set_buffer(const char* value);
  inline void set_buffer(const void* value, size_t size);
  inline ::std::string* mutable_buffer();
  
  // @@protoc_insertion_point(class_scope:protorpc.Message)
 private:
  mutable int _cached_size_;
  
  int type_;
  int method_id_;
  ::google::protobuf::uint32 id_;
  ::std::string* name_;
  static const ::std::string _default_name_;
  ::std::string* buffer_;
  static const ::std::string _default_buffer_;
  friend void  protobuf_AddDesc_Message_2eproto();
  friend void protobuf_AssignDesc_Message_2eproto();
  friend void protobuf_ShutdownFile_Message_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static Message* default_instance_;
};
// ===================================================================


// ===================================================================

// Message

// optional .protorpc.Type type = 1;
inline bool Message::has_type() const {
  return _has_bit(0);
}
inline void Message::clear_type() {
  type_ = 1;
  _clear_bit(0);
}
inline protorpc::Type Message::type() const {
  return static_cast< protorpc::Type >(type_);
}
inline void Message::set_type(protorpc::Type value) {
  GOOGLE_DCHECK(protorpc::Type_IsValid(value));
  _set_bit(0);
  type_ = value;
}

// optional .protorpc.FunctionID method_id = 2;
inline bool Message::has_method_id() const {
  return _has_bit(1);
}
inline void Message::clear_method_id() {
  method_id_ = 1;
  _clear_bit(1);
}
inline protorpc::FunctionID Message::method_id() const {
  return static_cast< protorpc::FunctionID >(method_id_);
}
inline void Message::set_method_id(protorpc::FunctionID value) {
  GOOGLE_DCHECK(protorpc::FunctionID_IsValid(value));
  _set_bit(1);
  method_id_ = value;
}

// optional uint32 id = 3;
inline bool Message::has_id() const {
  return _has_bit(2);
}
inline void Message::clear_id() {
  id_ = 0u;
  _clear_bit(2);
}
inline ::google::protobuf::uint32 Message::id() const {
  return id_;
}
inline void Message::set_id(::google::protobuf::uint32 value) {
  _set_bit(2);
  id_ = value;
}

// optional string name = 4;
inline bool Message::has_name() const {
  return _has_bit(3);
}
inline void Message::clear_name() {
  if (name_ != &_default_name_) {
    name_->clear();
  }
  _clear_bit(3);
}
inline const ::std::string& Message::name() const {
  return *name_;
}
inline void Message::set_name(const ::std::string& value) {
  _set_bit(3);
  if (name_ == &_default_name_) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void Message::set_name(const char* value) {
  _set_bit(3);
  if (name_ == &_default_name_) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void Message::set_name(const char* value, size_t size) {
  _set_bit(3);
  if (name_ == &_default_name_) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Message::mutable_name() {
  _set_bit(3);
  if (name_ == &_default_name_) {
    name_ = new ::std::string;
  }
  return name_;
}

// optional bytes buffer = 5;
inline bool Message::has_buffer() const {
  return _has_bit(4);
}
inline void Message::clear_buffer() {
  if (buffer_ != &_default_buffer_) {
    buffer_->clear();
  }
  _clear_bit(4);
}
inline const ::std::string& Message::buffer() const {
  return *buffer_;
}
inline void Message::set_buffer(const ::std::string& value) {
  _set_bit(4);
  if (buffer_ == &_default_buffer_) {
    buffer_ = new ::std::string;
  }
  buffer_->assign(value);
}
inline void Message::set_buffer(const char* value) {
  _set_bit(4);
  if (buffer_ == &_default_buffer_) {
    buffer_ = new ::std::string;
  }
  buffer_->assign(value);
}
inline void Message::set_buffer(const void* value, size_t size) {
  _set_bit(4);
  if (buffer_ == &_default_buffer_) {
    buffer_ = new ::std::string;
  }
  buffer_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Message::mutable_buffer() {
  _set_bit(4);
  if (buffer_ == &_default_buffer_) {
    buffer_ = new ::std::string;
  }
  return buffer_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace protorpc

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_Message_2eproto__INCLUDED
