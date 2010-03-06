#pragma once

#include <WinSock2.h>
#include <ws2bth.h>
#include <InitGuid.h>

//SDP record generated by bthnscreate.exe
//TODO: this needs to be modified!
BYTE rgbSdpRecord[] = {
    0x35, 0x4d, 0x09, 0x00, 0x01, 0x35, 0x11, 0x1c,
    0x29, 0xf9, 0xc0, 0xfd, 0xbb, 0x6e, 0x47, 0x97,
    0x9f, 0xa9, 0x3e, 0xc9, 0xa8, 0x54, 0x29, 0x0c,
    0x09, 0x00, 0x04, 0x35, 0x0c, 0x35, 0x03, 0x19,
    0x01, 0x00, 0x35, 0x05, 0x19, 0x00, 0x03, 0x08,
    0x1a, 0x09, 0x00, 0x06, 0x35, 0x09, 0x09, 0x65,
    0x6e, 0x09, 0x00, 0x6a, 0x09, 0x01, 0x00, 0x09,
    0x00, 0x09, 0x35, 0x08, 0x35, 0x06, 0x19, 0x11,
    0x05, 0x09, 0x01, 0x00, 0x09, 0x01, 0x00, 0x25,
    0x06, 0x53, 0x65, 0x72, 0x69, 0x61, 0x6c
};
//server GUID
static const GUID ServerGuid = 
{ 0x29F9C0FD, 0xBB6E, 0x4797, { 0x9F, 0xA9, 0x3E, 0xC9, 0xA8, 0x54, 0x29, 0x0C } };
//SDP record size constant returned by bthnscreate.exe
#define SDP_RECORD_SIZE 0x0000004f
#define SDP_CHANNEL_OFFSET 40

class bluetoothdevice_win : public bluetoothdevice
{
public:
    bluetoothdevice_win(const std::string& rmtname, SOCKET_ADDRESS rmtaddr)
        : name(rmtname)
    {
        memset(&bthaddr, 0, sizeof(bthaddr));
        bthaddr.addressFamily = AF_BTH;
        bthaddr.btAddr = rmtaddr.lpSockaddr==NULL ? 0 : ((SOCKADDR_BTH *)rmtaddr.lpSockaddr)->btAddr;
        bthaddr.serviceClassId = ServerGuid;
        addr.lpSockaddr = (LPSOCKADDR)&bthaddr;
        addr.iSockaddrLength = sizeof(bthaddr);
    }
    bluetoothdevice_win(const std::string& rmtname)
        : name(rmtname)
    {
        memset(&bthaddr, 0, sizeof(bthaddr));
        bthaddr.addressFamily = AF_BTH;
        bthaddr.btAddr = 0;
        bthaddr.serviceClassId = ServerGuid;
        addr.lpSockaddr = (LPSOCKADDR)&bthaddr;
        addr.iSockaddrLength = sizeof(bthaddr);
    }
    virtual const std::string& Name() const {return name;}
    SOCKET_ADDRESS Addr() const {return addr;}
private:
    SOCKADDR_BTH bthaddr;
    const std::string name;
    SOCKET_ADDRESS addr;
};