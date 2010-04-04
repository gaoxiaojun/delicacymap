#pragma once

class DBRow;

namespace ProtocolBuffer{
    class User;
}

class ProtubufDBRowConversion
{
public:
    // User
    static bool Convert(const DBRow& from, ::ProtocolBuffer::User& to);
    static bool Convert(const ::ProtocolBuffer::User& from, DBRow& to);

    // Restaurant
    static bool Convert(const DBRow&from, ::ProtocolBuffer::Restaurant& to);

    // Comment
    static bool Convert(const DBRow&from, ::ProtocolBuffer::Comment& to);
};
