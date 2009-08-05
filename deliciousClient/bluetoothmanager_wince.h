#pragma once
#include "bluetoothmanager.h"
#include <WinDef.h>
#include <winsock2.h>
#include <vector>

class bluetoothdevice_win;

class bluetoothmanager_wince :
    public bluetoothmanager
{
public:
    bluetoothmanager_wince(void);

    bool IsUseable();
    bool IsDiscoverable();

    int Discover();
    int NumOfDevices();
    bluetoothdevice* GetDevice(int id);
    bluetoothdevice* GetLocalDevice();

    int SendToDevice(bluetoothdevice* device, void* data, size_t cb);
    RECVFUNC SetRecvCallback(RECVFUNC func);

    virtual ~bluetoothmanager_wince(void);

private:
    int OpenServerConnection(BYTE *rgbSdpRecord, int cSdpRecord, int iChannelOffset);
    int RegisterService(BYTE *rgbSdpRecord, int cSdpRecord, int iChannelOffset, UCHAR channel);
    static DWORD WINAPI ReadThread(LPVOID param);
    static bool ACK(SOCKET s, int retrytimes);

private:
    std::vector<bluetoothdevice_win*> m_devices;
    DWORD m_bthmode;
    RECVFUNC m_callback;
    HANDLE m_hReadThread;
    SOCKET m_listensock;
};
