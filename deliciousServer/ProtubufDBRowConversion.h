#pragma once

class DBRow;

namespace ProtocolBuffer{
    class User;
}

class ProtubufDBRowConversion
{
public:
    static bool Convert(const DBRow& from, ::ProtocolBuffer::User& to);
    static bool Convert(const ::ProtocolBuffer::User& from, DBRow& to);
};
